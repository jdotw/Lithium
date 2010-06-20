//
//  LCInspDeviceRefreshViewController.m
//  Lithium Console
//
//  Created by James Wilson on 7/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspDeviceRefreshViewController.h"


@implementation LCInspDeviceRefreshViewController

+ (id) itemWithTarget:(id)initTarget
{
	return [[[LCInspDeviceRefreshViewController alloc] initWithTarget:initTarget] autorelease];
}

- (id) initWithTarget:(id)initTarget
{
	[super initWithTarget:initTarget];
	
	return self;
}

- (float) rowHeight
{ return 40.0; }

- (NSString *) nibName
{ return @"InspectorDeviceRefreshView"; }

@end
