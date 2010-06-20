//
//  LCVirtualRackDocument.h
//  Lithium Console
//
//  Created by James Wilson on 17/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCVRackUnit.h"
#import "LCVRackDevice.h"
#import "LCVRackCable.h"
#import "LCVRackCableGroup.h"
#import "LCDocument.h"

@interface LCVRackDocument : LCDocument
{
	NSImage *thumbnail;
	NSMutableArray *rackUnits;
	NSMutableArray *devices;
	NSMutableDictionary *deviceDict;
	NSMutableArray *cables;
	NSMutableDictionary *cableDict;
	NSMutableArray *cableGroups;
	NSMutableDictionary *cableGroupDict;
	id windowController;
}

#pragma mark Thumbnail Accessors
@property (copy) NSImage *thumbnail;

#pragma mark Rack Unit Accessors
@property (readonly) NSMutableArray *rackUnits;

#pragma mark Device Accessors
@property (readonly) NSMutableArray *devices;
- (void) insertObject:(LCVRackDevice *)device inDevicesAtIndex:(unsigned int)index;
- (void) removeObjectFromDevicesAtIndex:(unsigned int)index;
- (void) removeDevice:(LCVRackDevice *)rackDev;
- (LCVRackDevice *) objectInDevicesAtIndex:(unsigned int)index;
- (void) incrementSizeOfDevice:(LCVRackDevice *)rackDev;
- (void) decrementSizeOfDevice:(LCVRackDevice *)rackDev;
@property (readonly) NSMutableDictionary *deviceDict;

#pragma mark Cable Accessors
@property (readonly) NSMutableArray *cables;
- (void) insertObject:(LCVRackCable *)cable inCablesAtIndex:(unsigned int)index;
- (void) removeObjectFromCablesAtIndex:(unsigned int)index;
- (LCVRackCable *) objectInCablesAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *cableDict;

#pragma mark Cable Group Accessors
@property (readonly) NSMutableArray *cableGroups;
- (void) insertObject:(LCVRackCableGroup *)group inCableGroupsAtIndex:(unsigned int)index;
- (void) removeObjectFromCableGroupsAtIndex:(unsigned int)index;
- (LCVRackCableGroup *) objectInCableGroupsAtIndex:(unsigned int)index;
- (void) removeCableGroup:(LCVRackCableGroup *)group;
- (void) bindCablestoCableGroups;
@property (readonly) NSMutableDictionary *cableGroupDict;

@end
