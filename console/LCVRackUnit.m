//
//  LCVRackUnit.m
//  Lithium Console
//
//  Created by James Wilson on 12/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCVRackUnit.h"


@implementation LCVRackUnit

- (void) dealloc
{
	[hostedDevice release];
	[super dealloc];
}

#pragma mark Accessors

@synthesize ruIndex;
@synthesize hostedDevice;
@synthesize rect;

@end
