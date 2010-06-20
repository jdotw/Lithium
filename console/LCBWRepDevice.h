//
//  LCBWRepDevice.h
//  Lithium Console
//
//  Created by James Wilson on 25/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCBWRepItem.h"
#import "LCBWRepInterface.h"

@interface LCBWRepDevice : LCBWRepItem 
{

}

#pragma mark "Constructors"
+ (LCBWRepDevice *) deviceItemWithEntity:(LCEntity *)initEntity;

#pragma mark "Interface Methods"
- (void) removeInterface:(LCBWRepInterface *)iface;

#pragma mark "Device methods"
- (void) removeDevice;

#pragma mark "Accessors"
- (NSString *) displayDescription;

@end
