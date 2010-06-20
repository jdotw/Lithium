//
//  LCXsanList.m
//  Lithium Console
//
//  Created by James Wilson on 11/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXsanList.h"


@implementation LCXsanList

#pragma mark "Constructors"

+ (LCXsanList *) xsanListForCustomer:(id)initCustomer
{
	return [[[LCXsanList alloc] initForCustomer:initCustomer] autorelease];
}

- (LCXsanList *) initForCustomer:(id)initCustomer
{
	[super init];
	
	clients = [[NSMutableArray array] retain];
	clientDictionary = [[NSMutableDictionary dictionary] retain];
	controllers = [[NSMutableArray array] retain];
	controllerDictionary = [[NSMutableDictionary dictionary] retain];
	volumes = [[NSMutableArray array] retain];
	volumeDictionary = [[NSMutableDictionary dictionary] retain];
	
	[self setCustomer:initCustomer];
	
	return self;
}

- (void) dealloc
{
	[self setCustomer:nil];
	[clients release];
	[clientDictionary release];
	[controllers release];
	[controllerDictionary release];
	[volumes release];
	[volumeDictionary release];
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
												   xmlname:@"xsanregistry_list" 
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
	xmlClientDictionary = [[NSMutableDictionary dictionary] retain];
	xmlControllerDictionary = [[NSMutableDictionary dictionary] retain];
	xmlVolumeDictionary = [[NSMutableDictionary dictionary] retain];
	
	/* Set inVolume flag */
	inVolume = NO;
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	curXMLElement = [element retain];
	
	/* Check for the start of a computer */
	if ([element isEqualToString:@"volume"])
	{
		inVolume = YES;
	}		

	/* Check for start of entity_descriptor */
	else if ([element isEqualToString:@"entity_descriptor"])
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
		
		/* Check if we're dealing with a computer or volume entity */
		if (!inVolume)
		{
			if (computerType == 1)
			{
				/* CONTROLLER */
				if (entity && [entityDescriptor addressString])
				{ [xmlControllerDictionary setObject:entity forKey:[entityDescriptor addressString]]; }
			
				/* Check for existing */
				if (![controllerDictionary objectForKey:[entityDescriptor addressString]])
				{
					/* Insert new entity */
					[self insertObject:entity inControllersAtIndex:[controllers count]];
					[[entity device] normalPriorityRefresh];
				}	
			}
			else if (computerType == 2)
			{
				/* CLIENT */
				if (entity && [entityDescriptor desc])
				{ [xmlClientDictionary setObject:entity forKey:[entityDescriptor addressString]]; }
				
				/* Check for existing */
				if (![clientDictionary objectForKey:[entityDescriptor addressString]])
				{
					/* Insert new entity */
					[self insertObject:entity inClientsAtIndex:[clients count]];
					[[entity device] normalPriorityRefresh];
				}	
			}
		}
		else
		{
			/* VOLUME */
			if (entity && [entityDescriptor addressString])
			{ [xmlVolumeDictionary setObject:entity forKey:[entityDescriptor addressString]]; }
			
			/* Check for existing */
			if (![volumeDictionary objectForKey:[entity desc]])
			{
				/* Insert new entity */
				[self insertObject:[LCXsanVolume volumeWithObject:entity] inVolumesAtIndex:[volumes count]];
				[[entity device] normalPriorityRefresh];
			}
			else
			{
				/* Add entity to volumes object list */
				LCXsanVolume *vol = [volumeDictionary objectForKey:[entity desc]];
				if (![[vol objectDictionary] objectForKey:[entityDescriptor addressString]])
				{ [vol insertObject:entity inObjectsAtIndex:[[vol objects] count]]; }
			}
		}
		
		/* Release current dictionary */
		[curXMLDictionary release];
		curXMLDictionary = nil;
	}
	
	/* Check for end of volume */
	else if ([element isEqualToString:@"volume"])
	{ inVolume = NO; }
	
	/* Check for end of type */
	if (!curXMLDictionary && [element isEqualToString:@"type"])
	{
		computerType = [curXMLString intValue];
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
		/* Remove extinct controllers */
		LCEntity *entity;
		for (entity in controllers)
		{
			if (![xmlControllerDictionary objectForKey:[[entity entityAddress] addressString]])
			{
				/* Controller is extinct */
//				[self removeObjectFromControllersAtIndex:[controllers indexOfObject:entity]];
				/* DEBUG This has been removed due to an unresolved crash */
			}
		}
		
		/* Remove extinct clients */
		NSMutableIndexSet *indexSet = [NSMutableIndexSet indexSet];
		for (i=0; i < [clients count]; i++)
		{
			LCEntity *entity = [clients objectAtIndex:i];
			if (![xmlClientDictionary objectForKey:[[entity entityAddress] addressString]])
			{
				/* Controller is extinct, add index to indexSet */
				[indexSet addIndex:i];
//				[clientDictionary removeObjectForKey:[[entity entityAddress] addressString]];
				/* DEBUG This has been removed due to an unresolved crash */
			}
		}
//		[self willChangeValueForKey:@"clients"];
//		[clients removeObjectsAtIndexes:indexSet];
//		[self didChangeValueForKey:@"clients"];		

		/* Remove extinct volumes */
		LCXsanVolume *vol;
		for (vol in volumes)
		{
			indexSet = [NSMutableIndexSet indexSet];
			for (i=0; i < [[vol objects] count]; i++)
			{
				LCEntity *entity = [[vol objects] objectAtIndex:i];
				if (![xmlVolumeDictionary objectForKey:[[entity entityAddress] addressString]])
				{
					/* Controller is extinct, add index to indexSet */
					[indexSet addIndex:i];
//					[[vol objectDictionary] removeObjectForKey:[[entity entityAddress] addressString]];
				}
			}

			/* FIX Disabled due to crash */
//			[vol willChangeValueForKey:@"objects"];
//			[[vol objects] removeObjectsAtIndexes:indexSet];
//			[vol didChangeValueForKey:@"objects"];
			/* Check it's worth keeping the vol */
//			if ([[vol objects] count] < 1)
//			{
//				/* No volume objects */
//				[self removeObjectFromVolumesAtIndex:[volumes indexOfObject:vol]];
//			}
		}
	}
	
	/* Release XML entity dictionary */
	[xmlClientDictionary release];
	xmlClientDictionary = nil;	
	[xmlControllerDictionary release];
	xmlControllerDictionary = nil;	
	[xmlVolumeDictionary release];
	xmlVolumeDictionary = nil;	
	
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

- (NSMutableArray *) controllers
{ return controllers; }
- (void) insertObject:(LCEntity *)entity inControllersAtIndex:(unsigned int)index
{
	[controllers insertObject:entity atIndex:index];
	[controllerDictionary setObject:entity forKey:[[entity entityAddress] addressString]];
}
- (void) removeObjectFromControllersAtIndex:(unsigned int)index
{
	[controllerDictionary removeObjectForKey:[[[controllers objectAtIndex:index] entityAddress] addressString]];
	[controllers removeObjectAtIndex:index];
}

- (NSMutableArray *) clients
{ return clients; }
- (void) insertObject:(LCEntity *)entity inClientsAtIndex:(unsigned int)index
{
	[clients insertObject:entity atIndex:index];
	[clientDictionary setObject:entity forKey:[[entity entityAddress] addressString]];
}
- (void) removeObjectFromClientsAtIndex:(unsigned int)index
{
	[clientDictionary removeObjectForKey:[[[clients objectAtIndex:index] entityAddress] addressString]];
	[clients removeObjectAtIndex:index];
}

- (NSMutableArray *) volumes
{ return volumes; }
- (void) insertObject:(LCXsanVolume *)vol inVolumesAtIndex:(unsigned int)index
{
	[volumes insertObject:vol atIndex:index];
	[volumeDictionary setObject:vol forKey:[vol displayString]];
}
- (void) removeObjectFromVolumesAtIndex:(unsigned int)index
{
	[volumeDictionary removeObjectForKey:[[volumes objectAtIndex:index] displayString]];
	[volumes removeObjectAtIndex:index];
}

- (BOOL) refreshInProgress
{ return refreshInProgress; }
- (void) setRefreshInProgress:(BOOL)flag
{ refreshInProgress = flag; }

@synthesize clients;
@synthesize clientDictionary;
@synthesize controllers;
@synthesize controllerDictionary;
@synthesize volumes;
@synthesize volumeDictionary;
@synthesize refreshXMLRequest;
@synthesize curXMLDictionary;
@synthesize xmlControllerDictionary;
@synthesize xmlClientDictionary;
@synthesize xmlVolumeDictionary;
@synthesize curXMLElement;
@synthesize curXMLString;
@synthesize inVolume;
@synthesize computerType;
@end
