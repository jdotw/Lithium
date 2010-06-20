//
//  LCBackgroundView.m
//  Lithium Console
//
//  Created by James Wilson on 21/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBackgroundView.h"


@implementation LCBackgroundView

- (void) drawResizeIndicators:(NSRect)rect
{
}

#pragma mark "Drawing Method"

- (void) drawRect:(NSRect)rects
{
	if (image)
	{
		[image drawInRect:[self bounds]
				 fromRect:NSMakeRect(0, 0, [image size].width, [image size].height)
				operation:NSCompositeSourceOver 
				 fraction:1.0];
	}
//	else
//	{
//		[[NSColor colorWithCalibratedRed:232/256.0 green:232/256.0 blue:232/256.0 alpha:1.0] setFill];
//		NSRectFill([self bounds]);
//	}
}

#pragma mark "Accessors"

- (NSImage *) image
{ return image; }

- (void) setImage:(NSImage *)newImage
{
	[image release];
	image = [newImage retain];
	[self setNeedsDisplay:YES];
}

@end
