//
//  LCInspMiniGraphItem.m
//  Lithium Console
//
//  Created by James Wilson on 5/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspMiniGraphItem.h"

#import "LCMetric.h"
#import "LCObject.h"

@implementation LCInspMiniGraphItem

+ (id) itemWithTarget:(id)initTarget forController:(id)initController
{
	return [[[LCInspMiniGraphItem alloc] initWithTarget:initTarget forController:initController] autorelease];
}

+ (LCInspMiniGraphItem *) itemForMinorMetricsWithTarget:(id)initTarget forController:(id)initController
{
	return [[[LCInspMiniGraphItem alloc] initForMinorMetricsWithTarget:initTarget forController:initController] autorelease];
}

+ (LCInspMiniGraphItem *) itemWithTarget:(id)initTarget metrics:(NSArray *)initMetrics forController:(id)initController
{
	return [[[LCInspMiniGraphItem alloc] initForTarget:initTarget withMetrics:initMetrics forController:initController] autorelease];
}

- (id) initWithTarget:(id)initTarget forController:(id)initController
{
	[super initWithTarget:initTarget forController:initController];
	
	if ([target object] == target)
	{
		/* Create main graph */
		[self setDisplayString:@"Object Graph"];
		LCInspMiniGraphViewController *viewController = (LCInspMiniGraphViewController *) [[LCInspMiniGraphViewController alloc] initWithTarget:initTarget];
		[viewController setRowHeight:120.0];
		[self insertObject:viewController inViewControllersAtIndex:0];
		[viewController autorelease];
	}
	else if ([target container] == target)
	{
		NSArray *graphableMetrics = [[target container] graphableMetrics];
		NSMutableDictionary *objectDict = [NSMutableDictionary dictionary];
		for (LCMetric *metric in graphableMetrics)
		{
			if (![objectDict objectForKey:[metric.parent name]])
			{ [objectDict setObject:metric.parent forKey:[metric.parent name]]; }
		}
		NSArray *objects = [objectDict allValues];
		int count = 0;
		for (LCObject *obj in objects)
		{
			/* Check if the obj is an interface */
			if ([[[obj parent] name] isEqualToString:@"iface"])
			{
				/* Skip iface if it's down */
				NSString *opStateValue = [[(LCMetric *) [obj childNamed:@"opstate"] currentValue] valueString];
				if (![opStateValue isEqualToString:@"Up"])
				{ continue; }
			}
			
			/* Create view */
			if ([[LCMetricGraphController graphableMetricsForEntities:[NSArray arrayWithObject:obj]] count] > 0)
			{
				LCInspMiniGraphViewController *viewController = (LCInspMiniGraphViewController *) [[LCInspMiniGraphViewController alloc] initWithTarget:obj];
				[viewController setShowDesc:YES];
				[viewController setRowHeight:80.0];
				[self insertObject:viewController inViewControllersAtIndex:[viewControllers count]];
				[viewController autorelease];
			}
			count++;
			if (count >= 24) break;
		}
		if (count >= 24) [self setDisplayString:@"Object Graphs (12+)"];
		else [self setDisplayString:[NSString stringWithFormat:@"Object Graphs (%i)", count]];
	}
	
	return self;
}

- (id) initForMinorMetricsWithTarget:(id)initTarget forController:(id)initController
{
	[super initWithTarget:initTarget forController:initController];

	/* Create minor graphs */
	NSArray *mainGraphMetrics = [LCMetricGraphController graphableMetricsForEntities:[NSArray arrayWithObject:target]];
	NSEnumerator *metricEnum = [[target children] objectEnumerator];
	LCMetric *metric;
	while (metric=[metricEnum nextObject])
	{
		if ([metric isGraphable] && ![mainGraphMetrics  containsObject:metric])
		{ 
			LCInspMiniGraphViewController *viewController = (LCInspMiniGraphViewController *) [[LCInspMiniGraphViewController alloc] initWithTarget:metric];
			[viewController setRowHeight:80.0];
			[self insertObject:viewController inViewControllersAtIndex:[viewControllers count]];
			[viewController release];
		}
	}

	[self setDisplayString:[NSString stringWithFormat:@"Other Metric Graphs (%i)", [viewControllers count]]];
	if ([viewControllers count] < 3)
	{ [self setExpandByDefault:YES]; }
	else
	{ [self setExpandByDefault:NO]; }
	
	return self;
}

- (id) initForTarget:(id)initTarget withMetrics:(NSArray *)initMetrics forController:(id)initController
{
	[super initWithTarget:initTarget forController:initController];
	
	/* Create graphs */
	for (LCMetric *metric in initMetrics)
	{
		if ([metric isGraphable])
		{ 
			LCInspMiniGraphViewController *viewController = (LCInspMiniGraphViewController *) [[LCInspMiniGraphViewController alloc] initWithTarget:metric];
			[viewController setRowHeight:80.0];
			[self insertObject:viewController inViewControllersAtIndex:[viewControllers count]];
			[viewController release];
		}
	}
	
	[self setDisplayString:[NSString stringWithFormat:@"Metric Graphs (%i)", [viewControllers count]]];
	[self setExpandByDefault:YES];
	
	return self;	
}

- (BOOL) allowsResize
{ return YES; }

@end
