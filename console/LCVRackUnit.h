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
@property (nonatomic, assign) int ruIndex;
@property (nonatomic,retain) id hostedDevice;
@property (nonatomic, assign) NSRect rect;

@end
