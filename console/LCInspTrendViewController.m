//
//  LCInspTrendViewController.m
//  Lithium Console
//
//  Created by James Wilson on 5/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspTrendViewController.h"
#import "LCMetricGraphDocument.h"

#import "LCMetric.h"

@implementation LCInspTrendViewController

+ (id) itemWithTarget:(id)initTarget
{
	return [[[LCInspTrendViewController alloc] initWithTarget:initTarget] autorelease];
}

- (id) initWithTarget:(id)initTarget
{
	[super initWithTarget:initTarget];
	
	/* Set up analysis metrics */
	analysisMetrics = [[NSMutableArray array] retain];
	for (LCMetric *met in [target children])
	{
		/* Check if recorded & trigger */
		if ([[met children] count] < 1) continue;
		if (!met.recordEnabled) continue;
		
		/* Create analysis metric */
		LCAnalysisMetric *aMet = [LCAnalysisMetric analysisMetricWithMetric:met];
		[self insertObject:aMet inAnalysisMetricsAtIndex:[analysisMetrics count]];
		[aMet processMetricForPeriod:5 atDate:[NSDate date]];
	}
	
	/* Expand all */
	int i;
	for (i=0; i < [outlineView numberOfRows]; i++)
	{ 
		if ([outlineView levelForRow:i] < 1)
		{ [outlineView expandItem:[outlineView itemAtRow:i]]; }
	}			
	
	return self;
}

- (void) dealloc
{
	[analysisMetrics release];
	[super dealloc];
}

- (float) rowHeight
{ 
	if ([outlineView numberOfRows] == 0) return 40;
	float height = ([outlineView numberOfRows] * 13.0) + 20;
	if (height <= 120.0) return height;
	else return 120.0;
}

- (void) tableViewDoubleClicked:(NSArray *)selectedObjects
{
	if (selectedObjects.count < 1) return;
	LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
	document.initialEntity = [[[selectedObjects objectAtIndex:0] valueForKey:@"entity"] metric];
	[[NSDocumentController sharedDocumentController] addDocument:document];
	[document makeWindowControllers];
	[document showWindows];		
}

@synthesize analysisMetrics;
- (void) insertObject:(LCAnalysisMetric *)metric inAnalysisMetricsAtIndex:(unsigned int)index
{
	[analysisMetrics insertObject:metric atIndex:index];
}
- (void) removeObjectFromAnalysisMetricsAtIndex:(unsigned int)index
{
	[analysisMetrics removeObjectAtIndex:index];
}

- (NSString *) nibName
{ return @"InspectorTrendView"; }

@end
