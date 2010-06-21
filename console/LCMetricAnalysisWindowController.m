//
//  LCMetricAnalysisWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 24/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMetricAnalysisWindowController.h"
#import "LCAnalysisMetric.h"
#import "LCMetricHistoryWindowController.h"
#import "LCMetricGraphDocument.h"
#import "LCMetric.h"

@implementation LCMetricAnalysisWindowController

#pragma mark "Constructors"

- (LCMetricAnalysisWindowController *) initWithObject:(LCEntity *)initObject
{
	/* Set object */
	[self setObject:initObject];
	
	/* Set defaults */
	[self setDatePeriod:5];
	[self setReferenceDate:[NSDate date]];
	
	/* Set up analysis metrics */
	analysisMetrics = [[NSMutableArray array] retain];
	
	for (LCMetric *met in object.children)
	{
		/* Check if recorded & trigger */
		if (![met children] || [[met children] count] < 1) continue;
		if (met.recordMethod != 1) continue;
		
		/* Create analysis metric */
		LCAnalysisMetric *aMet = [LCAnalysisMetric analysisMetricWithMetric:met];
		[self insertObject:aMet inAnalysisMetricsAtIndex:[analysisMetrics count]];
	}
	
	/* Super-class init */
	[self initWithWindowNibName:@"MetricAnalysisWindow"];
	[self window];
	[self buildToolbar];
	
	/* Set up window */
	[backView setImage:[NSImage imageNamed:@"slateback.png"]];
	[objView setAutoresizesSubviews:YES];
	[objView addSubview:[[object entityViewController] view]];
	[[self window] makeKeyAndOrderFront:self];

	/* Analyse all */
	[self analyseClicked:self];
	
	/* Expand all */
	int i;
	for (i=0; i < [outlineView numberOfRows]; i++)
	{
		[outlineView expandItem:[outlineView itemAtRow:i]];
	}
	
	/* Add KVO Observer */
	[treeController addObserver:self
			 forKeyPath:@"selection" 
				options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
				context:nil];	
	
	return self;
}

- (void) dealloc
{
	[object release];
	[analysisMetrics release];
	
	[super dealloc];
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Remove KVO */
	[treeController removeObserver:self forKeyPath:@"selection"];
	
	/* Set content */
	[controllerAlias setContent:nil];
	
	/* Remove all observers */
	LCAnalysisMetric *metric;
	for (metric in analysisMetrics)
	{
		[metric removeObserver:self forKeyPath:@"refreshInProgress"];		
	}
	
	/* Autorelease */
	[self autorelease];
}

#pragma mark "Toolbar Methods"

- (void) buildToolbar
{
	NSToolbarItem *tbitem;
	
	/* Create toolbar */
	toolbar = [[NSToolbar alloc] initWithIdentifier: @"MetricAnalysis"];
	[toolbar setAutosavesConfiguration:YES];
	[toolbar setAllowsUserCustomization: NO];
	[toolbar setDelegate: self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setSizeMode:NSToolbarSizeModeSmall];
	
	/* Create arrays/dict */
	toolbarItems = [[NSMutableDictionary dictionary] retain];	
	toolbarSelectableItems = [[NSMutableArray array] retain];
	toolbarDefaultItems = [[NSMutableArray array] retain];
	
	/* Refresh graph */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Refresh Analysis"] autorelease];
	[tbitem setLabel:@"Refresh Analysis"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (analyseClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Refresh.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Refresh Analysis"];
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
		
	/* Graph */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Graph Object"] autorelease];
	[tbitem setLabel:@"Graph Object"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (graphObjectClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Chart.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Graph Object"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Metric History */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Metric History"] autorelease];
	[tbitem setLabel:@"Metric History"];
	[tbitem setImage:[NSImage imageNamed:@"MetricHistory.tiff"]];
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
	
	[[self window] setToolbar:toolbar];
	return;
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


#pragma mark "UI Actions"

- (IBAction) analyseClicked:(id)sender
{
	LCAnalysisMetric * item;
	
	for (item in analysisMetrics)
	{
		[item processMetricForPeriod:datePeriod atDate:referenceDate];
	}
}

- (IBAction) metricHistoryClicked:(id)sender
{
	NSEnumerator *selectedEnum = [[treeController selectedObjects] objectEnumerator];
	LCEntity *entity;
	while ((entity = (LCEntity *)[[selectedEnum nextObject] entity]))
	{
		[[LCMetricHistoryWindowController alloc] initWithMetric:[entity metric]];
	}
}

- (IBAction) graphSelectedClicked:(id)sender
{
	for (LCEntity *entity in [treeController selectedObjects])
	{
		LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
		document.initialEntity = entity;
		[[NSDocumentController sharedDocumentController] addDocument:document];
		[document makeWindowControllers];
		[document showWindows];			
	}
}

- (IBAction) graphObjectClicked:(id)sender
{
	LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
	document.initialEntity = object;
	[[NSDocumentController sharedDocumentController] addDocument:document];
	[document makeWindowControllers];
	[document showWindows];			
}

#pragma mark "Refresh In Progress Methods"
- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)representedObject 
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (representedObject == treeController && [keyPath isEqualToString:@"selection"])
	{
		/* Selection changed, validate toolbar items */
		if ([[treeController selectedObjects] count] > 0)
		{
			[metricHistoryItem setTarget: self];
			[metricHistoryItem setAction: @selector (metricHistoryClicked:)];
		}
		else
		{
			[metricHistoryItem setTarget: nil];
			[metricHistoryItem setAction: nil];
		}
	}
	else if ([keyPath isEqualToString:@"refreshInProgress"])
	{
		LCAnalysisMetric * item;
		BOOL result = NO;
		
		for (item in analysisMetrics)
		{
			if ([item refreshInProgress])
			{
				result = YES;
				break;
			}
		}
		[self setRefreshInProgress:result];
	}	
}	

#pragma mark "Accessors"

- (LCEntity *) object
{ return object; }
- (void) setObject:(LCEntity *)newObject
{
	[object release];
	object = [newObject retain];
	
	[objects release];
	objects = [[NSArray arrayWithObject:newObject] retain];
	
	[[object device] highPriorityRefresh];
}

- (NSArray *) objects 
{ return objects; }

- (NSMutableArray *) analysisMetrics
{ return analysisMetrics; }
- (void) insertObject:(LCAnalysisMetric *)metric inAnalysisMetricsAtIndex:(unsigned int)index
{
	[metric addObserver:self
		   forKeyPath:@"refreshInProgress" 
			  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
			  context:nil];
	[analysisMetrics insertObject:metric atIndex:index];
}
- (void) removeObjectFromAnalysisMetricsAtIndex:(unsigned int)index
{
	LCAnalysisMetric *metric = [analysisMetrics objectAtIndex:index];
	[metric removeObserver:self forKeyPath:@"refreshInProgress"];
	[analysisMetrics removeObjectAtIndex:index];
}

- (int) datePeriod
{ return datePeriod; }

- (void) setDatePeriod:(int)period
{ 
	datePeriod = period; 
	[self analyseClicked:self];
}

- (NSDate *) referenceDate
{ return referenceDate; }

- (void) setReferenceDate:(NSDate *)date
{ 
	[referenceDate release];
	referenceDate = [date retain];
	[self analyseClicked:self];
}

-(BOOL) refreshInProgress
{	return refreshInProgress;}

-(void) setRefreshInProgress:(BOOL)flag
{ 
	refreshInProgress = flag;
}

@synthesize objects;
@synthesize analysisMetrics;
@synthesize controllerAlias;
@synthesize backView;
@synthesize objView;
@synthesize contentView;
@synthesize outlineView;
@synthesize treeController;
@synthesize toolbar;
@synthesize toolbarItems;
@synthesize toolbarSelectableItems;
@synthesize toolbarDefaultItems;
@synthesize metricHistoryItem;
@synthesize dateSelectorView;
@synthesize periodSelectorView;
@synthesize refreshIndicatorView;
@end
