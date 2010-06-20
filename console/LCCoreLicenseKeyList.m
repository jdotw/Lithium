//
//  LCCoreLicenseKeyList.m
//  Lithium Console
//
//  Created by James Wilson on 6/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCoreLicenseKeyList.h"


@implementation LCCoreLicenseKeyList

#pragma mark "Construct"

+ (LCCoreLicenseKeyList *) keyListForCustomer:(LCCustomer *)initCustomer
{
	return [[[LCCoreLicenseKeyList alloc] initForCustomer:initCustomer] autorelease];
}

- (LCCoreLicenseKeyList *) initForCustomer:(LCCustomer *)initCustomer
{
	self = [super init];
	if (!self) return nil;
	
	self.customer = initCustomer;
	keys = [[NSMutableArray array] retain];
	keyDictionary = [[NSMutableDictionary dictionary] retain];
	
	return self;
}

- (void) dealloc
{
	if (refreshXMLRequest) 
	{ 
		[refreshXMLRequest cancel]; 
		[refreshXMLRequest release]; 
	}
	[keys release];
	[keyDictionary release];
	[customer release];
	[super dealloc];
}

#pragma mark "Refresh methods"

- (void) refreshWithPriority:(int)priority
{
	/* Check state */
	if (refreshXMLRequest) return;
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:customer
												  resource:[customer resourceAddress] 
													entity:[customer entityAddress] 
												   xmlname:@"lic_key_list"
													refsec:0 
													xmlout:nil] retain];
	[refreshXMLRequest setDelegate:self];
	[refreshXMLRequest setThreadedXmlDelegate:self];
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

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	NSMutableArray *seenKeys = [NSMutableArray array];
	
	for (LCXMLNode *childNode in rootNode.children)
	{
		if ([childNode.name isEqualToString:@"license"])
		{
			LCCoreLicenseKey *key = [keyDictionary objectForKey:[childNode.properties objectForKey:@"id"]];
			if (!key)
			{
				key = [LCCoreLicenseKey new];
				key.keyID = [[childNode.properties objectForKey:@"id"] intValue];
				[self insertObject:key inKeysAtIndex:keys.count];
			}
			[key setXmlValuesUsingXmlNode:childNode];
			[seenKeys addObject:key];
		}
	}
	
	NSMutableArray *removeArray = [NSMutableArray array];
	for (LCCoreLicenseKey *key in keys)
	{ 
		if (![seenKeys containsObject:key]) [removeArray addObject:key];
	}	
	for (LCCoreLicenseKey *key in removeArray)
	{
		[self removeObjectFromKeysAtIndex:[keys indexOfObject:key]];
	}
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Free XML request */
	if (sender == refreshXMLRequest)
	{
		[refreshXMLRequest release];
		refreshXMLRequest = nil;
	}
	
	/* Set Flag */
	self.refreshInProgress = NO;
}

#pragma mark "Properties"

@synthesize customer;
@synthesize keys;
- (void) insertObject:(LCCoreLicenseKey *)key inKeysAtIndex:(unsigned int)index
{ 
	[keyDictionary setObject:key forKey:[NSString stringWithFormat:@"%i", key.keyID]];
	[keys insertObject:key atIndex:index];
}
- (void) removeObjectFromKeysAtIndex:(unsigned int)index
{
	[keyDictionary removeObjectForKey:[NSString stringWithFormat:@"%i", [[keys objectAtIndex:index] keyID]]];
	[keys removeObjectAtIndex:index];
}
@synthesize keyDictionary;
@synthesize refreshInProgress;

@end
