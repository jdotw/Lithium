//
//  LCViewCell.h
//  Lithium Console
//
//  Created by James Wilson on 21/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCViewCell : NSCell 
{
	NSView *subView;
}

#pragma Initialisation 
- (void) dealloc;

#pragma View Manipulation
- (NSView *) subView;
- (void) setSubView:(NSView *)view;

#pragma Drawing Methods
- (void) drawWithFrame:(NSRect) cellFrame inView:(NSView *) controlView;

@property (assign,getter=subView,setter=setSubView:) NSView *subView;
@end
