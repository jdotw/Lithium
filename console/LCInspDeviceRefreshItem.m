//
//  LCInspDeviceRefreshItem.m
//  Lithium Console
//
//  Created by James Wilson on 7/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspDeviceRefreshItem.h"


@implementation LCInspDeviceRefreshItem

+ (id) itemWithTarget:(id)initTarget forController:initController
{
	return [[[LCInspDeviceRefreshItem alloc] initWithTarget:initTarget forController:initController] autorelease];
}

- (id) initWithTarget:(id)initTarget forController:initController
{
	[super initWithTarget:initTarget forController:initController];
	
	[self setDisplayString:@"Device Refresh"];
	
	LCInspDeviceRefreshViewController *viewController = (LCInspDeviceRefreshViewController *) [[LCInspDeviceRefreshViewController alloc] initWithTarget:initTarget];
	[self insertObject:viewController inViewControllersAtIndex:0];
	[viewController autorelease];
	
	return self;
}

- (BOOL) expandByDefault
{ return YES; }

@end
