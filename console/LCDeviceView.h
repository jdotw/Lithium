//
//  LCDeviceView.h
//  Lithium Console
//
//  Created by James Wilson on 23/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCDevice.h"

@interface LCDeviceView : NSView 
{
	/* Device */
	LCDevice *device;
}

- (BOOL) drawWarnings;
- (BOOL) hasWarnings;

@property (nonatomic,retain) LCDevice *device;

@end
