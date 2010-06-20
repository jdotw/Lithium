//
//  LCInspEntityTreeItem.m
//  Lithium Console
//
//  Created by James Wilson on 4/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspEntityTreeItem.h"
#import "LCInspEntityTreeViewController.h"

@implementation LCInspEntityTreeItem

+ (id) itemWithTarget:(id)initTarget forController:(id)initController
{
	return [[[LCInspEntityTreeItem alloc] initWithTarget:initTarget forController:initController] autorelease];
}

- (id) initWithTarget:(id)initTarget forController:(id)initController 
{
	[super initWithTarget:initTarget forController:initController];
	
	[self setDisplayString:@"Entity Tree"];

	LCInspEntityTreeViewController *viewController = (LCInspEntityTreeViewController *) [[LCInspEntityTreeViewController alloc] initWithTarget:initTarget];
	[self insertObject:viewController inViewControllersAtIndex:0];
	[viewController autorelease];					  

	return self;
}

@end
