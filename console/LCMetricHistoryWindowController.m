//
//  LCMetricHistoryWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 19/07/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMetricHistoryWindowController.h"
#import "LCMetricAnalysisWindowController.h"
#import "LCMetricGraphDocument.h"

@implementation LCMetricHistoryWindowController

#pragma mark "Initialisation"

- (id) initWithMetric:(LCMetric *)initMetric
{
	/* Create history class */
	history = [[LCMetricHistory historyWithMetric:initMetric] retain];
	[history refresh:XMLREQ_PRIO_HIGH];

	/* Set entity */
	[self setMetric:initMetric];
	
	/* Super-class init */
	[self initWithWindowNibName:@"MetricHistoryWindow"];
	[self window];
	[backView setImage:[NSImage imageNamed:@"slateback.png"]];
	
	/* Build toolbar */
	[self buildToolbar];
	
	/* Show */
	[[self window] makeKeyAndOrderFront:self];
	
	return self;
}

- (void) dealloc
{
	[metric release];
	[history release];
	[super dealloc];
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Autorelease */
	[self autorelease];
}

#pragma mark "Toolbar Methods"

- (void) buildToolbar
{
	NSToolbarItem *tbitem;
	
	/* Create toolbar */
	toolbar = [[NSToolbar alloc] initWithIdentifier: @"MetricHistoryToolbar"];
	[toolbar setAutosavesConfiguration:YES];
	[toolbar setAllowsUserCustomization: NO];
	[toolbar setDelegate: self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setSizeMode:NSToolbarSizeModeSmall];
	
	/* Create arrays/dict */
	toolbarItems = [[NSMutableDictionary dictionary] retain];	
	toolbarSelectableItems = [[NSMutableArray array] retain];
	toolbarDefaultItems = [[NSMutableArray array] retain];

	/* Refresh entities */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Refresh Data"] autorelease];
	[tbitem setLabel:@"Refresh Data"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (refreshClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Refresh.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Refresh Data"];
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
	
	/* Graph metric */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Graph Metric"] autorelease];
	[tbitem setLabel:@"Graph Metric"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (graphMetricClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Chart.tiff"]];
	[tbitem setToolTip:@"Click here to graph this Metric"];
	[toolbarItems setObject:tbitem forKey:@"Graph Metric"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Analyse */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Trend Analysis"] autorelease];
	[tbitem setLabel:@"Trend Analysis"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (trendAnalysisClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"TrendAnalysis.tiff"]];
	[tbitem setToolTip:@"Click here to perform Trend Analysis for this Metric"];
	[toolbarItems setObject:tbitem forKey:@"Trend Analysis"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		

	/* Export */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Export to CSV"] autorelease];
	[tbitem setLabel:@"Export to CSV"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (exportToCSVClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Export.tiff"]];
	[tbitem setToolTip:@"Click here to export the table data to a CSV formatted file"];
	[toolbarItems setObject:tbitem forKey:@"Export to CSV"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
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
	
	[[self window] setToolbar:toolbar];
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

- (IBAction) toggleToolbarClicked:(id)sender
{ [[self window] toggleToolbarShown:sender]; }

#pragma mark "Toolbar Actions"

- (void) refreshClicked:(id)sender
{
	[history refresh:XMLREQ_PRIO_HIGH];
}

- (void) graphMetricClicked:(id)sender
{
	LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
	document.initialEntity = [self metric];
	[[NSDocumentController sharedDocumentController] addDocument:document];
	[document makeWindowControllers];
	[document showWindows];		
}

- (void) trendAnalysisClicked:(id)sender
{
	[[LCMetricAnalysisWindowController alloc] initWithObject:[[self metric] object]];
}

- (IBAction) exportToCSVClicked:(id)sender
{
	NSSavePanel *savePanel = [NSSavePanel savePanel];
	[savePanel beginSheetForDirectory:nil
								 file:[NSString stringWithFormat:@"%@ %@ %@ %@.csv", [metric.device desc], [metric.container desc], [metric.object desc], metric.desc]
					   modalForWindow:[self window]
						modalDelegate:self
					   didEndSelector:@selector(exportSavePanelDidEnd:returnCode:contextInfo:)
						  contextInfo:nil];
}

- (void) exportSavePanelDidEnd:(NSSavePanel *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		NSMutableString *csvString = [NSMutableString stringWithFormat:@"timestamp,date,minimum,average,maximum\n"];
		for (LCMetricHistoryRow *row in history.rows)
		{
			[csvString appendFormat:@"%.0f,%@,%.2f,%.2f,%.2f\n", 
			 [row.timeStamp timeIntervalSince1970], [row.timeStamp description], 
			 row.minimum, row.average, row.maximum];			 
		}
		[csvString writeToFile:[sheet filename] atomically:YES encoding:NSUTF8StringEncoding error:nil];
	}
}


#pragma mark "Accessors"

- (LCMetric *) metric
{ return metric; }
- (void) setMetric:(LCMetric *)newEntity
{
	if (metric) [metric release];
	metric = [newEntity retain];
}

- (LCMetricHistory *) history
{ return history; }

@synthesize history;
@synthesize toolbar;
@synthesize toolbarItems;
@synthesize toolbarSelectableItems;
@synthesize toolbarDefaultItems;
@synthesize dateSelectorView;
@synthesize periodSelectorView;
@synthesize refreshIndicatorView;
@synthesize tableView;
@synthesize backView;
@end
