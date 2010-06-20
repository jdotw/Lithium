//
//  LCVRackDevice.h
//  Lithium Console
//
//  Created by James Wilson on 11/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCVRackUnit.h"
#import "LCXMLObject.h"

@interface LCVRackDevice : LCXMLObject
{
	NSString *uuid;
	
	NSColor *colour;
	LCEntity *entity;
	int hostRUindex;
	int size;
	
	id hostRU;
}

#pragma mark Constructors
+ (LCVRackDevice *) rackDeviceWithEntity:(LCEntity *)targetEntity;
- (id) initWithEntity:(LCEntity *)targetEntity;

#pragma mark Colour
@property (copy) NSColor *colour;
- (NSColor *) colourWithAlpha:(float)alpha;

#pragma mark Accessors
@property (copy) NSString *uuid;
@property (retain) LCEntity *entity;
@property (assign) int hostRUindex;
@property (assign) int size;
@property (retain) id hostRU;

@end
