//
//  LCMetricGraphDocument.m
//  Lithium Console
//
//  Created by James Wilson on 7/10/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCMetricGraphDocument.h"

#import "LCBrowser2Controller.h"
#import "LCCaseController.h"
#import "LCFaultHistoryController.h"
#import "LCMetricHistoryWindowController.h"
#import "LCTriggerTuningWindowController.h"
#import "LCMetricAnalysisWindowController.h"
#import "LCMetricGraphMetricItem.h"

@implementation LCMetricGraphDocument

#pragma mark "Constructors"

#pragma mark "Window Controller Methods"

- (NSString *) windowNibName 
{
    return @"MetricGraphWindow";
}

- (void)windowControllerDidLoadNib:(NSWindowController *)windowController
{
	/* Set initial values */
	if (initialReferenceDate) graphController.referenceDate = self.initialReferenceDate;
	if (initialGraphPeriod > 0) graphController.graphPeriod = self.initialGraphPeriod;
	for (LCMetric *metric in [LCMetricGraphController graphableMetricsForEntities:initialEntities])
	{
		LCMetricGraphMetricItem *item = [LCMetricGraphMetricItem new];
		item.metric = metric;
		[graphController insertObject:item inMetricItemsAtIndex:[graphController.metricItems count]];
		[item autorelease];
	}
	[initialEntities release];
	initialEntities = nil;

	/* Setup undo */
	graphController.undoResponder = (NSResponder *)self;

	/* Setup window */
	[backgroundView setImage:[NSImage imageNamed:@"flatgrey.png"]];
	if ([self windowFrame].size.width > 0 && [self windowFrame].size.height > 0)
	{
		if (NSPointInRect(NSMakePoint([self windowFrame].origin.x, [self windowFrame].origin.y), [[NSScreen mainScreen] visibleFrame]))
		{ [[self windowForSheet] setFrame:self.windowFrame display:NO]; }
		
	}
	tableView.allowDeleteKey = YES;
	
	/* Add KVO Observer */
	[metricArrayController addObserver:self
							forKeyPath:@"selection" 
							   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
							   context:NULL];
	
	/* Config */
	[metricArrayController setAllowDrop:YES];
	graphController.getMinMaxAvgValues = YES;

	/* Refresh */
	[self refreshGraphClicked:self];
	
	/* Add auto-refresh timer */
	refreshTimer = [NSTimer scheduledTimerWithTimeInterval:60.0
													target:self
												  selector:@selector(refreshTimerFired:)
												  userInfo:nil
												   repeats:YES];
	
	documentLoading = NO;
}

- (void) dealloc
{
	[refreshTimer release];
	[super dealloc];
}

#pragma mark "Document Save and Load Methods"

- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError
{
	NSMutableData *data = [NSMutableData data];
	NSKeyedArchiver *archiver = [[NSKeyedArchiver alloc] initForWritingWithMutableData:data];
	
	NSMutableArray *entityArray = [NSMutableArray array];
	for (LCMetricGraphMetricItem *item in graphController.metricItems)
	{ [entityArray addObject:item.metric]; }
	[archiver encodeObject:entityArray forKey:@"metricArray"];
	[archiver encodeInt:graphController.graphPeriod forKey:@"graphPeriod"];
	[archiver encodeObject:graphController.referenceDate forKey:@"referenceDate"];
	[archiver encodeRect:self.windowFrame forKey:@"windowFrame"];
	
	[archiver finishEncoding];
	
	[archiver autorelease];
	
    return data;
}

- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{
	documentLoading = YES;
	
	NSKeyedUnarchiver *unarchiver = [[NSKeyedUnarchiver alloc] initForReadingWithData:data];
	self.initialEntities = [unarchiver decodeObjectForKey:@"metricArray"];
	self.initialGraphPeriod = [unarchiver decodeIntForKey:@"graphPeriod"];
	self.initialReferenceDate = [unarchiver decodeObjectForKey:@"referenceDate"];
	self.windowFrame = [unarchiver decodeRectForKey:@"windowFrame"];
	[unarchiver finishDecoding];
	[unarchiver autorelease];
	
    return YES;
}

#pragma mark "Toolbar Methods"

- (void) buildToolbar
{
	NSToolbarItem *tbitem;
	
	/* Create toolbar */
	toolbar = [[NSToolbar alloc] initWithIdentifier: @"MetricGraph"];
	[toolbar setAutosavesConfiguration:YES];
	[toolbar setAllowsUserCustomization: NO];
	[toolbar setDelegate: self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconOnly];
	[toolbar setSizeMode:NSToolbarSizeModeSmall];
	
	/* Create arrays/dict */
	toolbarItems = [[NSMutableDictionary dictionary] retain];	
	toolbarSelectableItems = [[NSMutableArray array] retain];
	toolbarDefaultItems = [[NSMutableArray array] retain];
	
	/* Refresh graph */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Refresh Graph"] autorelease];
	[tbitem setLabel:@"Refresh Graph"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (refreshGraphClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Refresh.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Refresh Graph"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
		
	/* Date Selector */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Reference Date"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setView:dateSelectorView];
	[tbitem setMinSize:[dateSelectorView bounds].size];
	[tbitem setMaxSize:[dateSelectorView bounds].size];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Period Selector */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Graph Period"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setView:periodSelectorView];
	[tbitem setMinSize:[periodSelectorView bounds].size];
	[tbitem setMaxSize:[periodSelectorView bounds].size];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
	/* Trend Ananlysis */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Trend Analysis"] autorelease];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (analyseSelectedEntityClicked:)];
	[tbitem setLabel:@"Trend Analysis"];
	[tbitem setImage:[NSImage imageNamed:@"TrendAnalysis.tiff"]];
	[tbitem setToolTip:@"Select a Metric from the list below then click here to perform Trend Analysis on that Metric"];
	[toolbarItems setObject:tbitem forKey:@"Trend Analysis"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	trendAnalysisItem = tbitem;
	
	/* Metric History */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Metric History"] autorelease];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (metricHistoryForSelectedEntityClicked:)];
	[tbitem setLabel:@"Metric History"];
	[tbitem setImage:[NSImage imageNamed:@"MetricHistory.tiff"]];
	[tbitem setToolTip:@"Select a Metric from the list below then click here to retrieve historical values for that Metric"];
	[toolbarItems setObject:tbitem forKey:@"Metric History"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	metricHistoryItem = tbitem;
	
	/* Spacer */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarFlexibleSpaceItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
	/* Refresh Selector */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:@"Refresh Indicator"] autorelease];
	[tbitem setLabel:@""];
	[tbitem setTarget: self];
	[tbitem setView:refreshIndicatorView];
	[tbitem setMinSize:[refreshIndicatorView bounds].size];
	[tbitem setMaxSize:[refreshIndicatorView bounds].size];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
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

#pragma mark "Menubar Items"

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item
{
	SEL action = [item action];
	
	if (action == @selector(analyseSelectedEntityClicked:) || action == @selector(metricHistoryForSelectedEntityClicked:))
	{
		if ([metricArrayController selectedObjects] > 0)
		{ return YES; }
		else
		{ return NO; }
	}
	return YES;
}

#pragma mark "Toolbar Action Methods"

- (IBAction) refreshGraphClicked:(id)sender
{
	[graphController refreshGraph:XMLREQ_PRIO_HIGH];
}

- (void) refreshTimerFired:(NSTimer *)timer
{
	[graphController refreshGraph:XMLREQ_PRIO_HIGH];
}
	
#pragma mark "Entity Action Methods"

- (IBAction) removeSelectedEntityClicked:(id)sender
{
	for (LCMetricGraphMetricItem *item in [metricArrayController selectedObjects])
	{
		[graphController removeObjectFromMetricItemsAtIndex:[graphController.metricItems indexOfObject:item]];
		[self refreshGraphClicked:self];
	}	
}

- (IBAction) refreshSelectedEntityClicked:(id)sender
{
	for (LCMetricGraphMetricItem *item in [metricArrayController selectedObjects])
	{
		[[item.metric device] highPriorityRefresh];
	}
}

- (IBAction) graphSelectedEntityClicked:(id)sender
{
	for (LCMetricGraphMetricItem *item in [metricArrayController selectedObjects])
	{
		LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
		document.initialEntity = item.metric;
		[[NSDocumentController sharedDocumentController] addDocument:document];
		[document makeWindowControllers];
		[document showWindows];			
	}
}

- (IBAction) browseToSelectedEntityClicked:(id)sender
{
	for (LCMetricGraphMetricItem *item in [metricArrayController selectedObjects])
	{
		[[[LCBrowser2Controller alloc] initWithEntity:item.metric] autorelease];
	}
}

- (IBAction) openCaseForSelectedEntityClicked:(id)sender
{
	NSMutableArray *entities = [NSMutableArray array];
	for (LCMetricGraphMetricItem *item in [metricArrayController selectedObjects])
	{
		[entities addObject:item.metric];
	}
	[[LCCaseController alloc] initForNewCaseWithEntityList:entities];
}

- (IBAction) faultHistoryForSelectedEntityClicked:(id)sender
{
	for (LCMetricGraphMetricItem *item in [metricArrayController selectedObjects])
	{
		[[LCFaultHistoryController alloc] initForEntity:item.metric];
	}
}

- (IBAction) metricHistoryForSelectedEntityClicked:(id)sender
{
	for (LCMetricGraphMetricItem *item in [metricArrayController selectedObjects])
	{
		[[LCMetricHistoryWindowController alloc] initWithMetric:item.metric];
	}
}

- (IBAction) analyseSelectedEntityClicked:(id)sender
{
	for (LCMetricGraphMetricItem *item in [metricArrayController selectedObjects])
	{
		[[LCMetricAnalysisWindowController alloc] initWithObject:[item.metric object]];
	}
}

- (IBAction) triggerTuningForSelectedEntityClicked:(id)sender
{
	for (LCMetricGraphMetricItem *item in [metricArrayController selectedObjects])
	{
		[[LCTriggerTuningWindowController alloc] initWithObject:[item.metric object]];
	}
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	if (object == metricArrayController && [keyPath isEqualToString:@"selection"])
	{
		if ([[metricArrayController selectedObjects] count] > 0)
		{
			[trendAnalysisItem setTarget: self];
			[trendAnalysisItem setAction: @selector (analyseSelectedEntityClicked:)];
			[metricHistoryItem setTarget: self];
			[metricHistoryItem setAction: @selector (metricHistoryForSelectedEntityClicked:)];
		}
		else
		{
			[trendAnalysisItem setTarget: nil];
			[trendAnalysisItem setAction: nil];
			[metricHistoryItem setTarget: nil];
			[metricHistoryItem setAction: nil];
		}
	}
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Remove KVO */
	[metricArrayController removeObserver:self forKeyPath:@"selection"];
	
	/* Cancel any refresh */
	if ([graphController refreshInProgress] == YES)
	{ [graphController cancelRefresh]; }
	
	/* Remove content */
	[controllerAlias setContent:nil];
	[graphControllerAlias setContent:nil];
}

#pragma mark "Date Picker"

@synthesize dateDropDownTag;
- (void) setDateDropDownTag:(int)value
{
	dateDropDownTag = value;
	if (dateDropDownTag)
	{ 
		originalDate = [graphController.referenceDate copy];
		[NSApp beginSheet:datePickerSheet
		   modalForWindow:[self windowForSheet]
			modalDelegate:self
		   didEndSelector:@selector(datePickerSheetEnded:)
			  contextInfo:nil];
	}
}

- (void) datePickerSheetEnded:(id)sender
{
	self.dateDropDownTag = 0;
}

- (IBAction) datePickerSelectClicked:(id)sender
{
	[originalDate release];
	[NSApp endSheet:datePickerSheet];
	[datePickerSheet close];
}

- (IBAction) datePickerCancelClicked:(id)sender
{
	graphController.referenceDate = originalDate;
	[originalDate release];
	[NSApp endSheet:datePickerSheet];
	[datePickerSheet close];
}

#pragma mark "Property Methods"

@synthesize graphController;

@synthesize initialEntities;
- (LCEntity *) initialEntity
{
	if ([self.initialEntities count] > 0) return [self.initialEntities objectAtIndex:0];
	else return nil;
}
- (void) setInitialEntity:(LCEntity *)entity
{
	self.initialEntities = [NSArray arrayWithObject:entity];
}

@synthesize initialGraphPeriod;
@synthesize initialReferenceDate;

@synthesize windowFrame;


@end
