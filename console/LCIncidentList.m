//
//  LCIncidentList.m
//  Lithium Console
//
//  Created by James Wilson on 29/06/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCAuthenticator.h"
#import "LCIncident.h"
#import "LCGrowler.h"
#import "LCCustomerList.h"
#import "LCAction.h"

#import "LCIncidentList.h"

@implementation LCIncidentList

#pragma "Initialisation"

- (LCIncidentList *) init
{
	[super init];

	criteria = [[NSMutableDictionary dictionary] retain];
	incidents = [[NSMutableArray array] retain];
	incidentDictionary = [[NSMutableDictionary dictionary] retain];
	newIncidents = [[NSMutableArray array] retain];
	clearedIncidents = [[NSMutableArray array] retain];

	[self setInitialRefresh:YES];
	
	return self;
}

- (void) dealloc 
{
	if (refreshXMLRequest) { [refreshXMLRequest cancel]; [refreshXMLRequest release]; }
	if (curXMLString) [curXMLString release];
	
	[criteria release];
	[incidents release];
	[incidentDictionary release];
	[newIncidents release];
	[clearedIncidents release];

	[super dealloc];
}

#pragma mark "Refresh methods"

- (void) refreshWithPriority:(int)priority
{
	NSXMLDocument *xmlDoc = nil;
	
	if (!customer) return;
	if (refreshXMLRequest) return;
	
	/* Create criteria XML */
	if ([criteria count] > 0)
	{
		/* Create request XML document */
		NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"criteria"];
		xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];
		[xmlDoc setVersion:@"1.0"];
		[xmlDoc setCharacterEncoding:@"UTF-8"];

		/* Loop through criteria */
		NSArray *keyArray = [criteria allKeys];
		for (id loopItem in keyArray)
		{
			if ([loopItem isEqualToString:@"entity"])
			{
				/* Entity encoding */
				[rootNode addChild:[[[criteria objectForKey:@"entity"] entityDescriptor] xmlNode]];
			}
			if ([loopItem hasPrefix:@"start"] || [loopItem hasPrefix:@"end"])
			{
				/* Date */
				[rootNode addChild:[NSXMLNode elementWithName:loopItem
												  stringValue:[NSString stringWithFormat:@"%li", [[criteria objectForKey:loopItem] longValue]]]];		
			}
			else
			{
				/* Generic encoding */
				[rootNode addChild:[NSXMLNode elementWithName:loopItem
												  stringValue:[[criteria objectForKey:loopItem] description]]]; 
			}
		}
	}
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:customer
												  resource:[customer resourceAddress] 
													entity:[customer entityAddress] 
												   xmlname:@"incident_list"
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
	/* Create XML incidents dictionary */
	xmlIncidentDictionary = [[NSMutableDictionary dictionary] retain];
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	/* Check for start of an incident */
	if ([element isEqualToString:@"incident"])
	{
		LCIncident *incident = [LCIncident new];
		curIncident = [[incident retain] autorelease];
		[curIncident setIsLive:isLiveList];
	}

	/* Check for start of an action */
	if ([element isEqualToString:@"action"])
	{
		LCAction *action = [LCAction new];
		action.hostEntity = self.customer;
		action.incident = curIncident;
		curAction = [[action retain] autorelease];
		processingAction = YES;
	}	
	
	/* Check for start of an entity descriptor */
	if ([element isEqualToString:@"entity_descriptor"])
	{
		curEntityDescDict = [[NSMutableDictionary dictionary] retain];
		processingEntityDescriptor = YES;
	}
	
	if (curXMLString)
	{ 
		[curXMLString release]; 
		curXMLString = nil;
	}	
}

- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	/* Free previous curXMLString */
	if (!curXMLString)
	{ curXMLString = [[NSMutableString string] retain]; }
	[curXMLString appendString:string];
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Check for an Incident ID */
	if ([element isEqualToString:@"id"] && !processingAction)
	{
		/* Set ID */
		curIncident.incidentID = (unsigned long) [curXMLString longLongValue];
		[xmlIncidentDictionary setObject:curIncident forKey:[NSNumber numberWithInt:curIncident.incidentID]];
		
		/* Check for existing */
		LCIncident *existingIncident = [incidentDictionary objectForKey:[NSNumber numberWithInt:curIncident.incidentID]];
		if (existingIncident)
		{
			/* Use existing */
			[curIncident release];
			curIncident = [existingIncident retain];
			curIncidentIsNew = NO;
			
			/* Reset action flags */
			[curIncident setHasPendingActions:NO];
		}
		else
		{
			/* Insert new incident */
			if (isLiveList) [self insertObject:curIncident inIncidentsAtIndex:0];
			else [self insertObject:curIncident inIncidentsAtIndex:[incidents count]];
			curIncidentIsNew = YES;
		}
	}
	
	/* Check for an Action ID */
	else if ([element isEqualToString:@"id"] && processingAction)
	{
		/* Set ID */
		((LCAction *)curAction).taskID = (unsigned long) [curXMLString longLongValue];
		
		/* Check for existing */
		LCAction *existingAction = [[curIncident actionDictionary] objectForKey:[NSNumber numberWithUnsignedLong:((LCAction *)curAction).taskID]];
		if (existingAction)
		{
			/* Switch to using existingAction */
			[curAction release];
			curAction = [existingAction retain];
		}
		else
		{
			/* Insert new action */
			[curIncident insertObject:curAction inActionsAtIndex:[[curIncident actions] count]];
		}
			
	}
				
	/* Check for end of a incident */
	else if ([element isEqualToString:@"incident"])
	{
		if (isLiveList)
		{
			/* Refresh device to show current value in Incident Manager */
			[[curIncident.entity object] highPriorityRefresh];

			if (curIncidentIsNew)
			{
				/* Add to device */
				[[[curIncident entity] device] insertObject:curIncident inIncidentsAtIndex:[[[[curIncident entity] device] incidents] count]];

				/* Update local entity */
				if (![self initialRefresh])
				{
					[[curIncident entityDescriptor] updateLocalFromDescriptor];
				}

				/* Add to newIncident list */
				if ([self initialRefresh] == NO)
				{
					/* Check for new unhandled */
					if (curIncident.caseID == 0)
					{
						[newIncidents addObject:curIncident];
					}
				}
			}
		}
		
		[curIncident release];
		curIncident = nil;
	}

	/* Check for the end of an Action */
	else if ([element isEqualToString:@"action"])
	{
		/* Free curAction */
		[curAction release];
		curAction = nil;
		processingAction = NO;
	}
	
	/* Check for the end of an Entity Descriptor */
	else if ([element isEqualToString:@"entity_descriptor"])
	{
		/* Set Incidents Entity Descriptor */
		if (!processingAction)
		{ curIncident.entityDescriptor = [LCEntityDescriptor descriptorWithProperties:curEntityDescDict]; }

		/* Free curEntityDescDict */
		[curEntityDescDict release];
		curEntityDescDict = nil;
		processingEntityDescriptor = NO;
	}
	
	/* Default Handling */
	else if (element && curXMLString)
	{ 
		if (processingAction)
		{
			[curAction setXmlValue:curXMLString forKey:element];
		}
		else if (processingEntityDescriptor)
		{
			[curEntityDescDict setObject:curXMLString forKey:element];
		}
		else 
		{ 
			[curIncident setXmlValue:curXMLString forKey:element]; 
		}
	}

	/* Free current curXMLString */
	[curXMLString release];
	curXMLString = nil;
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{	
	/* Check success */
	if ([sender success])
	{
		/* Remove extinct incidents */
		LCIncident *inc;
		for (inc in incidents)
		{
			if (![xmlIncidentDictionary objectForKey:[NSNumber numberWithUnsignedLong:inc.incidentID]])
			{ 
				/* Incident is extinct, add to cleared list */
				[clearedIncidents addObject:inc];
			}
		}
	}
	
	/* Process newIncidents */
	if (isLiveList)
	{
		if ([newIncidents count] > 0)
		{
			if ([newIncidents count] > 5)
			{
				/* Mass update growl */
				if ([[NSUserDefaults standardUserDefaults] boolForKey:@"growlNotifications"])
				{ [[LCGrowler masterGrowler] reportMultipleIncidents:newIncidents]; }
			}
			else
			{
				/* Individual alert growl */
				LCIncident *inc;
				for (inc in newIncidents)
				{
					if ([[NSUserDefaults standardUserDefaults] boolForKey:@"growlNotifications"])
					{ [[LCGrowler masterGrowler] reportIncident:inc]; }
				}
			}

			/* Bound dock icon once */
			[NSApp requestUserAttention: NSInformationalRequest];
					
			/* Ding */
			if ([[NSUserDefaults standardUserDefaults] boolForKey:@"dingOnIncident"])
			{ [[NSSound soundNamed:@"Glass"] play]; }
		}
	
		/* Process clearedIncidents */
		if ([clearedIncidents count] > 0)
		{
			if ([clearedIncidents count] > 5)
			{
				/* Mass update growl */
				[[LCGrowler masterGrowler] reportMultipleIncidentsCleared:clearedIncidents];
			}
			else
			{
				/* Individual alert growl */
				LCIncident *inc;
				for (inc in clearedIncidents)
				{
					[[LCGrowler masterGrowler] reportIncidentCleared:inc];
				}
			}
			for (LCIncident *inc in clearedIncidents)
			{
				[self removeObjectFromIncidentsAtIndex:[incidents indexOfObject:inc]];
			}
		}
	}
	
	/* Clean up */
	[newIncidents removeAllObjects];
	[clearedIncidents removeAllObjects];
		
	/* Release XML incidents dictionary */
	if (xmlIncidentDictionary) [xmlIncidentDictionary release];
	xmlIncidentDictionary = nil;
	
	/* Call delegate */
	if (delegate) 
	{
		SEL finishedsel = NSSelectorFromString(@"inclistRefreshFinished:");
		if (finishedsel && [delegate respondsToSelector:finishedsel])
		{ [delegate performSelector:finishedsel withObject:self]; }
	}
	
	/* Free XML request */
	[sender release];
	refreshXMLRequest = nil;
	
	/* Set refresh flag */
	[self setInitialRefresh:NO];
	
	/* Set refresh flag */
	[self setRefreshInProgress:NO];
	
	/* Update Badge */
	NSArray *allIncidents = [[LCCustomerList masterArray] valueForKeyPath:@"@distinctUnionOfArrays.activeIncidentsList.incidents"];
	if (allIncidents.count > 0)
	{
		[[NSApp dockTile] setBadgeLabel:[NSString stringWithFormat:@"%i", allIncidents.count]];
	}
	else
	{
		[[NSApp dockTile] setBadgeLabel:nil];
	}	
}

#pragma mark "Entity Searching"

- (NSMutableArray *) incidentsForEntity:(LCEntity *)entity
{
	/* Returns a list of incidents that are descendents
	 * of the specified entity
	 */
	NSMutableArray *array = [NSMutableArray array];
	NSEnumerator *incidentEnum = [[self incidents] objectEnumerator];
	LCIncident *inc;
	while ((inc=[incidentEnum nextObject])!=nil)
	{
		if ([(LCEntity *)[inc entity] isDescendantOf:entity])
		{ [array addObject:inc]; }
	}
	
	return array;	
}

#pragma mark "Criteria Accessor Methods"

- (NSNumber *) incidentID
{ return [criteria objectForKey:@"id"]; }
- (void) setIncidentID:(NSNumber *)number
{ [criteria setObject:number forKey:@"id"]; }

- (NSNumber *) stateInteger
{ return [criteria objectForKey:@"state"]; }
- (void) setStateInteger:(NSNumber *)number
{ [criteria setObject:number forKey:@"state"]; }

- (NSNumber *) startUpperSeconds
{ return [criteria objectForKey:@"startupper"]; }
- (void) setStartUpperSeconds:(NSNumber *)number
{ [criteria setObject:number forKey:@"startupper"]; }

- (NSNumber *) startLowerSeconds
{ return [criteria objectForKey:@"startlower"]; }
- (void) setStartLowerSeconds:(NSNumber *)number
{ [criteria setObject:number forKey:@"startlower"]; }

- (NSNumber *) endUpperSeconds
{ return [criteria objectForKey:@"endupper"]; }
- (void) setEndUpperSeconds:(NSNumber *)number
{ [criteria setObject:number forKey:@"endupper"]; }

- (NSNumber *) endLowerSeconds
{ return [criteria objectForKey:@"endlower"]; }
- (void) setEndLowerSeconds:(NSNumber *)number
{ [criteria setObject:number forKey:@"endlower"]; }

- (LCEntity *) entity
{ return [criteria objectForKey:@"entity"]; }
- (void) setEntity:(LCEntity *)newEntity
{ 
	if (newEntity) [criteria setObject:newEntity forKey:@"entity"]; 
	else [criteria removeObjectForKey:@"entity"];
}

- (NSNumber *) adminStateInteger
{ return [criteria objectForKey:@"adminstate_num"]; }
- (void) setAdminStateInteger:(NSNumber *)number
{ [criteria setObject:number forKey:@"adminstate_num"]; }

- (NSNumber *) opStateInteger
{ return [criteria objectForKey:@"opstate_num"]; }
- (void) setOpStateInteger:(NSNumber *)number
{ [criteria setObject:number forKey:@"opstate_num"]; }

- (NSNumber *) caseID
{ return [criteria objectForKey:@"caseid"]; }
- (void) setCaseID:(NSNumber *)number
{ [criteria setObject:number forKey:@"caseid"]; }

- (NSNumber *) maxCount
{ return [criteria objectForKey:@"max_count"]; }
- (void) setMaxCount:(NSNumber *)number
{ [criteria setObject:number forKey:@"max_count"]; }

#pragma mark "Relevance Scoring"

- (void) scoreRelevanceToEntity:(LCEntity *)localEntity
{
	/* Calculate Related Relevance Scores */
	for (LCIncident *relatedIncident in incidents)
	{
		/* Calculate relevance score */
		int i;
		BOOL contiguous = YES;
		for (i=0; i < [[[relatedIncident entity] typeInteger] intValue]; i++)
		{
			LCEntity *compareEntity = nil;
			LCEntity *relatedEntity = nil;
			
			switch (i)
			{
				case 0:
					relatedEntity = [[relatedIncident entity] customer];
					compareEntity = [localEntity customer];
					break;
				case 1:
					relatedEntity = [[relatedIncident entity] site];
					compareEntity = [localEntity site];
					break;
				case 2:
					relatedEntity = [[relatedIncident entity] device];
					compareEntity = [localEntity device];
					break;
				case 3:
					relatedEntity = [[relatedIncident entity] container];
					compareEntity = [localEntity container];
					break;
				case 4:
					relatedEntity = [[relatedIncident entity] object];
					compareEntity = [localEntity object];
					break;
				case 5:
					relatedEntity = [[relatedIncident entity] metric];
					compareEntity = [localEntity metric];
					break;
				case 6:
					relatedEntity = [[relatedIncident entity] trigger];
					compareEntity = [localEntity trigger];
					break;
			}
			
			if ([[relatedEntity name] isEqualToString:[compareEntity name]])
			{
				/* Match */
				int existingScore = relatedIncident.relevanceScore;
				int score;
				if (contiguous == YES)
				{ score = 2; }
				else
				{ score = 1; }
				score += existingScore;
				
				relatedIncident.relevanceScore = score;
			}
			else
			{
				/* No match */
				contiguous = NO;
			}
		}
	}
}

#pragma mark "Property Methods"

@synthesize customer;
@synthesize delegate;
@synthesize isLiveList;
@synthesize criteria;
@synthesize incidents;

- (void) insertObject:(LCIncident *)inc inIncidentsAtIndex:(unsigned int)index
{ 
	[incidentDictionary setObject:inc forKey:[NSNumber numberWithInt:inc.incidentID]];
	[incidents insertObject:inc atIndex:index];
}
- (void) removeObjectFromIncidentsAtIndex:(unsigned int) index
{ 
	if ([incidents objectAtIndex:index])
	{ 
		/* Remove */
		LCIncident *inc = [incidents objectAtIndex:index];
		[incidentDictionary removeObjectForKey:[NSNumber numberWithInt:inc.incidentID]]; 
		if ([[[[inc entity] device] incidents] indexOfObject:inc] != NSNotFound)
		{ [[[inc entity] device] removeObjectFromIncidentsAtIndex:[[[[inc entity] device] incidents] indexOfObject:inc]]; }
	}
	[incidents removeObjectAtIndex:index];
}

@synthesize incidentDictionary;
@synthesize initialRefresh;
@synthesize refreshInProgress;

@end
