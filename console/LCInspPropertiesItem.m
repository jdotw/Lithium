//
//  LCInspPropertiesItem.m
//  Lithium Console
//
//  Created by James Wilson on 5/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspPropertiesItem.h"


@implementation LCInspPropertiesItem

+ (id) itemWithTarget:(id)initTarget forController:(id)initController
{
	return [[[LCInspPropertiesItem alloc] initWithTarget:initTarget forController:initController] autorelease];
}

- (id) initWithTarget:(id)initTarget forController:(id)initController
{
	[super initWithTarget:initTarget forController:initController];
	
	[self setDisplayString:@"Properties"];
	
	LCInspPropertiesViewController *viewController = (LCInspPropertiesViewController *) [[LCInspPropertiesViewController alloc] initWithTarget:initTarget];
	[self insertObject:viewController inViewControllersAtIndex:0];
	[viewController autorelease];
	
	return self;
}

- (BOOL) expandByDefault
{ return YES; }

@end
