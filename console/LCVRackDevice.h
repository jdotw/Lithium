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
@property (nonatomic,copy) NSColor *colour;
- (NSColor *) colourWithAlpha:(float)alpha;

#pragma mark Accessors
@property (nonatomic,copy) NSString *uuid;
@property (nonatomic,retain) LCEntity *entity;
@property (nonatomic, assign) int hostRUindex;
@property (nonatomic, assign) int size;
@property (nonatomic,retain) id hostRU;

@end
