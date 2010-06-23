//
//  MBPercentMetricViewController.m
//  ModuleBuilder
//
//  Created by James Wilson on 22/02/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MBPercentMetricViewController.h"


@implementation MBPercentMetricViewController

#pragma mark "Constructors"

+ (MBPercentMetricViewController *) viewForMetric:(MBMetric *)initMetric
{
	return [[[MBPercentMetricViewController alloc] initWithMetric:initMetric] autorelease];
}

- (MBPercentMetricViewController *) initWithMetric:(MBMetric *)initMetric
{
	[super initWithNibName:@"PercentMetricView" bundle:nil];
	[self setRepresentedObject:initMetric];
	[self loadView];
	gaugeMetricField.metricProperty = @"gaugeMetric";
	maxMetricField.metricProperty = @"maxMetric";
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark Accessors

- (MBMetric *) metric
{ return [self representedObject]; }

@end
