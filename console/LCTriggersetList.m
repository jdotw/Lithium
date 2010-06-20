//
//  LCTriggersetList.m
//  Lithium Console
//
//  Created by James Wilson on 6/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCTriggersetList.h"
#import "LCTriggerset.h"
#import "LCTriggersetTrigger.h"

@implementation LCTriggersetList

#pragma mark "Constructors"

+ (LCTriggersetList *) listWithObject:(LCEntity *)initObject
{
	return [[[LCTriggersetList alloc] initWithObject:initObject] autorelease];
}

- (LCTriggersetList *) initWithObject:(LCEntity *)initObject
{
	[self init];
	obj = [initObject retain];
	return self;
}

- (LCTriggersetList *) init
{
	[super init];
	tsets = [[NSMutableArray array] retain];
	tsetDictionary = [[NSMutableDictionary dictionary] retain];
	return self;
}

- (void) dealloc
{
	[tsets release];
	[tsetDictionary release];
	[obj release];
	if (refreshXMLRequest) { [refreshXMLRequest cancel]; [refreshXMLRequest release]; }
	if (curXMLElement) [curXMLElement release];
	if (curXMLString) [curXMLString release];	
	[super dealloc];
}	

#pragma mark "Refresh Methods"

- (void) refreshWithPriority:(int)priority
{
	if (refreshXMLRequest) return;
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:[obj customer]
												  resource:[[obj device] resourceAddress] 
													entity:[obj entityAddress] 
												   xmlname:@"triggerset_list"
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
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	curXMLElement = [element retain];
	
	/* Check for start of an triggerset/trigger */
	if ([element isEqualToString:@"triggerset"])
	{
		curTset = [[LCTriggerset triggersetWithObject:obj] retain];
	}
	else if ([element isEqualToString:@"trigger"])
	{
		curTrg = [[LCTriggersetTrigger triggerWithObject:obj triggerset:curTset] retain];
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
	/* Add element string to properties dict */
	if (curXMLElement)
	{ 
		if (curTrg)
		{ 
			if (curXMLString) [[curTrg properties] setObject:curXMLString forKey:curXMLElement]; 
			else [[curTrg properties] removeObjectForKey:curXMLElement];
		}
		else
		{ 
			if (curXMLString) [[curTset properties] setObject:curXMLString forKey:curXMLElement]; 
			else [[curTset properties] removeObjectForKey:curXMLElement];
		}
	}
	
	/* Release current element */
	if (curXMLElement)
	{
		[curXMLElement release];
		curXMLElement = nil;
	}
	
	/* Check for a name element */
	if ([element isEqualToString:@"name"])
	{
		if (curTrg)
		{
			/* A trigger name is encountered, check 
			 * to see if the trigger already exists 
			 */
			LCTriggersetTrigger *existingTrg;
			existingTrg = [[curTset triggerDictionary] objectForKey:[curTrg name]];
			if (existingTrg)
			{
				/* Trigger exists, switch to using it */
				[curTrg release];
				curTrg = [existingTrg retain];
			}
			[curTrg willChangeValueForKey:@"xValueDisplayString"];
			[curTrg willChangeValueForKey:@"yValueDisplayString"];
		}
		else
		{
			/* A triggerset name is encountered, check
			 * to see if the triggerset alread exists
			 */
			LCTriggerset *existingTset;
			existingTset = [tsetDictionary objectForKey:[curTset name]];
			if (existingTset)
			{
				/* Triggerset exists, switch to using it */
				[curTset release];
				curTset = [existingTset retain];
			}
			[curTset willChangeValueForKey:@"applied"];
		}
	}
	
	/* Check for end of a triggerset/trigger */
	if ([element isEqualToString:@"triggerset"])
	{
		/* End of triggerset */
		if (![tsets containsObject:curTset])
		{
			[self insertObject:curTset inTriggersetsAtIndex:[tsets count]]; 
		}
		[curTset didChangeValueForKey:@"applied"];
		[curTset release];
		curTset = nil;
	}
	else if ([element isEqualToString:@"trigger"])
	{
		/* End of a trigger */
		if (![[curTset triggers] containsObject:curTrg])
		{
			[curTset insertObject:curTrg inTriggersAtIndex:[[curTset triggers] count]]; 
		}
		[curTrg didChangeValueForKey:@"xValueDisplayString"];
		[curTrg didChangeValueForKey:@"yValueDisplayString"];
		[curTrg release];
		curTrg = nil;
	}
	
	/* Free current curXMLString */
	if (curXMLString)
	{
		[curXMLString release];
		curXMLString = nil;
	}
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{	
	/* Call delegate */
	if (delegate) 
	{
		SEL finishedsel = NSSelectorFromString(@"triggersetlistRefreshFinished:");
		if (finishedsel && [delegate respondsToSelector:finishedsel])
		{ [delegate performSelector:finishedsel withObject:self]; }
	}
	
	/* Free XML request */
	[sender release];
	refreshXMLRequest = nil;
	
	/* Set refresh flag */
	[self setRefreshInProgress:NO];
}

#pragma mark "Accessors"

- (NSArray *) triggersets
{ return (NSArray *) tsets; }
- (void) insertObject:(id)tset inTriggersetsAtIndex:(unsigned int)index
{
	[tsetDictionary setObject:tset forKey:[tset name]];
	[tsets insertObject:tset atIndex:index];
}
- (void) removeObjectFromTriggersetsAtIndex:(unsigned int)index
{
	if ([tsets objectAtIndex:index])
	{ [tsetDictionary removeObjectForKey:[[tsets objectAtIndex:index] name]]; }
	[tsets removeObjectAtIndex:index];
}
- (NSDictionary *) triggersetDictionary
{ return tsetDictionary; }

- (BOOL) refreshInProgress
{ return refreshInProgress; } 
- (void) setRefreshInProgress:(BOOL)flag
{ refreshInProgress = flag; }

- (void) setDelegate:(id)newDelegate
{ delegate = newDelegate; }

@synthesize obj;
@synthesize tsets;
@synthesize tsetDictionary;
@synthesize refreshXMLRequest;
@synthesize curTset;
@synthesize curTrg;
@synthesize curXMLElement;
@synthesize curXMLString;
@synthesize xmlIncidentDictionary;
@synthesize delegate;
@end
