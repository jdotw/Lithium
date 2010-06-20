//
//  LCVRackInterface.m
//  Lithium Console
//
//  Created by James Wilson on 14/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCVRackInterface.h"


@implementation LCVRackInterface

#pragma mark Constructors

- (id) initWithEntity:(LCEntity *)initEntity atRect:(NSRect)initRect
{
	self = [super init];
	if (!self) return nil;

	self.entity = initEntity;
	self.rect = initRect;
	self.path = [NSBezierPath bezierPathWithRect:[self rect]];

	[[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.0] setFill];
	[[self path] fill];
	
	return self;
}

- (void) dealloc
{
	[entity release];
	[path release];
	[super dealloc];
}

#pragma mark Accessors

@synthesize rect;
@synthesize entity;
@synthesize path;
@synthesize deviceBackView;

@end
