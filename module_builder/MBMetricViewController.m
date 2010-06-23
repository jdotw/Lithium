//
//  MBMetricViewController.m
//  ModuleBuilder
//
//  Created by James Wilson on 7/09/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MBMetricViewController.h"

#import "MBDocumentWindowController.h"
#import "MBRateMetric.h"
#import "MBPercentMetric.h"
#import "MBTrigger.h"
#import "MBEnumeratorWindowController.h"

@implementation MBMetricViewController

#pragma mark "Constructors"

+ (MBMetricViewController *) viewForMetric:(MBMetric *)initMetric
{
	return [[[MBMetricViewController alloc] initWithMetric:initMetric] autorelease];
}

- (MBMetricViewController *) initWithMetric:(MBMetric *)initMetric
{
	[super initWithNibName:@"MetricView" bundle:nil];
	[self setRepresentedObject:initMetric];
	[self loadView];
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark UI Actions

- (IBAction) newClicked:(id)sender
{
	[NSMenu popUpContextMenu:newMenu withEvent:[NSApp currentEvent] forView:self.view];
}

- (IBAction) recordClicked:(id)sender
{
	if ([[[self metric] recordMethod] intValue] == 0)
	{ [[self metric] setRecordMethod:[NSNumber numberWithInt:1]]; }
	else
	{ [[self metric] setRecordMethod:[NSNumber numberWithInt:0]]; }
}

- (IBAction) summaryClicked:(id)sender
{
	if ([[self metric] showInSummary])
	{ [[self metric] setShowInSummary:NO]; }
	else
	{ [[self metric] setShowInSummary:YES]; }
}

- (IBAction) createRateMetricClicked:(id)sender
{
	MBRateMetric *rateMetric = [MBRateMetric new];
	[rateMetric setDesc:[NSString stringWithFormat:@"%@ Rate", [[self metric] desc]]];
	rateMetric.counterMetric = [self metric];
	[[[self metric] parent] insertObject:rateMetric inChildrenAtIndex:([[[[self metric] parent] children] indexOfObject:[self metric]]+1)];		
}

- (IBAction) createPercentMetricClicked:(id)sender
{
	MBPercentMetric *percentMetric = [MBPercentMetric new];
	[percentMetric setDesc:[NSString stringWithFormat:@"%@ Percent", [[self metric] desc]]];
	percentMetric.gaugeMetric = [self metric];
	[[[self metric] parent] insertObject:percentMetric inChildrenAtIndex:([[[[self metric] parent] children] indexOfObject:[self metric]]+1)];		
}

- (IBAction) addTriggerClicked:(id)sender
{
	MBDocumentWindowController *windowController = [[[self view] window] delegate];
	[windowController newTriggerForMetric:[self metric]];
}

- (IBAction) deleteMetricClicked:(id)sender
{
	MBContainer *container = (MBContainer *) [[self metric] container];
	unsigned int index = [[container children] indexOfObject:[self metric]];
	[container removeObjectFromChildrenAtIndex:index];
}

- (IBAction) enumeratorsClicked:(id)sender
{
	MBEnumeratorWindowController *wc = [[MBEnumeratorWindowController alloc] initWithWindowNibName:@"EnumeratorWindow"];
	wc.metric = [self metric];
	[[wc window] makeKeyAndOrderFront:self];
	[wc autorelease];
}

#pragma mark Accessors

- (MBMetric *) metric
{ return [self representedObject]; }

@end
