//
//  LCInspMiniGraphViewController.m
//  Lithium Console
//
//  Created by James Wilson on 5/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspMiniGraphViewController.h"


@implementation LCInspMiniGraphViewController

+ (LCInspMiniGraphViewController *) itemWithTarget:(id)initTarget
{
	return [[[LCInspMiniGraphViewController alloc] initWithTarget:initTarget] autorelease];
}

- (id) initWithTarget:(id)initTarget
{
	[super initWithTarget:initTarget];
	
	metrics = [[LCMetricGraphController graphableMetricsForEntities:[NSArray arrayWithObject:target]] retain];
	[graphController addMetricsFromArray:metrics];
	[graphController refreshGraph:XMLREQ_PRIO_HIGH];
	graphView.overlayLabel = [target displayString];
	graphView.fontSize = 9.0;
	graphView.dragAndDropEnabled = NO;
	graphView.showDates = NO;
	
	return self;
}

- (void) dealloc
{
	[metrics release];
	[super dealloc];
}

#pragma mark "View Management"

- (void) removedFromInspector
{
	/* Prepare to be released and dealloced */
	[graphView setGraphController:nil];
	[super removedFromInspector];
}

@synthesize showDesc;

@synthesize metrics;

- (float) defaultHeight
{ return 80.0; }

- (NSString *) nibName
{ return @"InspectorMiniGraphView"; }

@end
