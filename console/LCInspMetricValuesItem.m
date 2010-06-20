//
//  LCInspMetricValuesItem.m
//  Lithium Console
//
//  Created by James Wilson on 27/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCInspMetricValuesItem.h"


@implementation LCInspMetricValuesItem

+ (LCInspMetricValuesItem *) itemWithTarget:(id)initTarget metrics:(NSArray *)initMetrics forController:(id)initController
{
	return [[[LCInspMetricValuesItem alloc] initWithTarget:initTarget metrics:initMetrics forController:initController] autorelease];
}

- (LCInspMetricValuesItem *) initWithTarget:(id)initTarget metrics:(NSArray *)initMetrics forController:(id)initController
{
	[super initWithTarget:initTarget forController:initController];
	
	[self setDisplayString:@"Metric Values"];
	
	LCInspMetricValuesViewController *viewController = (LCInspMetricValuesViewController *) [[LCInspMetricValuesViewController alloc] initWithTarget:initTarget metrics:initMetrics];
	[self insertObject:viewController inViewControllersAtIndex:0];
	[viewController autorelease];
	
	return self;
}

@end
