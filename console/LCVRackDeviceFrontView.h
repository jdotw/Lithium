//
//  LCVRackDeviceFrontView.h
//  Lithium Console
//
//  Created by James Wilson on 12/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCVRackView.h"
#import "LCVRackDevice.h"
#import "LCEntity.h"
#import "LCVRackDeviceView.h"

@interface LCVRackDeviceFrontView : LCVRackDeviceView 
{
	/* Thumbnail */
	BOOL drawThumbnail;
}

#pragma mark "Drawing"
- (void)drawRect:(NSRect)rect;

#pragma mark "Properties"
@property (assign) BOOL drawThumbnail;

@end
