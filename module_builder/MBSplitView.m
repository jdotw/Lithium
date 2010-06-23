//
//  MBSplitView.m
//  ModuleBuilder
//
//  Created by James Wilson on 16/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "MBSplitView.h"


@implementation MBSplitView

- (void)drawDividerInRect:(NSRect)aRect
{
	if ([self isVertical])
	{
		NSImage *image = [NSImage imageNamed:@"greysplitviewhandle_vert.png"];
		[image setFlipped:YES];
		[image drawInRect:aRect
				 fromRect:NSMakeRect(0,0,[image size].width,[image size].height-1)
				operation:NSCompositeSourceOver 
				 fraction:1.0];
		[super drawDividerInRect:aRect];
	}
	else
	{
		NSImage *image = [NSImage imageNamed:@"greysplitviewhandle.png"];
		[image setFlipped:YES];
		[image drawInRect:aRect
				 fromRect:NSMakeRect(0,0,[image size].width,[image size].height-1)
				operation:NSCompositeSourceOver 
				 fraction:1.0];
		[super drawDividerInRect:aRect];
	}
	
	if (![[self window] isMainWindow])
	{
		[[NSColor colorWithCalibratedWhite:0.6 alpha:0.2] setFill];
		[[NSBezierPath bezierPathWithRect:[self bounds]] fill];
	}	
}

@end
