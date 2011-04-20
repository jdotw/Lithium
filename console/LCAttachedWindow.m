//
//  LCAttachedWindow.m
//  Lithium Console
//
//  Created by James Wilson on 11/04/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LCAttachedWindow.h"


@implementation LCAttachedWindow

@synthesize canBecomeKeyWindow;

- (LCAttachedWindow *)initWithView:(NSView *)view           // designated initializer
                   attachedToPoint:(NSPoint)point 
                          inWindow:(NSWindow *)window 
                            onSide:(MAWindowPosition)side 
                        atDistance:(float)distance;
{
	self = [super initWithView:view
			   attachedToPoint:point 
					  inWindow:window
						onSide:side
					atDistance:distance];
	
	[self setBorderWidth:1.];
	[self setCornerRadius:4.3];
	[self setBackgroundColor:[NSColor colorWithCalibratedWhite:0.0 alpha:0.8]];
	[self setHasShadow:YES];
	
	return self;
}

- (BOOL)canBecomeKeyWindow
{
    return canBecomeKeyWindow;
}

- (void)closeAttachedWindow
{
	[[self parentWindow] removeChildWindow:self];
	[self orderOut:self];
	if ([[self delegate] respondsToSelector:@selector(attachedWindowDidClose:)])
	{ [[self delegate] performSelector:@selector(attachedWindowDidClose:) withObject:self]; }
}

- (void) setAlertStyle:(NSAlertStyle)style
{
	switch (style)
	{
		case NSCriticalAlertStyle:
			[self setBorderColor:[NSColor colorWithCalibratedRed:1.0 green:0.0 blue:0.0 alpha:0.6]];
			break;
	}
}

@end
