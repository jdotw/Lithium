//
//  LCVRackInterface.h
//  Lithium Console
//
//  Created by James Wilson on 14/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"


@interface LCVRackInterface : NSObject 
{
	NSRect rect;
	NSBezierPath *path;
	LCEntity *entity;

	NSView *deviceBackView;
}

#pragma mark Constructors
- (id) initWithEntity:(LCEntity *)initEntity atRect:(NSRect)initRect;

#pragma mark Accessors
@property (nonatomic, assign) NSRect rect;
@property (nonatomic,retain) LCEntity *entity;
@property (nonatomic,retain) NSBezierPath *path;
@property (nonatomic,retain) NSView *deviceBackView;

@end
