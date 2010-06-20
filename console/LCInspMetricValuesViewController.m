//
//  LCInspMetricValuesViewController.m
//  Lithium Console
//
//  Created by James Wilson on 27/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCInspMetricValuesViewController.h"


@implementation LCInspMetricValuesViewController

+ (LCInspMetricValuesViewController *) itemWithTarget:(id)initTarget metrics:(NSArray *)initMetrics
{
	return [[[LCInspMetricValuesViewController alloc] initWithTarget:initTarget metrics:initMetrics] autorelease];
}

- (LCInspMetricValuesViewController *) initWithTarget:(id)initTarget metrics:(NSArray *)initMetrics
{
	[super initWithTarget:initTarget];
	
	self.metrics = initMetrics;
	[tableView setRoundedSelection:YES];
	
	return self;
}

- (void) dealloc
{
	[metrics release];
	[super dealloc];
}

- (float) defaultHeight
{ 
	float minHeight = [metrics count] * 13.0;
	if (minHeight < 120.0)
	{
		[[tableView enclosingScrollView] setAutohidesScrollers:YES];
		return minHeight + 2.0; 
	}
	return 120.0; 
}

@synthesize metrics;

- (NSString *) nibName
{ return @"InspectorMetricValuesView"; }

@end
