//
//  LCCaseEntityList.m
//  Lithium Console
//
//  Created by James Wilson on 25/10/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCaseEntityList.h"
#import "LCCase.h"

@implementation LCCaseEntityList

#pragma mark "Initialisation"

- (LCCaseEntityList *) init
{
	[super init];
	
	/* Create entry array/dict */
	entities = [[NSMutableArray array] retain];
	entityDictionary = [[NSMutableDictionary dictionary] retain];
	
	return self;
}

- (void) dealloc 
{
	[entities release];
	[entityDictionary release];
	if (refreshXMLRequest) 
	{ 
		[refreshXMLRequest cancel]; 
		[refreshXMLRequest release]; 
	}
	[super dealloc];
}

#pragma mark "Refresh methods"

- (void) refreshWithPriority:(int)priority
{
	/* Refresh the list of log entries */
	if (!cas || self.refreshInProgress) return;
	
	/* Create request XML document */
	NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"criteria"];
	NSXMLDocument *xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];
	[xmlDoc setVersion:@"1.0"];
	[xmlDoc setCharacterEncoding:@"UTF-8"];
	[rootNode addChild:[NSXMLNode elementWithName:@"caseid"
									  stringValue:[NSString stringWithFormat:@"%i", [(LCCase *)cas caseID]]]];
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:[(LCCase *)cas customer]
												  resource:[[(LCCase *)cas customer] resourceAddress] 
													entity:[[(LCCase *)cas customer] entityAddress] 
												   xmlname:@"case_entity_list" 
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
	/* Create XML entity list */
	xmlParseInProgress = YES;
	xmlEntityDictionary = [[NSMutableDictionary dictionary] retain];
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	/* Check for start of an entity descriptor */
	if ([element isEqualToString:@"entity_descriptor"])
	{
		if (curXMLDictionary) [curXMLDictionary release];
		curXMLDictionary = [[NSMutableDictionary dictionary] retain];
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
	/* Add element string to dict */
	if (curXMLString && curXMLDictionary)
	{ 
		[curXMLDictionary setObject:curXMLString forKey:element]; 
	}
	
	/* Check for end of an entity descriptor */
	if ([element isEqualToString:@"entity_descriptor"])
	{
		/* Create descriptor */
		LCEntityDescriptor *entityDescriptor = [LCEntityDescriptor descriptorWithProperties:curXMLDictionary];
		LCEntity *entity = [entityDescriptor locateEntity:YES];
		if (entity && [entityDescriptor addressString])
		{ [xmlEntityDictionary setObject:entity forKey:[entityDescriptor addressString]]; }

		/* Check for existing */
		if (![entityDictionary objectForKey:[entityDescriptor addressString]])
		{
			/* Insert new entity */
			[self willChangeValueForKey:@"entities"];
			[entities addObject:entity];
			[entityDictionary setObject:entity forKey:[entityDescriptor addressString]];
			[self didChangeValueForKey:@"entities"];
		}
		
		/* Refresh */
		[entity normalPriorityRefresh];
		
		/* Release current dictionary */
		[curXMLDictionary release];
		curXMLDictionary = nil;
	}
	
	[curXMLString release];
	curXMLString = nil;
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Check for non-refresh requests */
	if (sender != refreshXMLRequest)
	{ [sender release]; return; }

	/* Check success */
	if ([sender success])
	{
		/* Remove extinct */
		NSMutableArray *extinctEntities = [NSMutableArray array];
		for (LCEntity *entity in entities)
		{
			if (![xmlEntityDictionary objectForKey:[[entity entityAddress] addressString]])
			{
				/* Instinct is extinct */
				[extinctEntities addObject:entity];
			}
		}
		for (LCEntity *entity in extinctEntities)
		{
			[self removeObjectFromEntitiesAtIndex:[entities indexOfObject:entity]];
		}

		/* Set auto-refrsh */
		if ([entities count] > 0)
		{
			/* Enable auto-refresh */
			[cas setEntityListAutoRefresh:YES];
		}
		else
		{
			/* Disable auto-refresh */
			[cas setEntityListAutoRefresh:NO];
		}
	}
	
	/* Release XML entity dictionary */
	if (xmlEntityDictionary) [xmlEntityDictionary release];
	xmlEntityDictionary = nil;	
	xmlParseInProgress = NO;
	[self updateHighestEntityOpState];

	/* Call delegate */
	if (delegate) 
	{
		SEL finishedsel = NSSelectorFromString(@"caseEntityListRefreshFinished:");
		if (finishedsel && [delegate respondsToSelector:finishedsel])
		{ [delegate performSelector:finishedsel withObject:self]; }
		else
		{ NSLog (@"Warning: LCCaseEntityList delegate does not respond to caseEntityListRefreshFinished"); }
	}

	/* Free XML request */
	[sender release];
	refreshXMLRequest = nil;
	
	/* Set refresh flag */
	[self setRefreshInProgress:NO];
}

#pragma mark "Accessor Methods"

- (void) setDelegate:(id)setdel { delegate = setdel; }

- (NSMutableArray *) entities { return entities; }

- (void) insertObject:(LCEntity *)entity inEntitiesAtIndex:(unsigned int)index
{ 
	/* Add to array and dictionary */
	[entities insertObject:entity atIndex:index];
	if ([[entity entityAddress] addressString]) 
	{ [entityDictionary setObject:entity forKey:[[entity entityAddress] addressString]]; }
	
	/* Send entity info */
	if (!xmlParseInProgress && self.cas)
	{
		/* Create insert request XML document */
		NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"criteria"];
		NSXMLDocument *xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];
		[xmlDoc setVersion:@"1.0"];
		[xmlDoc setCharacterEncoding:@"UTF-8"];
		[rootNode addChild:[NSXMLNode elementWithName:@"caseid"
										  stringValue:[NSString stringWithFormat:@"%i", [(LCCase *)cas caseID]]]];
		[rootNode addChild:[[entity entityDescriptor] xmlNode]];
		
		/* Create XML Request */
		LCXMLRequest *insertXMLRequest = [[LCXMLRequest requestWithCriteria:[cas customer]
													 resource:[[cas customer] resourceAddress] 
													   entity:[[cas customer] entityAddress] 
													  xmlname:@"case_entity_insert" 
													   refsec:0 
													   xmlout:xmlDoc] retain];
		[insertXMLRequest setDelegate:self];
		
		/* Perform XML request */
		[insertXMLRequest performAsyncRequest];		
		[self updateHighestEntityOpState];
	}
}

- (void) removeObjectFromEntitiesAtIndex:(unsigned int)index
{
	/* Remove object from array and send XML delete request */
	
	/* Create insert request XML document */
	if (!xmlParseInProgress && self.cas)
	{
		NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"criteria"];
		NSXMLDocument *xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];
		[xmlDoc setVersion:@"1.0"];
		[xmlDoc setCharacterEncoding:@"UTF-8"];
		[rootNode addChild:[NSXMLNode elementWithName:@"caseid"
										  stringValue:[NSString stringWithFormat:@"%@", [(LCCase *)cas caseID]]]];
		[rootNode addChild:[[[entities objectAtIndex:index] entityDescriptor] xmlNode]];
		
		/* Create XML Request */
		LCXMLRequest *deleteXMLRequest = [[LCXMLRequest requestWithCriteria:[cas customer]
													 resource:[[cas customer] resourceAddress] 
													   entity:[[cas customer] entityAddress] 
													  xmlname:@"case_entity_delete" 
													   refsec:0 
													   xmlout:xmlDoc] retain];
		[deleteXMLRequest setDelegate:self];
		
		/* Perform XML request */
		[deleteXMLRequest performAsyncRequest];
		[self updateHighestEntityOpState];
	}

	/* Remove from array */
	if ([[[entities objectAtIndex:index] entityAddress] addressString]) 
	{ [entityDictionary removeObjectForKey:[[[entities objectAtIndex:index] entityAddress] addressString]]; }
	[entities removeObjectAtIndex:index];
}

- (void) updateHighestEntityOpState
{
	/* Returns the highest op state of all
	 * entities bound to this case
	 */
	int highestState = -1;
	LCEntity *entity;
	for (entity in entities)
	{
		if ([[entity opstateInteger] intValue] > highestState)
		{ highestState = [[entity opstateInteger] intValue]; }
	}	
	self.highestEntityOpStateInteger = highestState;
}

@synthesize cas;
@synthesize delegate;
@synthesize entities;
@synthesize entityDictionary;
@synthesize refreshInProgress;
@synthesize highestEntityOpStateInteger;

@end
