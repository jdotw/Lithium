//
//  LCCaseList.m
//  Lithium Console
//
//  Created by James Wilson on 7/07/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCase.h"
#import "LCCaseList.h"
#import "LCIncident.h"
#import "LCCustomer.h"
#import "LCAuthenticator.h"

@implementation LCCaseList

#pragma mark "Initialisation"

- (LCCaseList *) init
{
	[super init];
	
	cases = [[NSMutableArray array] retain];
	caseDictionary = [[NSMutableDictionary dictionary] retain];
	criteria = [[NSMutableDictionary dictionary] retain];
	
	return self;
}

- (void) dealloc 
{
	[cases release];
	[caseDictionary release];
	[criteria release];
	if (refreshXMLRequest) { [refreshXMLRequest cancel]; [refreshXMLRequest release]; }
	[super dealloc];
}

#pragma mark "Refresh methods"

- (void) refreshWithPriority:(int)priority
{
	/* Check state */
	if (!customer) return;
	if (refreshXMLRequest) return;
	
	/* Check for criteria */
	NSXMLDocument *xmlDoc = nil;
	if ([criteria count] > 0)
	{
		/* Create request XML document */
		NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"criteria"];
		xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];

		[xmlDoc setVersion:@"1.0"];
		[xmlDoc setCharacterEncoding:@"UTF-8"];

		/* Add criteria elements */
		NSArray *keys = [criteria allKeys];
		for (NSString *key in keys)
		{
			if ([key isEqualToString:@"entity"])
			{
				/* Entity encoding */
				[rootNode addChild:[[[criteria objectForKey:@"entity"] entityDescriptor] xmlNode]];
			}
			else if ([key hasPrefix:@"start"] || [key hasPrefix:@"end"])
			{
				/* Start/End date encoding */
				NSDate *date = [criteria objectForKey:key];
				NSString *str = [NSString stringWithFormat:@"%.0f", [date timeIntervalSince1970]];
				[rootNode addChild:[NSXMLNode elementWithName:key stringValue:str]]; 
			}
			else
			{
				/* Generic encoding */
				[rootNode addChild:[NSXMLNode elementWithName:key
												  stringValue:[criteria objectForKey:key]]]; 
			}
		}
	}

	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:customer
												  resource:[customer resourceAddress] 
													entity:[customer entityAddress] 
												   xmlname:@"case_list" 
													refsec:0 
													xmlout:xmlDoc] retain];
	[refreshXMLRequest setDelegate:self];
	[refreshXMLRequest setXMLDelegate:self];
	[refreshXMLRequest setPriority:priority];
	[refreshXMLRequest performAsyncRequest];
	self.refreshInProgress = YES;
}

- (void) highPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_HIGH]; }

- (void) normalPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_NORMAL]; }

- (void) lowPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_LOW]; }

- (void) XMLRequestPreParse:(id)sender
{
	/* Create XML cases dictionary */
	xmlCaseDictionary = [[NSMutableDictionary dictionary] retain];
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	/* Check for start of a case */
	if ([element isEqualToString:@"case"])
	{
		curCase = [LCCase new];
		((LCCase *)curCase).customer = self.customer;
	}

	if (curXMLString) [curXMLString release];
	curXMLString = [[NSMutableString string] retain];
}

- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	[curXMLString appendString:string];
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Add element case properties */
	if (curCase && curXMLString)
	{ [curCase setXmlValue:curXMLString forKey:element]; }
		
	/* Check for end of a case */
	if ([element isEqualToString:@"id"])
	{
		[xmlCaseDictionary setObject:(LCCase *)curCase forKey:curXMLString];

		/* Check for existing case */
		if ([caseDictionary objectForKey:curXMLString])
		{
			/* Switch to existing */
			[curCase release];
			curCase = [caseDictionary objectForKey:curXMLString];
		}
		else
		{
			/* Insert New case */
			[self insertObject:curCase inCasesAtIndex:[cases count]]; 
			
			/* Refresn entity list */
			[[curCase entityList] normalPriorityRefresh];
			[[curCase logEntryList] normalPriorityRefresh];
		}
	}
	else if ([element isEqualToString:@"case"])
	{ curCase = nil; }
	
	[curXMLString release];
	curXMLString = nil;
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Check success */
	if ([sender success])
	{
		/* Remove extinct cases */
		NSMutableArray *extinctCases = [NSMutableArray array];
		for (LCCase *cas in cases)
		{
			if (![xmlCaseDictionary objectForKey:[NSString stringWithFormat:@"%i", cas.caseID]])
			{ [extinctCases addObject:cas];	}
		}
		for (LCCase *cas in extinctCases)
		{ [self removeObjectFromCasesAtIndex:[cases indexOfObject:cas]]; }
	}
	
	/* Free xml case dictionary */
	if (xmlCaseDictionary) [xmlCaseDictionary release];
	xmlCaseDictionary = nil;
	
	/* Call delegate */
	if (delegate) 
	{
		SEL finishedsel = NSSelectorFromString(@"caselistRefreshFinished:");
		if (finishedsel && [delegate respondsToSelector:finishedsel])
		{ [delegate performSelector:finishedsel withObject:self]; }
		else
		{ NSLog (@"Warning: LCCaseList delegate does not respond to caselistRefreshFinished"); }
	}
	
	/* Free XML request */
	[sender release];
	refreshXMLRequest = nil;
	
	/* Set Flag */
	self.refreshInProgress = NO;
}

#pragma mark "Criteria Accessor Methods"

- (void) removeAllCriteria
{ [criteria removeAllObjects]; }

- (NSNumber *) caseID
{ return [criteria objectForKey:@"id"]; }
- (void) setCaseID:(NSNumber *)number
{ 
	if (number) [criteria setObject:number forKey:@"id"]; 
	else [criteria removeObjectForKey:@"id"];
}

- (NSNumber *) stateInteger
{ return [criteria objectForKey:@"state"]; }
- (void) setStateInteger:(NSNumber *)number
{ 
	if (number) [criteria setObject:number forKey:@"state"]; 
	else [criteria removeObjectForKey:@"state"];
}

- (NSDate *) startUpper
{ return [criteria objectForKey:@"startupper"]; }
- (void) setStartUpper:(NSDate *)date
{ 
	if (date) [criteria setObject:date forKey:@"startupper"]; 
	else [criteria removeObjectForKey:@"startupper"];
}

- (NSDate *) startLower
{ return [criteria objectForKey:@"startlower"]; }
- (void) setStartLower:(NSDate *)date
{ 
	if (date) [criteria setObject:date forKey:@"startlower"]; 
	else [criteria removeObjectForKey:@"startlower"];
}

- (NSDate *) endUpper
{ return [criteria objectForKey:@"endupper"]; }
- (void) setEndUpper:(NSDate *)date
{ 
	if (date) [criteria setObject:date forKey:@"endupper"]; 
	else [criteria removeObjectForKey:@"endupper"];
}

- (NSDate *) endLower
{ return [criteria objectForKey:@"endlower"]; }
- (void) setEndLower:(NSDate *)date
{ 
	if (date) [criteria setObject:date forKey:@"endlower"]; 
	else [criteria removeObjectForKey:@"endlower"];
}

- (NSString *) headline
{ return [criteria objectForKey:@"hline"]; }
- (void) setHeadline:(NSString *)string
{ 
	if (string) [criteria setObject:string forKey:@"hline"]; 
	else [criteria removeObjectForKey:@"hline"];
}

- (NSString *) owner
{ return [criteria objectForKey:@"owner"]; }
- (void) setOwner:(NSString *)string
{ 
	if (string) [criteria setObject:string forKey:@"owner"]; 
	else [criteria removeObjectForKey:@"owner"];
}

- (NSString *) requester
{ return [criteria objectForKey:@"requester"]; }
- (void) setRequester:(NSString *)string
{ 
	if (string) [criteria setObject:string forKey:@"requester"]; 
	else [criteria removeObjectForKey:@"requester"];
}

- (LCEntity *) entity
{ return [criteria objectForKey:@"entity"]; }
- (void) setEntity:(LCEntity *)newEntity
{ 
	if (newEntity) [criteria setObject:newEntity forKey:@"entity"];
	else [criteria removeObjectForKey:@"entity"];
}

#pragma mark "Accessor Methods"

@synthesize customer;
@synthesize delegate;
@synthesize cases;
- (void) insertObject:(id)cas inCasesAtIndex:(unsigned int)index
{ 
	[cases insertObject:cas atIndex:index];
	[caseDictionary setObject:cas forKey:[NSString stringWithFormat:@"%i", [(LCCase *)cas caseID]]];
}
- (void) removeObjectFromCasesAtIndex:(unsigned int) index
{ 
	if ([cases objectAtIndex:index])
	{ [caseDictionary removeObjectForKey:[NSString stringWithFormat:@"%i", [(LCCase *)[cases objectAtIndex:index] caseID]]]; }
	[cases removeObjectAtIndex:index]; 
}
@synthesize caseDictionary;
@synthesize refreshInProgress;

@end
