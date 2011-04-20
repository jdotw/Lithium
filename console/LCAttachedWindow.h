//
//  LCAttachedWindow.h
//  Lithium Console
//
//  Created by James Wilson on 11/04/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MAAttachedWindow.h"

@class LCAttachedWindow;

@protocol MAAttachedWindowDelegate

- (void) attachedWindowDidClose:(LCAttachedWindow *)attachedWindow;

@end

@interface LCAttachedWindow : MAAttachedWindow 
{
	BOOL canBecomeKeyWindow;
}

- (LCAttachedWindow *)initWithView:(NSView *)view           // designated initializer
                   attachedToPoint:(NSPoint)point 
                          inWindow:(NSWindow *)window 
                            onSide:(MAWindowPosition)side 
                        atDistance:(float)distance;
- (void)closeAttachedWindow;
- (void) setAlertStyle:(NSAlertStyle)style;

@property (nonatomic,assign) BOOL canBecomeKeyWindow;

@end
