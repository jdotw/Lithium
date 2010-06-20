//
//  LCTriggersetValRuleList.m
//  Lithium Console
//
//  Created by James Wilson on 6/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCTriggersetValRuleList.h"
#import "LCTriggerset.h"

@implementation LCTriggersetValRuleList

#pragma mark "Constructors"

+ (LCTriggersetValRuleList *) listWithObject:(LCEntity *)initObject triggerset:(id)initTriggerset trigger:(id)initTrigger
{
	return [[[LCTriggersetValRuleList alloc] initWithObject:initObject triggerset:initTriggerset trigger:initTrigger] autorelease];
}

- (LCTriggersetValRuleList *) initWithObject:(LCEntity *)initObject triggerset:(id)initTriggerset trigger:(id)initTrigger
{
	[super init];
	obj = [initObject retain];
	tset = [initTriggerset retain];
	trg = [initTrigger retain];
	rules = [[NSMutableArray array] retain];
	ruleDictionary = [[NSMutableDictionary dictionary] retain];
	return self;
	
}

- (void) dealloc
{
	[obj release];
	[tset release];
	[trg release];
	[rules release];
	[ruleDictionary release];
	[super dealloc];
}

#pragma mark "Refresh Methods"

- (void) refreshWithPriority:(int)priority
{
	if (refreshXMLRequest) return;
	
	/* Create request XML document */
	NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"criteria"];
	NSXMLDocument *xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];
	[xmlDoc setVersion:@"1.0"];
	[xmlDoc setCharacterEncoding:@"UTF-8"];
	[rootNode addChild:[NSXMLNode elementWithName:@"tset_name"
									  stringValue:[(LCTriggerset *)tset name]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"met_name"
									  stringValue:[(LCTriggerset *)tset metricName]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"trg_name"
									  stringValue:[trg name]]]; 
	
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:[obj customer]
												  resource:[[obj device] resourceAddress] 
													entity:[obj entityAddress] 
												   xmlname:@"triggerset_valrule_list"
													refsec:0 
													xmlout:xmlDoc] retain];
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
	xmlRuleDict = [[NSMutableDictionary dictionary] retain];
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	curXMLElement = [element retain];
	
	/* Check for start of an rule */
	if ([element isEqualToString:@"valrule"])
	{
		curRule = [[LCTriggersetValRule ruleWithObject:obj triggerset:tset trigger:trg] retain];
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
	if (curXMLElement && curXMLString)
	{ 
		[[curRule properties] setObject:curXMLString forKey:curXMLElement];
	}
	
	/* Release current element */
	if (curXMLElement)
	{
		[curXMLElement release];
		curXMLElement = nil;
	}
	
	/* Check for a id element */
	if ([element isEqualToString:@"id"])
	{
		/* A rule id is encountered, check
		* to see if the rule already exists
		*/
		LCTriggersetValRule *existingRule;
		existingRule = [ruleDictionary objectForKey:[curRule ID]];
		if (existingRule)
		{
			[curRule release];
			curRule = [existingRule retain];
		}
	}
	
	/* Check for end of a rule */
	if ([element isEqualToString:@"valrule"])
	{
		/* End of rule */
		[xmlRuleDict setObject:curRule forKey:[curRule ID]];
		if (![rules containsObject:curRule])
		{
			[self insertObject:curRule inRulesAtIndex:[rules count]]; 
		}
		[curRule release];
		curRule = nil;
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
	/* Check obsolescence */
	LCTriggersetValRule *rule;
	for (rule in rules)
	{
		if (![xmlRuleDict objectForKey:[rule ID]])
		{
			[self removeRule:rule];
		}
	}
	[xmlRuleDict release];
	xmlRuleDict = nil;
	
	/* Call delegate */
	if (delegate) 
	{
		SEL finishedsel = NSSelectorFromString(@"valRuleListRefreshFinished:");
		if (finishedsel && [delegate respondsToSelector:finishedsel])
		{ [delegate performSelector:finishedsel withObject:self]; }
	}
	
	/* Free XML request */
	[sender release];
	refreshXMLRequest = nil;
	
	/* Set refresh flag */
	[self setRefreshInProgress:NO];
	[self setInitialRefreshCompleted:YES];
}

#pragma mark "Accessors"

- (NSArray *) rules
{ return rules; }
- (void) insertObject:(id)rule inRulesAtIndex:(unsigned int)index
{
	[ruleDictionary setObject:rule forKey:[rule ID]];
	[rules insertObject:rule atIndex:index];
}
- (void) removeObjectFromRulesAtIndex:(unsigned int)index
{
	if ([rules objectAtIndex:index])
	{ [ruleDictionary removeObjectForKey:[[rules objectAtIndex:index] ID]]; }
	[rules removeObjectAtIndex:index];
}
- (void) removeRule:(LCTriggersetValRule *)rule
{
	if ([rules containsObject:rule])
	{ [self removeObjectFromRulesAtIndex:[rules indexOfObject:rule]]; }
}
- (NSMutableDictionary *) ruleDictionary
{ return ruleDictionary; }

- (BOOL) refreshInProgress
{ return refreshInProgress; } 
- (void) setRefreshInProgress:(BOOL)flag
{ refreshInProgress = flag; }

- (BOOL) initialRefreshCompleted
{ return initialRefreshCompleted; }
- (void) setInitialRefreshCompleted:(BOOL)flag
{ initialRefreshCompleted = flag; }

- (void) setDelegate:(id)newDelegate
{ delegate = newDelegate; }


@synthesize obj;
@synthesize tset;
@synthesize trg;
@synthesize ruleDictionary;
@synthesize refreshXMLRequest;
@synthesize curRule;
@synthesize curXMLElement;
@synthesize curXMLString;
@synthesize xmlRuleDict;
@synthesize delegate;
@end
