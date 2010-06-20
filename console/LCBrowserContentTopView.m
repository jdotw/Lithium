//
//  LCBrowserContentTopView.m
//  Lithium Console
//
//  Created by James Wilson on 10/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserContentTopView.h"


@implementation LCBrowserContentTopView

- (BOOL) isOpaque
{ return YES; }

- (void) drawRect:(NSRect)rect
{
	NSBezierPath *path = [NSBezierPath bezierPathWithRect:self.bounds];
	[[NSColor colorWithCalibratedWhite:64.0/256.0 alpha:1.0] setFill];
	[path fill];
	
	NSImage *image = [NSImage imageNamed:@"centre.png"];
	[image drawInRect:self.bounds
			 fromRect:NSMakeRect(0.0, 0.0, image.size.width, image.size.height)
			operation:NSCompositeSourceOver 
			 fraction:1.0];
	
	image = [NSImage imageNamed:@"topleft.png"];
	[image drawInRect:NSMakeRect(NSMinX(self.bounds), NSMaxY(self.bounds)-16.0, 16.0, 16.0) 
			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0)
			operation:NSCompositeSourceOver 
			 fraction:1.0];	
	
	image = [NSImage imageNamed:@"top.png"];
	[image drawInRect:NSMakeRect(NSMinX(self.bounds) + 16.0, NSMaxY(self.bounds)-16.0, NSWidth(self.bounds) - 32.0, 16.0) 
			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0)
			operation:NSCompositeSourceOver 
			 fraction:1.0];
	
	
	image = [NSImage imageNamed:@"topright.png"];
	[image drawInRect:NSMakeRect(NSMaxX(self.bounds) - 16.0, NSMaxY(self.bounds)-16.0, 16.0, 16.0) 
			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0)
			operation:NSCompositeSourceOver 
			 fraction:1.0];
	
	image = [NSImage imageNamed:@"left.png"];
	[image drawInRect:NSMakeRect(NSMinX(self.bounds), NSMinY(self.bounds), 16.0, NSHeight(self.bounds)-16.0) 
			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0)
			operation:NSCompositeSourceOver 
			 fraction:1.0];
	
	image = [NSImage imageNamed:@"right.png"];
	[image drawInRect:NSMakeRect(NSMaxX(self.bounds) - 16.0, NSMinY(self.bounds), 16.0, NSHeight(self.bounds)-16.0) 
			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0)
			operation:NSCompositeSourceOver 
			 fraction:1.0];

	if (![[self window] isMainWindow])
	{
		[[NSColor colorWithCalibratedWhite:0.7 alpha:0.2] setFill];
		[[NSBezierPath bezierPathWithRect:[self bounds]] fill];
	}
	
}

@end
