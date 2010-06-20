//
//  LCVRackUnit.h
//  Lithium Console
//
//  Created by James Wilson on 12/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCVRackDevice.h"

@interface LCVRackUnit : NSObject 
{
	int ruIndex;
	id hostedDevice;
	NSRect rect;
}

#pragma mark Accessors
@property (assign) int ruIndex;
@property (retain) id hostedDevice;
@property (assign) NSRect rect;

@end
