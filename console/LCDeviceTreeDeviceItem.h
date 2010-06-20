//
//  LCDeviceTreeDeviceItem.h
//  Lithium Console
//
//  Created by James Wilson on 4/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDevice.h"
#import "LCDeviceTreeItem.h"

@interface LCDeviceTreeDeviceItem : LCDeviceTreeItem 
{
	LCDevice *device;
}

- (id) initWithDevice:(LCDevice *)initDevice;
@property (retain) LCDevice *device;

@end
