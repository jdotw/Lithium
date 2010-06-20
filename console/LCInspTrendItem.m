//
//  LCInspTrendItem.m
//  Lithium Console
//
//  Created by James Wilson on 5/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspTrendItem.h"

#import "LCMetric.h"

@implementation LCInspTrendItem

+ (id) itemWithTarget:(id)initTarget forController:(id)initController
{
	return [[[LCInspTrendItem alloc] initWithTarget:initTarget forController:initController] autorelease];
}

- (id) initWithTarget:(id)initTarget forController:(id)initController
{
	[super initWithTarget:initTarget forController:initController];
	
	[self setDisplayString:@"Trend Analysis"];
	
	LCInspTrendViewController *viewController = (LCInspTrendViewController *) [[LCInspTrendViewController alloc] initWithTarget:initTarget];
	[self insertObject:viewController inViewControllersAtIndex:0];
	[viewController autorelease];
	
	return self;
}

- (BOOL) expandByDefault
{ return YES; }

+ (BOOL) targetHasTriggers:(id)target
{
	for (LCMetric *met in [target children])
	{
		if ([[met children] count] < 1 || !met.recordEnabled) continue;
		else return YES;
	}
	
	return NO;
}

@end
