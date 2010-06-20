//
//  LCInspServiceConfigItem.m
//  Lithium Console
//
//  Created by James Wilson on 8/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspServiceConfigItem.h"


@implementation LCInspServiceConfigItem

+ (id) itemWithTarget:(id)initTarget forController:(id)initController
{
	return [[[LCInspServiceConfigItem alloc] initWithTarget:initTarget forController:initController] autorelease];
}

- (id) initWithTarget:(id)initTarget forController:(id)initController
{
	[super initWithTarget:initTarget forController:initController];
	
	[self setDisplayString:@"Monitored Service"];
	
	LCInspServiceConfigViewController *viewController = (LCInspServiceConfigViewController *) [[LCInspServiceConfigViewController alloc] initWithTarget:initTarget];
	[self insertObject:viewController inViewControllersAtIndex:0];
	[viewController release];
	
	return self;
}

@end
