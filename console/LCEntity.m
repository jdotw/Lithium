//
//  LCEntity.m
//  Lithium Console
//
//  Created by James Wilson on 14/06/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCEntity.h"
#import "LCEntityDescriptor.h"
#import "LCXMLRequest.h"
#import "LCEntityViewController.h"
#import "LCAssistController.h"
#import "LCConsoleController.h"
#import "LCDevice.h"
#import "LCMetric.h"

@implementation LCEntity

#pragma mark "Initialisation"

+ (LCEntity *) entityWithType:(int)initType name:(NSString *)initName desc:(NSString *)initDesc resourceAddressString:(NSString *)initResAddr
{
	return [[[[self class] alloc] initWithType:initType
										  name:initName
										  desc:initDesc
						 resourceAddressString:initResAddr] autorelease];
}

- (LCEntity *) initWithType:(int)initType name:(NSString *)initName desc:(NSString *)initDesc resourceAddressString:(NSString *)initResAddr
{
	[self init];
	
	self.type = initType;
	self.name = initName;
	self.desc = initDesc;
	self.resourceAddressString = initResAddr;
	
	return self;
}

- (id) init
{
	[super init];
	
	/* Create variables */
	children = [[NSMutableArray array] retain];
	childrenDictionary = [[NSMutableDictionary dictionary] retain];
	incidents = [[NSMutableArray array] retain];
	
	/* Setup XML Translation */
	if (!self.xmlTranslation) { self.xmlTranslation = [NSMutableDictionary dictionary]; }
	[self.xmlTranslation setObject:@"name" forKey:@"name"];
	[self.xmlTranslation setObject:@"desc" forKey:@"desc"];
	[self.xmlTranslation setObject:@"type" forKey:@"type_num"];
	[self.xmlTranslation setObject:@"opState" forKey:@"opstate_num"];
	[self.xmlTranslation setObject:@"adminState" forKey:@"adminstate_num"];
	[self.xmlTranslation setObject:@"listPriority" forKey:@"prio"];
	[self.xmlTranslation setObject:@"entityIndex" forKey:@"index"];
	[self.xmlTranslation setObject:@"version" forKey:@"version"];
	[self.xmlTranslation setObject:@"syncVersion" forKey:@"sync_version"];
	[self.xmlTranslation setObject:@"resourceAddressString" forKey:@"resaddr"];
	[self.xmlTranslation setObject:@"isLicensed" forKey:@"licensed"];
	[self.xmlTranslation setObject:@"isConfigured" forKey:@"configured"];
	[self.xmlTranslation setObject:@"resource_started" forKey:@"resourceStarted"];

	/* Set initial state */
	self.refreshSuccessful = YES;
	self.opState = -1;
	self.isConfigured = YES;
	
	return self;
}

- (void) dealloc 
{
	if (refreshXMLRequest) 
	{ [refreshXMLRequest cancel]; [refreshXMLRequest release]; }
	[children release];
	[childrenDictionary release];
	[incidents release];
	[resourceAddress release];
	[resourceAddressString release];
	[name release];
	[desc release];
	[displayString release];
	[longDisplayString release];
	[displayValue release];
	[refreshVersionTag release];
	[childRefreshVersionTag release];
	[localRefreshVersionTag release];
	[super dealloc];
}

#pragma DEBUG Binding Intervention

- (void)bind:(NSString *)binding 
	toObject:(id)observable
 withKeyPath:(NSString *)keyPath 
	 options:(NSDictionary *)options
{
	if ([[keyPath componentsSeparatedByString:@"properties"] count] > 1)
	{ 
		NSLog (@"BAD BAD BINDING: Tried to bind to %@ on %@:%@", keyPath, [self class], [self name]); 
		return;
	}
	[super bind:binding toObject:observable withKeyPath:keyPath options:options];
}
	
#pragma mark "Refresh Methods"

- (void) refreshWithPriority:(int)priority
{
	/* Check for refresh in progress */
	if (self.isOrphan) return;
	if (refreshXMLRequest) return;

	/* Create outbound XML */
	NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"tree_request"];
	NSXMLDocument *xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];
	[xmlDoc setVersion:@"1.0"];
	[xmlDoc setCharacterEncoding:@"UTF-8"];
	[rootNode addChild:[NSXMLNode elementWithName:@"sync_version" stringValue:[NSString stringWithFormat:@"%lu", self.syncVersion]]];	
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:[self customer]
												  resource:[self resourceAddress]
													entity:[self entityAddress] 
												   xmlname:@"entity_tree_authorative" 
													refsec:0 
													xmlout:xmlDoc] retain];
	[(LCXMLRequest *) refreshXMLRequest setThreadedXmlDelegate:self];
	[(LCXMLRequest *) refreshXMLRequest setDelegate:self];	
	[(LCXMLRequest *) refreshXMLRequest setPriority:priority];
 	[(LCXMLRequest *) refreshXMLRequest performAsyncRequest];

	[self setRefreshInProgress:YES];
}
- (void) highPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_HIGH]; }
- (void) normalPriorityRefresh;
{ [self refreshWithPriority:XMLREQ_PRIO_NORMAL]; }
- (void) lowPriorityRefresh;
{ [self refreshWithPriority:XMLREQ_PRIO_LOW]; }

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	for (LCXMLNode *childNode in rootNode.children)
	{ 
		[self updateEntityUsingXMLNode:childNode]; 
	}
}

- (void) updateEntityUsingXMLNode:(LCXMLNode *)node
{
	/* Update properties */
	self.presenceConfirmed = YES;
	if ([node.name isEqualToString:@"entity"])
	{
		NSArray *keys = [node.properties allKeys];
		
		for (NSString *key in keys)
		{ [self setXmlValue:[node.properties objectForKey:key] forKey:key];	}
	}
	
	/* Recursively update */
	self.localRefreshVersionTag = [NSDate date];
	LCEntity *lastEntity = nil;
	for (LCXMLNode *childNode in node.children)
	{
		if ([childNode.name isEqualToString:@"entity"])
		{ 
			/* Locate/Create Child */
			LCEntity *childEntity = [childrenDictionary objectForKey:[childNode.properties objectForKey:@"name"]];
			if (!childEntity)
			{
				childEntity = [[[self childClass] new] autorelease];
				childEntity.parent = self;
			}
			
			/* Perform update */
			[childEntity updateEntityUsingXMLNode:childNode];
			
			/* Set versions */
			self.childRefreshVersionTag = self.localRefreshVersionTag;
			childEntity.refreshVersionTag = self.localRefreshVersionTag;
			
			/* Add new child if necessary */
			if (![self.childrenDictionary objectForKey:childEntity.name])
			{ 
				if (lastEntity && [children containsObject:lastEntity])
				{ [self insertObject:childEntity inChildrenAtIndex:[children indexOfObject:lastEntity]+1]; }
				else if (!lastEntity)
				{ [self insertObject:childEntity inChildrenAtIndex:0]; }
				else
				{ [self insertObject:childEntity inChildrenAtIndex:children.count]; }
			}
			
			/* Set lastEntity variable */
			[lastEntity release];
			lastEntity = [childEntity retain];
		}
	}
	[lastEntity release];
	lastEntity = nil;
	
	/* Check for obsolete entities */
	NSMutableArray *removeArray = [NSMutableArray array];
	if ([self.childRefreshVersionTag isEqualToDate:self.localRefreshVersionTag])
	{
		/* Our children have been refreshed */
		for (LCEntity *child in self.children)
		{
			if (!child.refreshVersionTag || [self.localRefreshVersionTag compare:child.refreshVersionTag] == NSOrderedDescending)
			{ 
				/* Object is obsolete. I.e it was not found in the current refresh tree */
				[removeArray addObject:child];
			}			
		}
	}
	else if (self.type != 3 && self.children.count > 0)
	{
		/* We are NOT a device, but no entities below us
		 * were refreshed. All child entities must be obsolete
		 */
		for (LCEntity *child in self.children)
		{
			[removeArray addObject:child];
		}
	}
	for (LCEntity *child in removeArray)
	{
		[self removeChild:child];
	}
	
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	/* Check to make sure we're not mid-restart */
	if (restartInProgress) return;
	
	/* See if curEntity will respond to delegate */
	if ([curEntity respondsToSelector:@selector(entityXmlDidStartElement:)])
	{
		[curEntity performSelectorOnMainThread:@selector(entityXmlDidStartElement:) withObject:element waitUntilDone:YES];
	}
		
	/* Local processing */
	if ([element isEqualToString:@"entity"])
	{
		/* This is the start of a new entity 
		 * The first time round, this will be our local
		 * entity (self). Subsequent entities are children.
		 */

		/* Check for existence of current entity */
		if (curEntity) 
		{
			/* This entity is a child of curEntity, create a 
			 * new entity which may or not be just a place 
			 * holder. Set the child's parent to curEntity
			 * and then set curEntity to the child
			 */
			
			/* Set the parents refresh tag */
			[curEntity setChildRefreshVersionTag:self.localRefreshVersionTag];

			/* Create child */
			LCEntity *child = [[curEntity childClass] new];
			[child setParent:curEntity];
			curEntity = [[child retain] autorelease];
			
			/* Set child's refresh tag */
			[curEntity setRefreshVersionTag:self.localRefreshVersionTag];
			
			/* Set parent's childrenPopulated flag */
			[[child parent] setChildrenPopulated:YES];
		}
		else
		{
			/* This entity is us (self) */
			self.localRefreshVersionTag = [NSDate date];
			curEntity = [self retain];
			curEntity.refreshVersionTag = [NSDate date];
		}
	}
	
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
	{ curXMLString = [[NSMutableString string] retain]; }
	[curXMLString appendString:string];
	
	/* Check to make sure we're not mid-restart */
	if (restartInProgress)
	{
		[curXMLString release];
		curXMLString = nil;
	}
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Check to make sure we're not mid-restart */
	if (restartInProgress) return;

	/* See if curEntity will respond to delegate */
	if ([curEntity respondsToSelector:@selector(entityXmlDidEndElement:)])
	{
		NSMutableDictionary *elementInfo = [NSMutableDictionary dictionary];
		[elementInfo setObject:element forKey:@"key"];
		if (curXMLString)
		{ [elementInfo setObject:curXMLString forKey:@"value"]; }
		[curEntity performSelectorOnMainThread:@selector(entityXmlDidEndElement:) withObject:elementInfo waitUntilDone:YES];
	}	
	
	/* Process version element */
	if ([element isEqualToString:@"version"])
	{
		if (curXMLString) receivedVersion = [curXMLString longLongValue]; 
		else receivedVersion = 0;
	}	
	
	/* Process name */
	else if ([element isEqualToString:@"name"])
	{
		/* Set Name */
		curEntity.name = curXMLString; 
		
		/* Check to see if we're a child entity */
		if (curEntity != self)
		{
			/* Add the child entity if it's new 
			 * 
			 * This must be done on the main thread and as a 
			 * single-shot check-then-add operation to ensure
			 * cross-thread synchronization
			 */
			
			[[curEntity parent] performSelectorOnMainThread:@selector(addChildIfNew:) withObject:curEntity waitUntilDone:YES];
		 
			/* Get the live entity */
			LCEntity *liveEntity = [[curEntity parent] childNamed:curEntity.name];
			
			/* Update live entry */
			liveEntity.presenceConfirmed = YES;
			liveEntity.refreshVersionTag = curEntity.refreshVersionTag;
			
			/* Replace the temporary curEntity with the live entity */
			[curEntity release];
			curEntity = [liveEntity retain];
		}
	}
	
	/* Process end of an entity */
	else if ([element isEqualToString:@"entity"])
	{
		/* End of an entity, check the validity
		 * all children and then record that the
		 * entity has had a refresh, and then set 
		 * the curEntity to the parent of curEntity
		 */
		
		/* Set version info */
		curEntity.version = receivedVersion;
		
		/* Check children are valid */
		NSMutableArray *removeArray = [NSMutableArray array];
		if ([curEntity.childRefreshVersionTag isEqualToDate:self.localRefreshVersionTag])
		{
			/* The curEntities children has been refreshed */
			for (LCEntity *child in curEntity.children)
			{
				if ([child.refreshVersionTag compare:curEntity.childRefreshVersionTag] == NSOrderedAscending)
				{ 
					/* Object is obsolete. I.e it was not found in the current refresh tree */
					[removeArray addObject:child];
				}
					
			}
		}
		else if ([[curEntity typeInteger] intValue] != 3 && 
				 [curEntity.childRefreshVersionTag compare:self.localRefreshVersionTag] == NSOrderedAscending)
		{
			/* curEntity is NOT a device, but no entities below it 
			 * were refreshed. All child entities must be obsolete
			 */
			for (LCEntity *child in curEntity.children)
			{
				[removeArray addObject:child];
			}
		}
		for (LCEntity *child in removeArray)
		{
			[curEntity performSelectorOnMainThread:@selector(removeChild:)
										withObject:child
									 waitUntilDone:YES];
		}
		
		/* Set curEntity to parent */
		[curEntity release];
		curEntity = [curEntity parent];				
	}
	
	/* Default processing of property */
	else if (curXMLString && receivedVersion > curEntity.version)
	{ 
		/* 
		 * Other element, use xmlTranslation to set value
		 */		

		if ([[[curEntity parent] childrenDictionary] objectForKey:[curEntity name]])
		{
			/* Update existing entity on main thread */
			if (!valueDictionary) valueDictionary = [[NSMutableDictionary dictionary] retain];
			[valueDictionary setObject:curXMLString forKey:@"value"];
			[valueDictionary setObject:element forKey:@"key"];
			[curEntity performSelectorOnMainThread:@selector(setXmlValueUsingProperties:) 
										withObject:valueDictionary 
									 waitUntilDone:YES];
		}
		else
		{ 
			/* Safe to update new (not yet added) on xml thread */
			[curEntity setXmlValue:curXMLString forKey:element]; 
		}
	}
	
	/* Release current string */
	[curXMLString release];
	curXMLString = nil;
}

- (void) XMLRequestFinished:(id)sender
{
	if (sender == refreshXMLRequest)
	{
		/* Set Flags */
		if (!self.initialRefreshPerformed) self.initialRefreshPerformed = YES;
		[self setRefreshInProgress:NO];
		if ([sender success])
		{
			self.refreshSuccessful = YES;		
			self.childrenPopulated = YES;
			self.lastXmlRefresh = [NSDate date];
			
			/* Check for customer */
			if (self.type == 1)
			{
//				/* Check for no devices */
//				NSArray *devices = [self valueForKeyPath:@"children.@unionOfArrays.children"];
//				if ([devices count] < 1 && !assistShown &&
//					![[NSUserDefaults standardUserDefaults] boolForKey:[NSString stringWithFormat:@"LCAssist_%@_doNotShowAgain", [self name]]])
//				{ 
//					[[LCAssistController alloc] initForCustomer:self];
//					assistShown = YES;
//					[[LCConsoleController masterController] setShowingAssistant:YES];
//				}
				
				/* Update Browser Tree */
				[[NSNotificationCenter defaultCenter] postNotificationName:@"BrowserTreeItemAdded" object:self];
			}
			
			/* Post Notification */
			[[NSNotificationCenter defaultCenter] postNotificationName:@"EntityRefreshFinished" object:self];
		}
		else
		{
			/* The refresh of an entity failed 
			 * 
			 * Reset all statuses to -1 
			 */
			
			self.refreshSuccessful = NO;
			[self recursivelySetOpStateTo:-1];
		}
		
		/* Cleanup */
		refreshXMLRequest = nil;
	}
	else if (sender == restartXMLRequest)
	{
		restartInProgress = NO;
		restartXMLRequest = nil;
		[self highPriorityRefresh];
	}
	
	/* Release request */
	[sender release];
}

#pragma mark "Process Restart"

- (void) restartMonitoringProcess
{
	/* CHeck we're a dev/cust */
	if (self.type != 3 && self.type != 1)
	{
		if (self.type > 3) [[self device] restartMonitoringProcess];
		else [[self customer] restartMonitoringProcess]; 
		return;
	}
	
	/* Create XML Request */
	LCXMLRequest *restartReq = [[LCXMLRequest requestWithCriteria:[self customer]
												  resource:[self resourceAddress] 
													entity:[self entityAddress] 
												   xmlname:@"restart" 
													refsec:0 
													xmlout:nil] retain];
	[(LCXMLRequest *) restartReq setPriority:XMLREQ_PRIO_HIGH];
	[(LCXMLRequest *) restartReq setDelegate:self];
	[(LCXMLRequest *) restartReq performAsyncRequest];
	restartXMLRequest = restartReq;
	
	/* Remove all children */
	while ([children count] > 0)
	{ [self removeObjectFromChildrenAtIndex:0]; }
	self.version = 0;
	self.syncVersion = 0;
	
	/* Remove non-essential properties */
	self.initialRefreshPerformed = NO;
	self.resourceStarted = NO;
	self.restartInProgress = YES;
	
	/* Reset our state */
	self.opState = -1;
}

#pragma mark "Auto-refresh Methods"

- (BOOL) autoRefresh
{ return autoRefresh; }

- (void) setAutoRefresh:(BOOL)flag
{
	/* Manipulate timer */
	if (flag == YES && autoRefresh == NO)
	{
		/* Enable */
		autoRefreshTimer = [NSTimer scheduledTimerWithTimeInterval:300.0
															target:self 
														  selector:@selector(lowPriorityRefresh) 
														  userInfo:nil 
														   repeats:YES];
	}
	else if (flag == NO && autoRefresh == YES)
	{
		/* Disable */
		if (autoRefreshTimer) [autoRefreshTimer invalidate];
		autoRefreshTimer = nil;
	}
	
	/* Update flag */
	autoRefresh = flag;
}

#pragma mark "Browser-related Methods"

- (BOOL) isBrowserLeaf
{
	switch ([[self typeInteger] intValue])
	{
		case 4: return NO;
		case 5: return NO;
	}
	
	return NO;
}

#pragma mark "Encoding"

- (Class)classForCoder
{ 
	/* Use the LCEntityDescriptor class for encoding */
	return [LCEntityDescriptor class];
}

- (id)replacementObjectForCoder:(NSCoder *)aCoder
{
	return [LCEntityDescriptor descriptorForEntity:self];
}

#pragma mark "Status Color"

- (NSColor *) opStateColor
{
	return [self opStateColorWithAlpha:0.8];
}

- (NSColor *) opStateColorWithAlpha:(float)alpha
{
	if (self.adminState == 1)
	{ return [NSColor colorWithCalibratedRed:0.6 green:0.6 blue:0.6 alpha:alpha]; }

	switch (self.opState)
	{
		case 0:
			return [NSColor colorWithCalibratedRed:0.0 green:0.9 blue:0.0 alpha:alpha];
		case 1:
			return [NSColor colorWithCalibratedRed:(float)255/255 green:(float)192/255 blue:(float)0/255 alpha:alpha];
		case 2:
			return [NSColor colorWithCalibratedRed:(float)255/255 green:(float)192/255 blue:(float)0/255 alpha:alpha];
		case 3:
			return [NSColor colorWithCalibratedRed:(float)241/255 green:(float)32/255 blue:(float)32/255 alpha:alpha];
	}
	
	return [NSColor colorWithCalibratedRed:0.6 green:0.6 blue:0.6 alpha:alpha];	
}

#pragma mark "Hierarchy Methods"

- (id) entityInHierarchyOfType:(int)specificedType
{
	LCEntity *entity = self;
	while (entity)
	{ 
		if ([entity type] == specificedType) break;
		entity = [entity parent];
	}
	
	return entity;
}
- (id) customer
{ return [self entityInHierarchyOfType:1]; }
- (id) site
{ return [self entityInHierarchyOfType:2]; }
- (id) device
{ return [self entityInHierarchyOfType:3]; }
- (id) container
{ return [self entityInHierarchyOfType:4]; }
- (id) object
{ return [self entityInHierarchyOfType:5]; }
- (id) metric
{ return [self entityInHierarchyOfType:6]; }
- (id) trigger
{ return [self entityInHierarchyOfType:7]; }

- (BOOL) isDescendantOf:(LCEntity *)foreignEntity
{
	/* Return true if we are a descendant of the specified entity */
	if ([[foreignEntity typeInteger] intValue] >= [[self typeInteger] intValue]) return NO;
	
	int i;
	for (i=0; i < foreignEntity.type; i++)
	{
		switch (i)
		{
			case 0: if ([self customer] != [foreignEntity customer]) return false;
				break;
			case 1: if ([self site] != [foreignEntity site]) return false;
				break;
			case 2: if ([self device] != [foreignEntity device]) return false;
				break;
			case 3: if ([self container] != [foreignEntity container]) return false;
				break;
			case 4: if ([self object] != [foreignEntity object]) return false;
				break;
			case 5: if ([self metric] != [foreignEntity metric]) return false;
				break;
			case 6: if ([self trigger] != [foreignEntity trigger]) return false;
				break;
		}
	}
	
	/* Still here, all matches */
	return YES;
}

#pragma mark "Web URL"

- (NSString *) webURLString
{
	/* Returns a urlString that represents where to find
	 * the same entity via LITHIUM.Web
	 */
	NSString *seperator;
	if ([[[self customer] url] hasSuffix:@"/"]) seperator = @"";
	else seperator = @"/";
	NSMutableString *urlString = [NSMutableString stringWithFormat:@"%@%@index.php?action=form_get&resaddr=%@&entaddr=%@&formname=main&refsec=0", 
						   [[self customer] url], seperator, [[self resourceAddress] addressString], [[self entityAddress] addressString]];
	[urlString replaceOccurrencesOfString:@"//" withString:@"/" options:0 range:NSMakeRange(0, [urlString length])];
	return urlString;
}

#pragma mark "Parent/Child Methods"

- (NSMutableArray *) children			
{ 
	if (([[self typeInteger] intValue] == 1 || [[self typeInteger] intValue] == 3) && [self childrenPopulated] == NO && ![self refreshInProgress])
	{
		[self highPriorityRefresh]; 
	}
	return children; 
}
- (void) insertObject:(LCEntity *)entity inChildrenAtIndex:(unsigned int)index
{
	[children insertObject:entity atIndex:index]; 
	[childrenDictionary setObject:entity forKey:[entity name]];
	[[self customer] insertObject:entity inGlobalEntityArrayAtIndex:[[[self customer] globalEntityArray] count]];
}
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{ 
	LCEntity *child = [children objectAtIndex:index];
	child.isOrphan = YES;
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LCEntityRemoved" object:child];
	[childrenDictionary removeObjectForKey:[[children objectAtIndex:index] name]];
	[[self customer] removeObjectFromGlobalEntityArrayAtIndex:[[[self customer] globalEntityArray] indexOfObject:child]];
	[children removeObjectAtIndex:index];
}

- (void) addChild:(LCEntity *)child
{
	[self insertObject:child inChildrenAtIndex:children.count];
}

- (void) removeChild:(LCEntity *)child
{
	int index = [children indexOfObject:child];
	if (index != NSNotFound)
	{
		[self removeObjectFromChildrenAtIndex:index];
	}
}

- (LCEntity *) childNamed:(NSString *)childName
{
	return [childrenDictionary objectForKey:childName]; 
}

- (Class) childClass
{ return [LCEntity class]; }

- (void) addChildIfNew:(LCEntity *)newEntity
{
	/* Adds the child entity only if it doesn't already exist. */
	if (![childrenDictionary objectForKey:newEntity.name])
	{ 
		[self addChild:newEntity]; 
	}
}

#pragma mark "Incident Methods"

- (void) insertObject:(id)object inIncidentsAtIndex:(unsigned int)index
{
	[incidents insertObject:object atIndex:index];
}

- (void) removeObjectFromIncidentsAtIndex:(unsigned int)index
{
	[incidents removeObjectAtIndex:index];
}

#pragma mark "Connect-using Methods"

- (void) connectUsingSSH
{
    NSDictionary* errorDict;
    NSAppleEventDescriptor* returnDescriptor = NULL;
	
	NSString *script = [NSString stringWithFormat:@"\
	tell application \"Terminal\"\n\
	do script \"ssh %@\"\n\
	end tell", [(LCDevice *)[self device] ipAddress]];
    NSAppleScript* scriptObject = [[NSAppleScript alloc] initWithSource:script];
	
    returnDescriptor = [scriptObject executeAndReturnError: &errorDict];
    [scriptObject release];
	
    if (returnDescriptor != NULL)
    {
        // successful execution
        if (kAENullEvent != [returnDescriptor descriptorType])
        {
            // script returned an AppleScript result
            if (cAEList == [returnDescriptor descriptorType])
            {
				// result is a list of other descriptors
            }
            else
            {
                // coerce the result to the appropriate ObjC type
            }
        }
    }
    else
    {
        // no script result, handle error here
    }	
	
}

- (void) connectUsingTelnet
{
    NSDictionary* errorDict;
    NSAppleEventDescriptor* returnDescriptor = NULL;
	
	NSString *script = [NSString stringWithFormat:@"\
	tell application \"Terminal\"\n\
	do script \"telnet %@\"\n\
	end tell", [[self device] ipAddress]];
    NSAppleScript* scriptObject = [[NSAppleScript alloc] initWithSource:script];
	
    returnDescriptor = [scriptObject executeAndReturnError: &errorDict];
    [scriptObject release];
	
    if (returnDescriptor != NULL)
    {
        // successful execution
        if (kAENullEvent != [returnDescriptor descriptorType])
        {
            // script returned an AppleScript result
            if (cAEList == [returnDescriptor descriptorType])
            {
				// result is a list of other descriptors
            }
            else
            {
                // coerce the result to the appropriate ObjC type
            }
        }
    }
    else
    {
        // no script result, handle error here
    }	
	
	
}

- (void) connectUsingWeb
{
	NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"http://%@", [[self device] ipAddress]]];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (void) connectUsingARD
{
    NSDictionary* errorDict;
    NSAppleEventDescriptor* returnDescriptor = NULL;
	
	NSString *script = [NSString stringWithFormat:@"\
	tell application \"Remote Desktop\"\n\
	control computer \"%@\"\n\
	end tell", [[self device] name]];
    NSAppleScript* scriptObject = [[NSAppleScript alloc] initWithSource:script];
	
    returnDescriptor = [scriptObject executeAndReturnError: &errorDict];
    [scriptObject release];
	
    if (returnDescriptor != NULL)
    {
        // successful execution
        if (kAENullEvent != [returnDescriptor descriptorType])
        {
            // script returned an AppleScript result
            if (cAEList == [returnDescriptor descriptorType])
            {
				// result is a list of other descriptors
            }
            else
            {
                // coerce the result to the appropriate ObjC type
            }
        }
    }
    else
    {
        // no script result, handle error here
    }	
}

#pragma mark "Graphable Metrics"

- (NSMutableArray *) graphableMetrics
{
	NSMutableArray *metrics = [NSMutableArray array];
	for (LCEntity *child in self.children)
	{
		if (child.type == 6)
		{
			/* Metric */
			LCMetric *metric = (LCMetric *) child;
			if (metric.isGraphable)
			{ [metrics addObject:metric]; }
		}
		else if (child.type == 7)
		{
			/* Trgger */
			LCMetric *metric = (LCMetric *) child.parent;
			if (metric.isGraphable)
			{ [metrics addObject:metric]; }
		}			
		else if (child.type < 6)
		{ [metrics addObjectsFromArray:[child graphableMetrics]]; }
	}
	
	return metrics;
}

#pragma mark "Sanctioned Properties"

@synthesize parent;
@synthesize childrenDictionary;
@synthesize isOrphan;
- (void) setIsOrphan:(BOOL)value
{
	isOrphan = value;
	if (refreshXmlRequest)
	{
		[refreshXmlRequest cancel];
		[refreshXmlRequest release];
		refreshXmlRequest = nil;
		self.refreshInProgress = NO;
	}
	for (LCEntity *entity in children)
	{ entity.isOrphan = YES; }
}
@synthesize type, name, desc, resourceAddressString;
@synthesize opState, adminState;
@synthesize listPriority;
@synthesize entityIndex;
@synthesize version;
@synthesize syncVersion;
@synthesize resourceStarted;
@synthesize isLicensed;
@synthesize isConfigured;
@synthesize presenceConfirmed;
@synthesize childrenPopulated;
@synthesize refreshInProgress;
@synthesize restartInProgress;
@synthesize initialRefreshPerformed;
@synthesize refreshSuccessful;
@synthesize refreshVersionTag;
@synthesize childRefreshVersionTag;
@synthesize localRefreshVersionTag;
@synthesize lastXmlRefresh;
@synthesize incidents;
@synthesize longDisplayString;
@synthesize uuidString;

- (void) setName:(NSString *)string
{
	if (![string isEqualToString:name])
	{
		[name release];
		name = [string copy];
		
		if (!self.desc) self.displayString = self.name;
	}
}

- (void) setDesc:(NSString *)string
{
	if (![string isEqualToString:desc])
	{
		[desc release];
		desc = [string copy];
		
		if (![displayString isEqualToString:desc])
		{ self.displayString = self.desc; }
	}
}

@synthesize displayString;
- (void) setDisplayString:(NSString *)value
{
	[displayString release];
	displayString = [value copy];
	self.sortString = displayString; 
}

- (void) recursivelySetOpStateTo:(int)value
{
	self.opState = value;
	self.version = 0;
	self.syncVersion = 0;
	for (LCEntity *child in children)
	{ [child recursivelySetOpStateTo:value]; }
}

#pragma mark "Unsure Properties"

@synthesize properties;
@synthesize entityViewNIBLoaded;
@synthesize entityView;
@synthesize entityViewObjectController;
@synthesize versionWarningShown;
@synthesize assistShown;

#pragma mark "Accessors"

- (id) currentValue
{ return nil; }

- (LCResourceAddress *) resourceAddress
{ 
	if ([self resourceAddressString])
	{ return [LCResourceAddress addressWithString:[self resourceAddressString]]; }
	else if (self.type >= 3)
	{ 
		if ([self.device resourceAddressString])
		{ return [LCResourceAddress addressWithString:[self.device resourceAddressString]]; }
		else
		{ return [LCResourceAddress addressWithString:[self.customer resourceAddressString]]; }			
	}
	else if (self.type < 3)
	{ return [LCResourceAddress addressWithString:[self.customer resourceAddressString]]; }
	else
	{ return nil; }
}

- (NSString *) typeString
{
	switch ([[self typeInteger] intValue])
	{
		case 1: return @"Customer";
		case 2: return @"Site";
		case 3: return @"Device";
		case 4: return @"Container";
		case 5: return @"Object";
		case 6: return @"Metric";
		case 7: return @"Trigger";
		default: return @"Unknown";
	}
}

- (LCEntityAddress *) entityAddress
{ return [LCEntityAddress addressForEntity:self]; }

- (id) entityDescriptor
{ return [LCEntityDescriptor descriptorForEntity:self]; }

- (id) entityViewController
{ return [LCEntityViewController controllerForEntity:self]; }

- (id) viewController
{ return [LCEntityViewController controllerForEntity:self]; }

- (BOOL) isDeviceLeaf
{
	if ([[self typeInteger] intValue] == 1) return NO;
	else if (![self parent]) return YES;
	else if ([[[self parent] typeInteger] intValue] == 2) return YES;
	else return NO;
}

- (BOOL) isBrowserTreeLeaf
{
	if ([[self typeInteger] intValue] == 1) return NO;
	else if (![self parent]) return YES;
	else if ([[[self parent] typeInteger] intValue] >= 2) return YES;
	else return NO;
}	

- (BOOL) isContainerTreeLeaf
{
	return YES;
}

- (BOOL) isObjectTreeLeaf
{
	return NO;
}

- (BOOL) isGroupTreeLeaf
{
	return YES;
}

- (BOOL) selectable
{ return YES; }

- (NSString *) longDisplayString
{
	/* IS NOT KVO COMPLIANT (by design) */
	switch ([[self typeInteger] intValue])
	{
		case 1:
			return [NSString stringWithFormat:@"%@", [self displayString]];
		case 2:
			return [NSString stringWithFormat:@"%@ (%@) Location", [self displayString], [[self customer] displayString]];
		case 3:
			return [NSString stringWithFormat:@"%@ at %@ (%@)", [self displayString], [[self site] displayString], [[self customer] displayString]];
		case 4: 
			return [NSString stringWithFormat:@"%@ on %@ at %@ (%@)", 
					[self displayString], [[self device] displayString], [[self site] displayString], [[self customer] displayString]];
		case 5: 
			return [NSString stringWithFormat:@"%@ on %@ at %@ (%@)", [self displayString], [[self device] displayString], [[self site] displayString], [[self customer] displayString]];
		case 6: 
			return [NSString stringWithFormat:@"%@ %@ Metric on %@ at %@ (%@)", [[self object] displayString], [self displayString], [[self device] displayString], [[self site] displayString], [[self customer] displayString]];
		case 7: 
			return [NSString stringWithFormat:@"%@ %@ %@ Trigger on %@ at %@ (%@)", [[self object] displayString], [[self metric] displayString], [self displayString],  [[self device] displayString], [[self site] displayString], [[self customer] displayString]];
		default:
			return nil;
	}
}

@synthesize browserViewType;

@synthesize displayValue;

- (NSString *) uniqueIdentifier
{ return [[self entityAddress] addressString]; }

- (BOOL) actionFilterLeaf
{
	if (self.type >= 3) return YES;
	else return NO;
}

#pragma mark "Legacy Accessors FIX"

- (NSString *) typeInteger		
{ return [NSString stringWithFormat:@"%i", self.type]; }

- (NSString *) adminstateInteger
{ return [NSString stringWithFormat:@"%i", self.adminState]; }

- (NSString *) opstateInteger
{ return [NSString stringWithFormat:@"%i", self.opState]; }

- (NSNumber *) indexNumber
{ return [NSNumber numberWithInt:self.entityIndex]; }

#pragma mark "Ugly Accessors"

//- (BOOL) recentlyRestarted
//{ 
//	if ([[properties objectForKey:@"refresh_count"] intValue] < 2)
//	{ return YES; }
//	else
//	{ return NO; }
//}


@end
