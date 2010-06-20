//
//  LCColorCell.m
//  Lithium Console
//
//  Created by James Wilson on 20/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCColorCell.h"


@implementation LCColorCell

#pragma mark Dealloc

- (void) dealloc
{ 
	[color release];
	[super dealloc];
}

#pragma mark Drawing Methods
- (void) drawWithFrame:(NSRect) cellFrame inView:(NSView *) controlView
{
	[[self objectValue] setFill];
	NSRectFill(cellFrame);
}

#pragma mark Accessors

- (NSColor *) color
{ return color; }
- (void) setColor:(NSColor *)newColor
{ 
	[color release];
	color = [newColor retain]; 
}

@end
