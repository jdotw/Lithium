//
//  MBRateMetricViewController.m
//  ModuleBuilder
//
//  Created by James Wilson on 22/02/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MBRateMetricViewController.h"

@implementation MBRateMetricViewController

#pragma mark "Constructors"

+ (MBRateMetricViewController *) viewForMetric:(MBMetric *)initMetric
{
	return [[[MBRateMetricViewController alloc] initWithMetric:initMetric] autorelease];
}

- (MBRateMetricViewController *) initWithMetric:(MBMetric *)initMetric
{
	[super initWithNibName:@"RateMetricView" bundle:nil];
	[self setRepresentedObject:initMetric];
	[self loadView];
	counterMetricField.metricProperty = @"counterMetric";
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark Accessors

- (MBRateMetric *) metric
{ return [self representedObject]; }


@end
