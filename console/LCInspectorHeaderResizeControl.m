//
//  LCInspectorHeaderResizeControl.m
//  Lithium Console
//
//  Created by James Wilson on 14/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspectorHeaderResizeControl.h"
#import "LCInspectorController.h"

@implementation LCInspectorHeaderResizeControl

- (void) awakeFromNib
{ 
	active = YES; 
}

- (void) drawRect:(NSRect)rect
{
	/* Dont draw for first row */
	int itemIndex = [[[item controller] items] indexOfObject:item];
	if (itemIndex == 0) 
	{
		active = NO;
		return;
	}
	
	/* Check if item supports resize */
	if (![[item controller] itemAboveAllowsResize:item])
	{
		active = NO;
		return;
	}

	/* Draw resizers */
	NSImage *image = [NSImage imageNamed:@"inspresizeindicator.png"];
	[image drawInRect:NSMakeRect(0.0,-1.0,16.0,16.0)
			 fromRect:NSMakeRect(0.0,0.0,16.0,16.0)
			operation:NSCompositeSourceOver
			 fraction:0.4];
}

- (void) mouseDown:(NSEvent *)event
{
	if (!active) return;
	if ([event clickCount] != 2)
	{
		oldPoint = [event locationInWindow];
	}
	else
	{ 
		if ([event modifierFlags] & NSCommandKeyMask)
		{ [(LCInspectorController *)[item controller] adjustAllItemsToDefaultHeight]; }
		else
		{ [(LCInspectorController *)[item controller] adjustItemAboveToDefaultHeight:item]; }
	}
}

- (void) mouseUp:(NSEvent *)event
{
	if (!active) return;
	[[item controller] resizeDragFinished];
}

- (void) mouseDragged:(NSEvent *)event
{
	if (!active) return;
	NSPoint newPoint = [event locationInWindow];
	[(LCInspectorController *)[item controller] adjustItemAbove:item viewHeightsBy:(oldPoint.y - newPoint.y)];
	oldPoint = newPoint;
}

@end
