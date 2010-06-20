//
//  LCInspActiveIncidentsItem.m
//  Lithium Console
//
//  Created by James Wilson on 15/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspActiveIncidentsItem.h"
#import "LCInspActiveIncidentsViewController.h"
#import "LCEntity.h"

@implementation LCInspActiveIncidentsItem

+ (id) itemWithTarget:(id)initTarget forController:(id)initController
{
	return [[[LCInspActiveIncidentsItem alloc] initWithTarget:initTarget forController:initController] autorelease];
}

- (id) initWithTarget:(id)initTarget forController:(id)initController
{
	[super initWithTarget:initTarget forController:initController];
	
	[self setDisplayString:[NSString stringWithFormat:@"Active Incidents (%i)", [[(LCEntity *)target incidents] count]]];
	
	LCInspActiveIncidentsViewController *viewController = (LCInspActiveIncidentsViewController *) [[LCInspActiveIncidentsViewController alloc] initWithTarget:initTarget];
	[self insertObject:viewController inViewControllersAtIndex:0];
	[viewController autorelease];
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

@end
