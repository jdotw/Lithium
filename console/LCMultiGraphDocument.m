//
//  LCMultiGraphDocument.m
//  Lithium Console
//
//  Created by James Wilson on 9/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMultiGraphDocument.h"
#import "LCMetricGraphMetricItem.h"

@implementation LCMultiGraphDocument

#pragma mark "Initialisation"

- (id)init
{
    [super init];
	
	/* Create properties */
	properties = [[NSMutableDictionary dictionary] retain];
	
	/* Create metric sets array */
	[self setMetricSets:[NSMutableArray array]];

	/* Set config defaults */
	[self setDisplayInterval:[NSNumber numberWithFloat:30.0]];
	[self setReferenceDate:[NSDate dateWithTimeIntervalSinceNow:0.0]];
	[self setUseSpecificReferenceDate:NO];
	[self setGraphPeriod:1];
	
	/* NIB Setup */
	showDrawer = YES;
	
    return self;
}

- (void) dealloc
{
	[properties release];
	[toolbar release];
	[super dealloc];
}

#pragma mark "NSDocument Methods"

- (NSString *)windowNibName
{
    return @"MultiGraphWindow";
}

- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{
	/* Super-init */
    [super windowControllerDidLoadNib:aController];
	[backView setImage:[NSImage imageNamed:@"slateback.png"]];

	/* NIB Setup */
	[graphSetDeviceArrayController setAllowDrop:YES];
	[graphSetMetricArrayController setAllowDrop:YES];

	/* Build toolbar */
	[self buildToolbar];
	
	/* Sync controllers */
	[self updateControllers];
	
	/* Reset display */
	[self resetDisplay];
}

- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError
{	
	return [NSKeyedArchiver archivedDataWithRootObject:properties];
}

- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{    
	[self setProperties:[NSKeyedUnarchiver unarchiveObjectWithData:data]];
    return YES;
}

- (void) windowWillClose:(id)sender
{
	[controllerAlias setContent:nil];
	
	if ([graphAController refreshInProgress] == YES) [graphAController cancelRefresh];
	[graphADeviceAlias setContent:nil];
	
	if ([graphBController refreshInProgress] == YES) [graphBController cancelRefresh];
	[graphBDeviceAlias setContent:nil];
	
	if ([graphCController refreshInProgress] == YES) [graphCController cancelRefresh];
	[graphCDeviceAlias setContent:nil];

	if ([graphDController refreshInProgress] == YES) [graphDController cancelRefresh];
	[graphDDeviceAlias setContent:nil];

	[displayTimer invalidate];
}

#pragma mark "Graph Display Methods"

- (void) resetDisplay
{
	/* (Re-)Starts the display of graphs */

	/* Set enumerators */
	if (setEnumerator) [setEnumerator release];
	setEnumerator = [[[self metricSets] objectEnumerator] retain];
	
	/* Device enumerator */
	if (deviceEnumerator) [deviceEnumerator release];
	deviceEnumerator = nil;

	/* If we're in playback, kick off a new timer */
	if (isPaused == NO)
	{
		/* Create timer */
		if (displayTimer)
		{ [displayTimer invalidate]; }
		displayTimer = [NSTimer scheduledTimerWithTimeInterval:[[self displayInterval] floatValue]
														target:self 
													  selector:@selector(displayTimerFired)
													  userInfo:nil 
													   repeats:YES];
		
		/* Fire */
		[NSTimer scheduledTimerWithTimeInterval:0.0
										 target:self 
									   selector:@selector(displayTimerFired)
									   userInfo:nil 
										repeats:NO];
	}
}

- (void) displayTimerFired
{
	/* For each of the four graphs, a new device
	 * and set of metrics is found and created. The
	 * list of metrics is sent to the applicable
	 * graph controller and a refresh is called 
	 * for that controller
	 */
	
	int i;

	/* Setup controllers */
	for (i=0; i < 4; i++)
	{
		LCEntity *device;

		/* Get device */
		device = [deviceEnumerator nextObject];
		if (!device)
		{ 
			/* No more devices, move to next set */
			[self setCurrentMetricSet:[setEnumerator nextObject]];
			if (![self currentMetricSet])
			{
				/* Reset enum */
				[setEnumerator release];
				setEnumerator = [[[self metricSets] objectEnumerator] retain];
				[self setCurrentMetricSet:[setEnumerator nextObject]];
				if (![self currentMetricSet])
				{ 
					/* No sets! */
					continue;
				}
			}
			
			/* Create new device enum */
			[deviceEnumerator release];
			deviceEnumerator = [[[[self currentMetricSet] devices] objectEnumerator] retain];
			device = [deviceEnumerator nextObject];
			if (!device)
			{
				/* No device! */
				continue;
			}
		}
		
		/* Create the metrics for this device */
		NSMutableArray *metricArray = [NSMutableArray array];
		for (LCMetricGraphMetricItem *metricItem in currentMetricSet.metrics)
		{
			/* Loops through each metric in the metric set.
			 * Create entity descriptors that describe the metrics
			 * as they would be present on the current device. 
			 * Then 'locate' the entities and add them to the array 
			 * and create the controller
			 */
			
			/* Create the descriptor from the set metric */
			LCEntityDescriptor *entityDesciptor = [LCEntityDescriptor descriptorForEntity:(LCEntity *)metricItem.metric];
			
			/* Edit the descriptor */
			[[entityDesciptor properties] setObject:[device name] forKey:@"dev_name"];
			[[entityDesciptor properties] setObject:[[device site] name] forKey:@"site_name"];
			[[entityDesciptor properties] setObject:[[device customer] name] forKey:@"cust_name"];
			
			/* Locate the entity */
			LCEntity *metric = [entityDesciptor locateEntity:YES];
		
			/* Add the entity to the array */
			[metricArray addObject:metric];
		}
		
		/* Set appropriate variables */
		LCMetricGraphController *graphController;
		NSObjectController *graphDeviceAlias;
		NSObjectController *graphSetAlias;
		switch (i)
		{
			case 0:			/* Graph A */
				graphController = graphAController;
				graphDeviceAlias = graphADeviceAlias;
				graphSetAlias = graphASetAlias;
				break;
			case 1: 
				graphController = graphBController;
				graphDeviceAlias = graphBDeviceAlias;
				graphSetAlias = graphBSetAlias;
				break;
			case 2:
				graphController = graphCController;
				graphDeviceAlias = graphCDeviceAlias;
				graphSetAlias = graphCSetAlias;
				break;
			case 3: 
				graphController = graphDController;
				graphDeviceAlias = graphDDeviceAlias;
				graphSetAlias = graphDSetAlias;
				break;
		}
		[graphController removeAllMetricItems];
		[graphController addMetricsFromArray:metricArray];
		[graphDeviceAlias setContent:device];
		[graphSetAlias setContent:[self currentMetricSet]];

		/* Blank Refresh graph */
		[graphController blankGraph];
		[graphController refreshGraph:XMLREQ_PRIO_HIGH];
	}		
}

#pragma mark "Toolbar Methods"

- (void) buildToolbar
{
	NSToolbarItem *tbitem;
	
	/* Create toolbar */
	toolbar = [[NSToolbar alloc] initWithIdentifier: @"MultiGraph"];
	[toolbar setAutosavesConfiguration:YES];
	[toolbar setAllowsUserCustomization: NO];
	[toolbar setDelegate: self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setSizeMode:NSToolbarSizeModeSmall];
	
	/* Create arrays/dict */
	toolbarItems = [[NSMutableDictionary dictionary] retain];	
	toolbarSelectableItems = [[NSMutableArray array] retain];
	toolbarDefaultItems = [[NSMutableArray array] retain];
	
	/* Drawer */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Toggle Drawer"] autorelease];
	[tbitem setLabel:@"Toggle Drawer"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (toggleDrawerClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Drawer.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Toggle Drawer"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
	/* Play/Pause */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"PlayPause"] autorelease];
	[tbitem setLabel:@"Pause"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (playPauseClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Pause.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"PlayPause"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		

	/* Next */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Next"] autorelease];
	[tbitem setLabel:@"Next"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (nextClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Right.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Next"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarFlexibleSpaceItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Graph Config */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Graph Config"] autorelease];
	[tbitem setLabel:@"Graph Config"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (graphConfigClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Preferences.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Graph Config"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Set toolbar */
	[[self windowForSheet] setToolbar:toolbar];
}

- (NSToolbarItem*) toolbar: (NSToolbar*) theToolbar
	 itemForItemIdentifier: (NSString*) identifier 
 willBeInsertedIntoToolbar: (BOOL) flag 
{ return [toolbarItems objectForKey: identifier]; }

- (NSArray*) toolbarAllowedItemIdentifiers: (NSToolbar*) theToolbar 
{ return toolbarDefaultItems; }

- (NSArray*) toolbarDefaultItemIdentifiers: (NSToolbar*) theToolbar 
{ return toolbarDefaultItems; }

- (NSArray*) toolbarSelectableItemIdentifiers: (NSToolbar*) theToolbar 
{ return toolbarSelectableItems; }

#pragma mark "Toolbar Actions"

- (void) toggleDrawerClicked:(id)sender
{
	if ([self showDrawer] == NO)
	{ [self setShowDrawer:YES]; }
	else
	{ [self setShowDrawer:NO]; }
}

#pragma mark "Graph Config Methods"

- (void) graphConfigClicked:(id)sender
{
	/* Open modal configuration sheet */
	[NSApp beginSheet:configSheet 
	   modalForWindow:[self windowForSheet] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];
}

- (IBAction) graphConfigCloseClicked:(id)sender
{
	/* Close sheet */
	[NSApp endSheet:configSheet];
	[configSheet close];

	/* Record change */
	[self updateChangeCount:NSChangeDone];
	
	/* Sync controllers */
	[self updateControllers];
	
	/* Reset display */
	[self resetDisplay];
}

#pragma mark "Graph Set Methods"

- (IBAction) graphSetAddClicked:(id)sender
{
	/* Create new graph set */
	newGraphSet = [[LCMetricGraphSet graphSet] retain];
	editGraphSet = nil;
	
	/* Set content */
	[editGraphSetAlias setContent:newGraphSet];
	
	/* Set button */
	[graphSetCreateButton setTitle:@"Create"];
	
	/* Open modal graphset sheet */
	[NSApp beginSheet:graphSetSheet 
	   modalForWindow:[self windowForSheet]
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];	
}

- (IBAction) graphSetRemoveClicked:(id)sender
{
	/* Check something is selected */
	if ([[metricSetArrayController selectedObjects] count] < 1) return;

	/* Remove each selected */
	NSEnumerator *selectedEnum = [[metricSetArrayController selectedObjects] objectEnumerator];
	LCMetricGraphSet *set;
	while (set = [selectedEnum nextObject])
	{
		[self removeObjectFromMetricSetsAtIndex:[[self metricSets] indexOfObject:set]];
	}

	/* Record change */
	[self updateChangeCount:NSChangeDone];
}

- (IBAction) graphSetCreateOrUpdateClicked:(id)sender
{
	/* Create/Update Set */
	if (newGraphSet)
	{		
		/* Add new set */
		[self insertObject:newGraphSet inMetricSetsAtIndex:[[self metricSets] count]];
		[self updateChangeCount:NSChangeDone];
	}
	else if (editGraphSet)
	{
		/* Update existing */
		[self updateChangeCount:NSChangeDone];
	}
	
	/* Re-set content */
	[editGraphSetAlias setContent:nil];
	if (newGraphSet) [newGraphSet release];
	newGraphSet = nil;
	editGraphSet = nil;
	
	/* Close Sheet */
	[NSApp endSheet:graphSetSheet];
	[graphSetSheet close];
	
	/* Reset display */
	[self resetDisplay];
}

- (IBAction) graphSetCancelClicked:(id)sender
{
	/* Remove temp objects */
	[editGraphSetAlias setContent:nil];
	if (newGraphSet) [newGraphSet release];
	newGraphSet = nil;
	editGraphSet = nil;
	
	/* Close Sheet */
	[NSApp endSheet:graphSetSheet];
	[graphSetSheet close];
}

#pragma mark "Table View Methods"

- (void) setTableViewDoubleClicked:(NSArray *)selectedObjects
{
	/* CHeck something is selected */
	if ([selectedObjects count] < 1) return;
	
	/* Create new graph set */
	newGraphSet = nil;
	editGraphSet = [selectedObjects objectAtIndex:0];
	
	/* Set content */
	[editGraphSetAlias setContent:editGraphSet];
	
	/* Set button */
	[graphSetCreateButton setTitle:@"Update"];
	
	/* Open modal graphset sheet */
	[NSApp beginSheet:graphSetSheet 
	   modalForWindow:[self windowForSheet]
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];
}

#pragma mark "Playback Control Methods"

- (IBAction) playPauseClicked:(id)sender
{
	if (isPaused)
	{
		/* System is paused, resume playback */

		/* Create timer */
		if (displayTimer)
		{ [displayTimer invalidate]; }
		displayTimer = [NSTimer scheduledTimerWithTimeInterval:[[self displayInterval] floatValue]
														target:self 
													  selector:@selector(displayTimerFired)
													  userInfo:nil 
													   repeats:YES];
		
		/* Fire */
		[NSTimer scheduledTimerWithTimeInterval:0.0
										 target:self 
									   selector:@selector(displayTimerFired)
									   userInfo:nil 
										repeats:NO];
		
		/* Adjust button back to pause mode */
		[[toolbarItems objectForKey:@"PlayPause"] setImage:[NSImage imageNamed:@"Pause.tiff"]];
		[[toolbarItems objectForKey:@"PlayPause"] setLabel:@"Pause"];

		/* Set flag */
		isPaused = NO;
	}
	else
	{
		/* System is in playback, pause it */
		[displayTimer invalidate];
		displayTimer = nil;

		/* Adjust button back to play mode */
		[[toolbarItems objectForKey:@"PlayPause"] setImage:[NSImage imageNamed:@"Play.tiff"]];
		[[toolbarItems objectForKey:@"PlayPause"] setLabel:@"Play"];

		/* Set flag */
		isPaused = YES;
	}
}

- (IBAction) nextClicked:(id)sender
{
	/* Display next set of graphs */
	[self displayTimerFired];
}

#pragma mark "Controller syncrhonisation Methods"

- (void) updateControllers
{
	/* Update graph period */
	[graphAController setGraphPeriod:[self graphPeriod]];
	[graphBController setGraphPeriod:[self graphPeriod]];
	[graphCController setGraphPeriod:[self graphPeriod]];
	[graphDController setGraphPeriod:[self graphPeriod]];

	/* Update reference date */
	if ([self useSpecificReferenceDate] == YES)
	{
		/* Set reference date */
		[graphAController setReferenceDate:[self referenceDate]];
		[graphBController setReferenceDate:[self referenceDate]];
		[graphCController setReferenceDate:[self referenceDate]];
		[graphDController setReferenceDate:[self referenceDate]];
	}
	else
	{
		/* Un-set reference date */
		[graphAController setReferenceDate:nil];
		[graphBController setReferenceDate:nil];
		[graphCController setReferenceDate:nil];
		[graphDController setReferenceDate:nil];
	}
}

#pragma mark "Accessor Methods"

- (NSMutableDictionary *) properties
{ return properties; }
- (void) setProperties:(NSMutableDictionary *)dict
{ 
	if (properties) [properties release];
	properties = [dict retain];
}

- (NSMutableArray *) metricSets
{ return [properties objectForKey:@"metricSets"]; }
- (void) setMetricSets:(NSMutableArray *)array
{ [properties setObject:array forKey:@"metricSets"]; }

- (void) insertObject:(LCMetricGraphSet *)set inMetricSetsAtIndex:(unsigned int)index
{ [[self metricSets] insertObject:set atIndex:index]; }
- (void) removeObjectFromMetricSetsAtIndex:(unsigned int)index
{ [[self metricSets] removeObjectAtIndex:index]; }

- (LCMetricGraphSet *) currentMetricSet
{ return currentMetricSet; }
- (void) setCurrentMetricSet:(LCMetricGraphSet *)set
{ 
	if (currentMetricSet)
	{ [currentMetricSet release]; }
	currentMetricSet = [set retain];
	[currentSetAlias setContent:currentMetricSet];
}

- (BOOL) showDrawer
{ return showDrawer; }
- (void) setShowDrawer:(BOOL)flag
{ showDrawer = flag; }

- (NSNumber *) displayInterval
{ return [properties objectForKey:@"displayInterval"]; }
- (void) setDisplayInterval:(NSNumber *)number
{ [properties setObject:number forKey:@"displayInterval"]; }

- (int) graphPeriod
{ return [[properties objectForKey:@"graphPeriod"] intValue]; }
- (void) setGraphPeriod:(int)period
{ [properties setObject:[NSNumber numberWithInt:period] forKey:@"graphPeriod"]; }

- (BOOL) useSpecificReferenceDate
{ return [[properties objectForKey:@"useSpecificReferenceDate"] boolValue]; }
- (void) setUseSpecificReferenceDate:(BOOL)flag
{ [properties setObject:[NSNumber numberWithBool:flag] forKey:@"useSpecificReferenceDate"]; }

- (NSDate *) referenceDate;
{ return [properties objectForKey:@"referenceDate"]; }
- (void) setReferenceDate:(NSDate *)date;
{ [properties setObject:date forKey:@"referenceDate"]; }

@synthesize controllerAlias;
@synthesize graphSetMetricArrayController;
@synthesize graphSetDeviceArrayController;
@synthesize metricSetArrayController;
@synthesize configSheet;
@synthesize graphSetSheet;
@synthesize graphSetCreateButton;
@synthesize editGraphSetAlias;
@synthesize currentSetAlias;
@synthesize backView;
@synthesize newGraphSet;
@synthesize editGraphSet;
@synthesize setEnumerator;
@synthesize deviceEnumerator;
@synthesize displayTimer;
@synthesize isPaused;
@synthesize graphAController;
@synthesize graphBController;
@synthesize graphCController;
@synthesize graphDController;
@synthesize graphADeviceAlias;
@synthesize graphBDeviceAlias;
@synthesize graphCDeviceAlias;
@synthesize graphDDeviceAlias;
@synthesize graphASetAlias;
@synthesize graphBSetAlias;
@synthesize graphCSetAlias;
@synthesize graphDSetAlias;
@synthesize toolbar;
@synthesize toolbarItems;
@synthesize toolbarSelectableItems;
@synthesize toolbarDefaultItems;
@end
