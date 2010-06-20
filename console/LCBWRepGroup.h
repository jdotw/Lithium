//
//  LCBWRepGroup.h
//  Lithium Console
//
//  Created by James Wilson on 25/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCBWRepItem.h"
#import "LCBWRepDevice.h"

@interface LCBWRepGroup : LCBWRepItem 
{
	NSMutableDictionary *deviceItems;		/* Dictionary of local-level device items, keyed by entity address */
}

#pragma mark "Constructors"
+ (LCBWRepGroup *) group;
- (LCBWRepGroup *) init;

#pragma mark "Children Methods"
- (NSArray *) displayChildren;
- (BOOL) arrangeByDevice;
- (void) setArrangeByDevice:(BOOL)value;

#pragma mark "Accessors"
- (NSString *) displayDescription;
- (void) setDisplayDescription:(NSString *)value;

#pragma mark "Device Methods"
- (NSMutableDictionary *) deviceItems;
- (LCBWRepDevice *) locateDeviceItem:(LCEntity *)device;
- (void) addDeviceItem:(LCBWRepDevice *)item;
- (void) removeDeviceItem:(LCBWRepDevice *)item;

@property (retain,getter=deviceItems) NSMutableDictionary *deviceItems;
@end
