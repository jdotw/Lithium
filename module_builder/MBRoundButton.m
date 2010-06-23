//
//  MBRoundButton.m
//  ModuleBuilder
//
//  Created by James Wilson on 7/03/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MBRoundButton.h"

#import "MBRoundButtonCell.h"

@implementation MBRoundButton

+ (Class) cellClass
{
    return [MBRoundButtonCell class];
}

- (void) drawRect:(NSRect)rect
{
	/* Draw Background */
	NSBezierPath *backPath = [NSBezierPath bezierPathWithOvalInRect:self.bounds];
	[[NSColor colorWithCalibratedWhite:0.0 alpha:0.1] setFill];
	[backPath fill];
	
	/* Draw Clicked over-lay */
	if (mouseIsDown)
	{
		NSBezierPath *mouseDownPath = [NSBezierPath bezierPathWithOvalInRect:self.bounds];
		[[NSColor colorWithCalibratedWhite:0.0 alpha:0.1] setFill];
		[mouseDownPath fill];
	}		

	if ([self image])
	{
		[[self image] drawInRect:NSMakeRect(4.0, 4.0, self.bounds.size.width - 8.0, self.bounds.size.height - 8.0)
						fromRect:NSMakeRect(0.0, 0.0, [self image].size.width, [self image].size.height)
					   operation:NSCompositeSourceOver
						fraction:0.6];
	}
}

- (void)mouseDown:(NSEvent *)theEvent
{
	mouseIsDown = YES;
	[super mouseDown:theEvent];
	[self mouseUp:theEvent];
}

- (void)mouseUp:(NSEvent *)theEvent
{
	mouseIsDown = NO;
}

@end
