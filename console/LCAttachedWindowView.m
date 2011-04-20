//
//  LCAttachedWindowView.m
//  Lithium Console
//
//  Created by James Wilson on 11/04/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LCAttachedWindowView.h"
#import "LCAttachedWindow.h"

@implementation LCAttachedWindowView

- (NSView *)hitTest:(NSPoint)aPoint
{
	NSView *view = [super hitTest:aPoint];
	if ([view isMemberOfClass:[NSButton class]]) return view;
	else return self;
}

- (void) mouseUp:(NSEvent *)theEvent
{	
	LCAttachedWindow *attachedWindow = (LCAttachedWindow *)[self window];
	[attachedWindow closeAttachedWindow];	  
}

@end
