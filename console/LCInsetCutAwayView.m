//
//  LCInsetCutAwayView.m
//  Lithium Console
//
//  Created by James Wilson on 27/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCInsetCutAwayView.h"


@implementation LCInsetCutAwayView

- (void) drawRect:(NSRect)rects
{
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
//	[image drawInRect:NSMakeRect(NSMinX(self.bounds) + shadowWidth, NSMaxY(self.bounds)-shadowWidth, NSWidth(self.bounds) - (2 * shadowWidth), shadowWidth) 
	[image drawInRect:NSMakeRect(NSMinX(self.bounds) + shadowWidth, NSMaxY(self.bounds)-shadowWidth, NSWidth(self.bounds) - (1 * shadowWidth), shadowWidth) 
			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0)
			operation:NSCompositeSourceOver 
			 fraction:shadowAlpha];
		
//	image = [NSImage imageNamed:@"topright.png"];
//	[image drawInRect:NSMakeRect(NSMaxX(self.bounds) - shadowWidth, NSMaxY(self.bounds)-shadowWidth, shadowWidth, shadowWidth) 
//			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0)
//			operation:NSCompositeSourceOver 
//			 fraction:shadowAlpha];
	
	image = [NSImage imageNamed:@"left.png"];
	[image drawInRect:NSMakeRect(NSMinX(self.bounds), NSMinY(self.bounds), shadowWidth, NSHeight(self.bounds)-shadowWidth) 
			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0)
			operation:NSCompositeSourceOver 
			 fraction:shadowAlpha];
	
//	image = [NSImage imageNamed:@"right.png"];
//	[image drawInRect:NSMakeRect(NSMaxX(self.bounds) - shadowWidth, NSMinY(self.bounds), shadowWidth, NSHeight(self.bounds)-shadowWidth) 
//			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0)
//			operation:NSCompositeSourceOver 
//			 fraction:shadowAlpha];

	NSBezierPath *path = [NSBezierPath bezierPathWithRect:self.bounds];
	[[NSColor colorWithCalibratedWhite:0.0 alpha:0.2] setFill];
	[path fill];	
	
	if (![[self window] isMainWindow])
	{
		[[NSColor colorWithCalibratedWhite:0.7 alpha:0.2] setFill];
		[[NSBezierPath bezierPathWithRect:[self bounds]] fill];
	}	
}


@end
