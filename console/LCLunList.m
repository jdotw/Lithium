//
//  LCLunList.m
//  Lithium Console
//
//  Created by James Wilson on 12/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCLunList.h"


@implementation LCLunList

#pragma mark "Constructors"

+ (LCLunList *) lunListForCustomer:(id)initCustomer
{
	return [[[LCLunList alloc] initForCustomer:initCustomer] autorelease];
}

- (LCLunList *) initForCustomer:(id)initCustomer
{
	[super init];
	
	luns = [[NSMutableArray array] retain];
	lunDictionary = [[NSMutableDictionary dictionary] retain];
	wwnDictionary = [[NSMutableDictionary dictionary] retain];
	
	[self setCustomer:initCustomer];
	
	return self;
}

- (void) dealloc
{
	[self setCustomer:nil];
	[luns release];
	[lunDictionary release];
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
												   xmlname:@"lunregistry_list" 
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
	/* Set current element to nil */
	curXMLElement = nil;
	
	/* Create XML entity list */
	xmlLunDictionary = [[NSMutableDictionary dictionary] retain];
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	curXMLElement = [element retain];
	
	/* Check for start of entity_descriptor */
	if ([element isEqualToString:@"entity_descriptor"])
	{
		curXMLDictionary = [[NSMutableDictionary dictionary] retain];
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
	if (curXMLElement)
	{
		if (!curXMLString)
		{ curXMLString = [[NSMutableString stringWithString:string] retain]; }
		else
		{ [curXMLString appendString:string]; }
	}
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Check for end of an entity desc */
	if ([element isEqualToString:@"entity_descriptor"])
	{
		/* Create descriptor */
		LCEntityDescriptor *entityDescriptor = [LCEntityDescriptor descriptorWithProperties:curXMLDictionary];
		LCEntity *entity = [entityDescriptor locateEntity:YES];
		
		/* Check type */
		if (curType == 1)
		{
			/* This is a LUN Target entry */
			
			/* Set entity in xmlLunDictionary */
			if (entity && [entityDescriptor addressString])
			{ [xmlLunDictionary setObject:entity forKey:[entityDescriptor addressString]]; }
			
			/* Check for existing */
			if (![lunDictionary objectForKey:[[entity entityAddress] addressString]])
			{
				/* Insert new entity */
				[self insertObject:[LCLun lunWithObject:entity wwn:curWwn] inLunsAtIndex:[luns count]];
				[[entity device] normalPriorityRefresh];
			}
		}
		else if (curType == 2)
		{
			/* This is a LUN Use Entry */
			
			/* Check for existing LCLun */
			LCLun *lun = [wwnDictionary objectForKey:curWwn];
			if (!lun)
			{
				NSMutableString *nodeWWN = [NSMutableString stringWithString:curWwn];
				[nodeWWN replaceCharactersInRange:NSMakeRange(0,1) withString:@"6"];
				lun = [wwnDictionary objectForKey:nodeWWN];
			}
			if (lun)
			{
				/* Found "target LUN" */
				if (![[lun useDictionary] objectForKey:[entityDescriptor addressString]])
				{
					/* New LUN Use Entry */
					[[lun useDictionary] setObject:entity forKey:[entityDescriptor addressString]];
					[lun insertObject:entity inUsesAtIndex:[[lun uses] count]];
				}
			}			
		}
		
		/* Release current dictionary */
		[curXMLDictionary release];
		curXMLDictionary = nil;
	}
	
	/* Check for end of wwn */
	else if (!curXMLDictionary && [element isEqualToString:@"wwn"])
	{
		curWwn = [NSString stringWithString:curXMLString];
	}

	/* Check for end of type */
	else if (!curXMLDictionary && [element isEqualToString:@"lun_type"])
	{
		curType = [curXMLString intValue];
	}	
	
	/* Default : Add element string to dict */
	else if (curXMLElement && curXMLString)
	{ 
		[curXMLDictionary setObject:curXMLString forKey:curXMLElement];
	}	
	
	/* Release current element */
	if (curXMLElement)
	{
		[curXMLElement release];
		curXMLElement = nil;
	}	
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Check success */
	int i;
	if ([sender success])
	{
		/* Remove extinct incidents */
		NSMutableIndexSet *indexSet = [NSMutableIndexSet indexSet];
		for (i=0; i < [luns count]; i++)
		{
			LCEntity *entity = [[luns objectAtIndex:i] object];
			if (![xmlLunDictionary objectForKey:[[entity entityAddress] addressString]])
			{
				/* Service is extinct, add index to indexSet */
				[indexSet addIndex:i];
				[lunDictionary removeObjectForKey:[[entity entityAddress] addressString]];
			}
		}
		[self willChangeValueForKey:@"luns"];
		[luns removeObjectsAtIndexes:indexSet];
		[self didChangeValueForKey:@"luns"];
	}
	
	/* Release XML entity dictionary */
	[xmlLunDictionary release];
	xmlLunDictionary = nil;	
	
	/* Free XML request */
	[sender release];
	refreshXMLRequest = nil;
	
	/* Set refresh flag */
	[self setRefreshInProgress:NO];
}

#pragma mark "Accessors"

- (id) customer
{ return customer; }

- (void) setCustomer:(id)newCustomer
{
	[customer release];
	customer = [newCustomer retain];
}

- (NSMutableArray *) luns
{ return luns; }
- (void) insertObject:(LCLun *)lun inLunsAtIndex:(unsigned int)index
{
	[luns insertObject:lun atIndex:index];
	[lunDictionary setObject:lun forKey:[[[lun object] entityAddress] addressString]];
	[wwnDictionary setObject:lun forKey:[lun wwn]];
}
- (void) removeObjectFromLunsAtIndex:(unsigned int)index
{
	[wwnDictionary removeObjectForKey:[[luns objectAtIndex:index] wwn]];
	[lunDictionary removeObjectForKey:[[[[luns objectAtIndex:index] object] entityAddress] addressString]];
	[luns removeObjectAtIndex:index];
}
- (NSMutableDictionary *) lunDictionary
{ return lunDictionary; }
- (NSMutableDictionary *) wwnDictionary
{ return wwnDictionary; }

- (BOOL) refreshInProgress
{ return refreshInProgress; }
- (void) setRefreshInProgress:(BOOL)flag
{ refreshInProgress = flag; }

@synthesize luns;
@synthesize lunDictionary;
@synthesize wwnDictionary;
@synthesize refreshXMLRequest;
@synthesize curXMLDictionary;
@synthesize xmlLunDictionary;
@synthesize curXMLElement;
@synthesize curXMLString;
@synthesize curWwn;
@synthesize curType;
@end
