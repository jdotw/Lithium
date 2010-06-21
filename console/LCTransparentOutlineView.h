//
//  LCTransparentOutlineView.h
//  Lithium Console
//
//  Created by James Wilson on 18/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCOutlineView.h"

@interface LCTransparentOutlineView : LCOutlineView 
{
	NSColor *backgroundTintColor;
}

- (BOOL) isOpaque;
- (void) drawBackgroundInClipRect:(NSRect)clipRect;
- (void) drawRect:(NSRect)rect;
- (id)_highlightColorForCell:(NSCell *)cell;
- (void) ROUNDhighlightSelectionInClipRect:(NSRect)clipRect;
- (NSBezierPath *) outlinePath:(NSRect)frame;
- (void) highlightSelectionInClipRect:(NSRect)clipRect;

@property (nonatomic,retain) NSColor *backgroundTintColor;

@end
