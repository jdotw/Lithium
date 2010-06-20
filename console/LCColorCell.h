//
//  LCColorCell.h
//  Lithium Console
//
//  Created by James Wilson on 20/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCColorCell : NSCell 
{
	NSColor *color;
}

#pragma mark Dealloc
- (void) dealloc;

#pragma mark Drawing Methods
- (void) drawWithFrame:(NSRect) cellFrame inView:(NSView *) controlView;

#pragma mark Accessors
- (NSColor *) color;
- (void) setColor:(NSColor *)newColor;

@property (retain,getter=color,setter=setColor:) NSColor *color;
@end
