//
//  LCBonjourBrowserList.m
//  Lithium Console
//
//  Created by James Wilson on 30/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBonjourBrowserList.h"
#import "LCCustomer.h"

@implementation LCBonjourBrowserList

#pragma mark "Constructors"

+ (LCBonjourBrowserList *) bonjourBrowserListForCustomer:(id)initCustomer
{
	return [[[LCBonjourBrowserList alloc] initForCustomer:initCustomer] autorelease];
}

- (id) initForCustomer:(id)initCustomer
{
	[super init];
	
	children = [[NSMutableArray array] retain];
	childDictionary = [[NSMutableDictionary dictionary] retain];
	
	[self setCustomer:initCustomer];
	
	return self;
}

- (void) dealloc
{
	if (refreshXMLRequest)
	{ [refreshXMLRequest cancel]; [refreshXMLRequest release]; }
	[self setCustomer:nil];
	[children release];
	[childDictionary release];
	[super dealloc];
}

#pragma mark "Refresh methods"

- (void) refreshWithPriority:(int)priority
{
	/* Check state */
	if (!customer) return;
	if (refreshXMLRequest) return;
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:customer
												  resource:[customer resourceAddress] 
													entity:[customer entityAddress] 
												   xmlname:@"bonjour_list" 
													refsec:0 
													xmlout:nil] retain];
	[refreshXMLRequest setDelegate:self];
	[refreshXMLRequest setXMLDelegate:self];
	[refreshXMLRequest setPriority:priority];
	[refreshXMLRequest performAsyncRequest];
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
	/* Check for start of browser */
	if ([element isEqualToString:@"browser"])
	{
		curBrowser = [LCBonjourBrowser new];
		curBrowser.parent = self;
	}	

	/* Check for start of service */
	else if ([element isEqualToString:@"service"])
	{
		curService = [LCBonjourService new];
		curService.parent = curBrowser;
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
	/* Check for end of a browser service name */
	if ([element isEqualToString:@"servicename"] && curBrowser)
	{
		/* Find browser */
		if ([childDictionary objectForKey:curXMLString])
		{
			/* Switch to existing */
			[curBrowser release];
			curBrowser = [childDictionary objectForKey:curXMLString];
		}
		else
		{
			/* Use New Browser */
			curBrowser.service = curXMLString;
			[self insertObject:curBrowser inChildrenAtIndex:[children count]];
			[curBrowser autorelease];
		}		
	}

	/* Check for end of a service name */
	else if ([element isEqualToString:@"name"] && curBrowser && curService)
	{
		/* Find service in browser */
		if ([curBrowser.childDictionary objectForKey:curXMLString])
		{
			/* Switch to existing */
			[curService release];
			curService = [curBrowser.childDictionary objectForKey:curXMLString];
		}
		else
		{
			/* Use New Service */
			curService.name = curXMLString;
			[curBrowser insertObject:curService inChildrenAtIndex:[curBrowser.children count]];
			[curService autorelease];
		}		
	}
	
	
	/* Check for end of a browser */
	else if ([element isEqualToString:@"browser"])
	{ curBrowser = nil;	}

	/* Check for end of a browser */
	else if ([element isEqualToString:@"service"])
	{ curService = nil; }
	
	/* Default to using setXmlValue */
	else if (curXMLString)
	{ 
		if (curService)
		{ [curService setXmlValue:curXMLString forKey:element]; }
		else if (curBrowser)
		{ [curBrowser setXmlValue:curXMLString forKey:element]; }
	}	
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{	
	/* Free XML request */
	[sender release];
	refreshXMLRequest = nil;
	
	/* Inform delegate */
	if ([delegate respondsToSelector:@selector(bonjourListRefreshFinished:)])
	{ [delegate performSelector:@selector(bonjourListRefreshFinished:) withObject:self]; }
	
	/* Set refresh flag */
	[self setRefreshInProgress:NO];
}

#pragma mark "Browser List Properties"

@synthesize displayString;
- (void) updateDisplayString
{
	if (refreshInProgress)
	{ self.displayString = [NSString stringWithFormat:@"%@ (Updating...)", [(LCCustomer *)customer displayString]]; }
	else
	{ self.displayString = [(LCCustomer *)customer displayString]; }
}

- (NSString *) ip
{ return nil; }

#pragma mark "Properties"
@synthesize delegate;
@synthesize customer;
- (void) setCustomer:(id)value
{
	[customer release];
	customer = [value retain];
	[self updateDisplayString];
}

@synthesize children;
- (void) insertObject:(LCBonjourBrowser *)browser inChildrenAtIndex:(unsigned int)index
{
	[children insertObject:browser atIndex:index];
	[childDictionary setObject:browser forKey:[browser service]];
}
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{
	[childDictionary removeObjectForKey:[[children objectAtIndex:index] service]];
	[children removeObjectAtIndex:index];
}
@synthesize childDictionary;
@synthesize refreshInProgress;
- (void) setRefreshInProgress:(BOOL)value
{
	refreshInProgress = value;
	[self updateDisplayString];
}

- (NSString *) uniqueIdentifier
{ return [[customer entityAddress] addressString]; }



@end
