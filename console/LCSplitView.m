//
//  LCSplitView.m
//  Lithium Console
//
//  Created by James Wilson on 4/03/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCSplitView.h"


@implementation LCSplitView

#pragma mark Frame Sizes

- (NSArray *) frameStrings
{
	return [NSArray arrayWithObjects:NSStringFromRect([[[self subviews] objectAtIndex:0] frame]), NSStringFromRect([[[self subviews] objectAtIndex:1] frame]), nil];
}

- (void) setFrameStrings:(NSArray *)strings
{
	/* View 0 */
	[[[self subviews] objectAtIndex:0] setFrame:NSRectFromString([strings objectAtIndex:0])];
	
	/* View 1 */
	[[[self subviews] objectAtIndex:1] setFrame:NSRectFromString([strings objectAtIndex:1])];
	
	[self adjustSubviews];
}

- (void)drawDividerInRect:(NSRect)aRect
{
	return; 
	if ([self isVertical])
	{
		NSImage *handle = [NSImage imageNamed:@"vertsplithandle.tif"];
		[handle drawInRect:aRect
				  fromRect:NSMakeRect(0, 0, [handle size].width, [handle size].height)
				 operation:NSCompositeSourceOver
				  fraction:1.0];
		NSImage *button = [NSImage imageNamed:@"vertsplitbutton.tif"];
		NSRect buttonRect = NSMakeRect(NSMinX(aRect), NSMidY(aRect) - ([button size].height * 0.5),
									   NSWidth(aRect), [button size].height);
		[button drawInRect:buttonRect
				  fromRect:NSMakeRect(0, 0, [button size].width, [button size].height)
				 operation:NSCompositeSourceOver
				  fraction:1.0];
	}
	else
	{
		NSImage *handle = [NSImage imageNamed:@"horizsplithandle.tif"];
		[handle setFlipped:YES];
		[handle drawInRect:aRect
				  fromRect:NSMakeRect(0, 0, [handle size].width, [handle size].height)
				 operation:NSCompositeSourceOver
				  fraction:1.0];
		NSImage *button = [NSImage imageNamed:@"horizsplitbutton.tif"];
		[button setFlipped:YES];
		NSRect buttonRect = NSMakeRect(NSMidX(aRect) - ([button size].width * 0.5), NSMinY(aRect), 
									   [button size].width, NSHeight(aRect));
		[button drawInRect:buttonRect
				  fromRect:NSMakeRect(0, 0, [button size].width, [button size].height)
				 operation:NSCompositeSourceOver
				  fraction:1.0];
	}
}

@end
