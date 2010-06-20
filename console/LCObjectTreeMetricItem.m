//
//  LCObjectTreeMetricItem.m
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCObjectTreeMetricItem.h"

#import "LCMetric.h"
#import "LCObjectTreeMetricViewController.h"

@implementation LCObjectTreeMetricItem

- (id) initWithObject:(LCObject *)initObject showAllMetrics:(BOOL)showAll
{
	self = [super initWithObject:initObject];
	
	if (self)
	{
		/* Basic Setup */
		self.isObjectTreeLeaf = YES;
		displayedMetrics = [[NSMutableArray array] retain];
		displayedMetricViewControllers = [[NSMutableArray array] retain];

		/* Determine which metrics to show */
		for (LCMetric *metric in object.children)
		{
			if (showAll || metric.recordEnabled || metric.children.count > 0 || metric.showInSummary)
			{
				/* Metric is being recorded and/or has triggers */
				[displayedMetrics addObject:metric];
			}
			
			/* Other specials */
			if ([metric.name isEqualToString:@"adminstate"]) [displayedMetrics addObject:metric];
		}
		
		/* Check to make sure something will be displayed */
		if (displayedMetrics.count == 0)
		{
			/* Do a more relaxed search */
			for (LCMetric *metric in object.children)
			{
				if (metric.recordMethod == 1 || metric.children.count > 0)
				{
					/* Metric could be recorded and/or has triggers */
					[displayedMetrics addObject:metric];
				}
			}
		}
			
		/* Check to make sure something will be displayed */
		if (displayedMetrics.count == 0)
		{
			/* Still nothing, use anything Do a more relaxed search */
			for (LCMetric *metric in object.children)
			{ [displayedMetrics addObject:metric]; }
		}				
		
		/* Sort Metrics */
		/* FIX */	

		/* Create View Controllers */
		for (LCMetric *metric in displayedMetrics)
		{
			LCObjectTreeMetricViewController *viewController;
			viewController = [[LCObjectTreeMetricViewController alloc] initWithMetric:metric];
			[displayedMetricViewControllers addObject:viewController];		
			[viewController autorelease];
		}
		
		/* Calculate row height */
		float metricRowHeight = 16.0f;
		self.rowHeight = (((displayedMetrics.count / 2) + displayedMetrics.count % 2) * metricRowHeight) + 4.0;
		
	}
			
	return self;
}

- (void) dealloc
{
	[displayedMetrics release];
	[displayedMetricViewControllers release];
	[super dealloc];
}

@synthesize displayedMetricViewControllers;

@end
