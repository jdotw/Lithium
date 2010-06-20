//
//  LCObjectView.m
//  Lithium Console
//
//  Created by James Wilson on 26/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCObjectView.h"


@implementation LCObjectView

#pragma mark "Hit-Test Method"

- (NSView *)hitTest:(NSPoint)aPoint
{
	if (hitView) return hitView;
	else return self;
}

- (void) drawRect:(NSRect)rect
{
	[super drawRect:rect];
	
	NSBezierPath *path = [NSBezierPath bezierPath];
	[path moveToPoint:NSMakePoint(0, 0)];
	[path lineToPoint:NSMakePoint([self frame].size.width, 0)];
	[[NSColor grayColor] setStroke];
	[path stroke];
}
	
#pragma mark "Accessor Methods"

- (NSView *) hitView
{ return hitView; }
- (void) setHitView:(NSView *)view
{ hitView = view; }

@end
