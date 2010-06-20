//
//  LCIPRegistryNetworkList.m
//  Lithium Console
//
//  Created by James Wilson on 2/10/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCIPRegistryNetworkList.h"


@implementation LCIPRegistryNetworkList

+ (LCIPRegistryNetworkList *) networkListForCustomer:(id)initCustomer
{ 
	return [[[LCIPRegistryNetworkList alloc] initForCustomer:initCustomer] autorelease];
}

- (LCIPRegistryNetworkList *) initForCustomer:(id)initCustomer
{
	self = [super init];
	if (!self) return nil;
	
	children = [[NSMutableArray array] retain];
	childDictionary = [[NSMutableDictionary dictionary] retain];
	self.customer = initCustomer;
	
	return self;
}

- (void) dealloc
{
	if (refreshXMLRequest)
	{ [refreshXMLRequest cancel]; [refreshXMLRequest release]; }
	[customer release];
	[children release];
	[childDictionary release];
	[super dealloc];
}

#pragma mark "Refresh Methods"

- (void) scanNetwork:(NSString *)address mask:(NSString *)mask
{
	/* Check state */
	if (!customer) return;
	if (refreshXMLRequest) return;
	
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"network"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"address" stringValue:address]];
	[rootnode addChild:[NSXMLNode elementWithName:@"mask" stringValue:mask]];
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:customer
												  resource:[customer resourceAddress] 
													entity:[customer entityAddress] 
												   xmlname:@"ipregistry_network_scan" 
													refsec:0 
													xmlout:xmldoc] retain];
	[refreshXMLRequest setDelegate:self];
	[refreshXMLRequest setXMLDelegate:self];
	[refreshXMLRequest setPriority:XMLREQ_PRIO_HIGH];
	[refreshXMLRequest performAsyncRequest];
	[self setScanInProgress:YES];
	[self setRefreshInProgress:YES];	
}

- (void) refreshWithPriority:(int)priority
{
	/* Check state */
	if (!customer) return;
	if (refreshXMLRequest) return;
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:customer
												  resource:[customer resourceAddress] 
													entity:[customer entityAddress] 
												   xmlname:@"ipregistry_network_list" 
													refsec:0 
													xmlout:nil] retain];
	[refreshXMLRequest setDelegate:self];
	[refreshXMLRequest setXMLDelegate:self];
	[refreshXMLRequest setPriority:priority];
	[refreshXMLRequest performAsyncRequest];
	[self setScanInProgress:NO];
	[self setRefreshInProgress:YES];
}

- (void) highPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_HIGH]; }

- (void) normalPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_NORMAL]; }

- (void) lowPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_LOW]; }

- (void) XMLRequestPreParse:(id)sender
{
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{	
	/* Check for start of a network */
	if ([element isEqualToString:@"network_entry"])
	{
		curNetwork = [LCIPRegistryNetwork new];
		curNetwork.parent = self;
	}
	
	/* Check for start of an IP entry */
	else if ([element isEqualToString:@"ip_entry"])
	{
		curEntry = [LCIPRegistryEntry new];
		curEntry.parent = curNetwork;
	}

	/* Free previous curXMLString */
	if (curXMLString)
	{
		[curXMLString release];
		curXMLString = nil;
	}
}

- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	/* Append characters to curXMLString */
	if (!curXMLString)
	{ curXMLString = [[NSMutableString stringWithString:string] retain]; }
	else
	{ [curXMLString appendString:string]; }
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Check for the address of the network */
	if ([element isEqualToString:@"address"] && curNetwork)
	{
		/* Check for existing network */
		if ([childDictionary objectForKey:curXMLString])
		{
			/* Use existing */
			[curNetwork release];
			curNetwork = [childDictionary objectForKey:curXMLString];
		}
		else
		{
			/* Continue to use new */
			curNetwork.address = curXMLString;
			[self insertObject:curNetwork inChildrenAtIndex:[children count]];
			[curNetwork autorelease];
		}
	}
	
	/* Check for the address of an entry */
	else if ([element isEqualToString:@"ip"] && curEntry)
	{		
		/* Check for existing network */
		if ([curNetwork.childDictionary objectForKey:curXMLString])
		{
			/* Use existing */
			[curEntry release];
			curEntry = [curNetwork.childDictionary objectForKey:curXMLString];
		}
		else
		{
			/* Continue to use new */
			curEntry.ip = curXMLString;
			[curNetwork insertObject:curEntry inChildrenAtIndex:[curNetwork.children count]];
			[curEntry autorelease];
		}
	}
	
	/* Check for end of network/entry */
	else if ([element isEqualToString:@"network_entry"])
	{ curNetwork = nil; }
	else if ([element isEqualToString:@"ip_entry"])
	{ curEntry = nil; }
	
	/* Default: Set XML Value */
	else if (curXMLString)
	{ 
		if (curEntry)
		{ [curEntry setXmlValue:curXMLString forKey:element]; }
		else if (curNetwork)
		{ [curNetwork setXmlValue:curXMLString forKey:element]; }
	}		
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Send delegate */
	if ([delegate respondsToSelector:@selector(networkListRefreshFinished:)])
	{ [delegate performSelector:@selector(networkListRefreshFinished:) withObject:self]; }
	
	/* Clean up */
	[sender release];
	refreshXMLRequest = nil;
	
	/* Set refresh flag */
	[self setScanInProgress:NO];
	[self setRefreshInProgress:NO];
}

#pragma mark "Accessors"

@synthesize customer;
- (void) setCustomer:(LCCustomer *)value
{
	[customer release];
	customer = [value retain];
	[self updateDisplayString];
}
@synthesize displayString;

- (void) updateDisplayString
{ 
	if (refreshXMLRequest && scanInProgress)
	{ self.displayString = [NSString stringWithFormat:@"%@ (Scanning...)", customer.displayString]; }
	else if (refreshXMLRequest)
	{ self.displayString = [NSString stringWithFormat:@"%@ (Updating...)", customer.displayString]; }
	else
	{ self.displayString = [NSString stringWithFormat:@"%@", customer.displayString]; }
}	
	

- (NSString *) ip
{ return nil; }

@synthesize children;
- (void) insertObject:(LCIPRegistryNetwork *)network inChildrenAtIndex:(unsigned int)index
{
	[children insertObject:network atIndex:index];
	[childDictionary setObject:network forKey:[network address]];
}
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{
	[childDictionary removeObjectForKey:[[children objectAtIndex:index] address]];
	[children removeObjectAtIndex:index];
}
@synthesize childDictionary;

@synthesize refreshInProgress;
- (void) setRefreshInProgress:(BOOL)value
{
	refreshInProgress = value;
	[self updateDisplayString];
}
@synthesize scanInProgress;
@synthesize delegate;

- (NSString *) uniqueIdentifier
{ return [[customer entityAddress] addressString]; }

@end
