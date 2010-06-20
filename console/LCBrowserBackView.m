
//
//  LCBrowserBackView.m
//  Lithium Console
//
//  Created by James Wilson on 14/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserBackView.h"


@implementation LCBrowserBackView

#pragma mark "Drawing Method"

- (void) awakeFromNib
{
	[self setDrawsBackground:YES];
	[self setAutoresizesSubviews:YES];
}

- (void) drawRect:(NSRect)rects
{
	NSBezierPath *path = [NSBezierPath bezierPathWithRect:self.bounds];
	[[NSColor colorWithCalibratedWhite:64.0/256.0 alpha:1.0] setFill];
	[path fill];
	
	CGFloat shadowWidth = 14.0;
	CGFloat shadowAlpha = 1.0;
	
	NSImage *image = [NSImage imageNamed:@"centre.png"];
	[image drawInRect:self.bounds
			 fromRect:NSMakeRect(0.0, 0.0, image.size.width, image.size.height)
			operation:NSCompositeSourceOver 
			 fraction:1.0];

	image = [NSImage imageNamed:@"topleft.png"];
	[image drawInRect:NSMakeRect(NSMinX(self.bounds), NSMaxY(self.bounds)-shadowWidth, shadowWidth, shadowWidth) 
			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0)
			operation:NSCompositeSourceOver 
			 fraction:shadowAlpha];	
	
	image = [NSImage imageNamed:@"top.png"];
	[image drawInRect:NSMakeRect(NSMinX(self.bounds) + shadowWidth, NSMaxY(self.bounds)-shadowWidth, NSWidth(self.bounds) - 24.0, shadowWidth) 
			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0)
			operation:NSCompositeSourceOver 
			 fraction:shadowAlpha];
	
	
	image = [NSImage imageNamed:@"topright.png"];
	[image drawInRect:NSMakeRect(NSMaxX(self.bounds) - shadowWidth, NSMaxY(self.bounds)-shadowWidth, shadowWidth, shadowWidth) 
			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0)
			operation:NSCompositeSourceOver 
			 fraction:shadowAlpha];

	image = [NSImage imageNamed:@"left.png"];
	[image drawInRect:NSMakeRect(NSMinX(self.bounds), NSMinY(self.bounds), shadowWidth, NSHeight(self.bounds)-shadowWidth) 
			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0)
			operation:NSCompositeSourceOver 
			 fraction:shadowAlpha];

	image = [NSImage imageNamed:@"right.png"];
	[image drawInRect:NSMakeRect(NSMaxX(self.bounds) - shadowWidth, NSMinY(self.bounds), shadowWidth, NSHeight(self.bounds)-shadowWidth) 
			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0)
			operation:NSCompositeSourceOver 
			 fraction:shadowAlpha];
	
	if (![[self window] isMainWindow])
	{
		[[NSColor colorWithCalibratedWhite:0.7 alpha:0.2] setFill];
		[[NSBezierPath bezierPathWithRect:[self bounds]] fill];
	}	
}

- (BOOL) drawsBackground
{ return drawsBackground; }

- (void) setDrawsBackground:(BOOL)flag
{ drawsBackground = flag; }


@end
