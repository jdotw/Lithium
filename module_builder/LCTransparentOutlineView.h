//
//  LCTransparentOutlineView.h
//  Lithium Console
//
//  Created by James Wilson on 18/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCOutlineView.h"

@interface LCTransparentOutlineView : LCOutlineView 
{
	NSColor *backgroundTintColor;
}

- (void) awakeFromNib;
- (BOOL) isOpaque;
- (void) drawBackgroundInClipRect:(NSRect)clipRect;
- (void) drawRect:(NSRect)rect;
- (id)_highlightColorForCell:(NSCell *)cell;
- (void) ROUNDhighlightSelectionInClipRect:(NSRect)clipRect;
- (NSBezierPath *) outlinePath:(NSRect)frame;
- (void) highlightSelectionInClipRect:(NSRect)clipRect;
- (void)selectRow:(int)row byExtendingSelection:(BOOL)extend;
- (void)deselectRow:(int)row;
- (NSColor *) backgroundTintColor;
- (void) setBackgroundTintColor:(NSColor *)value;

#pragma mark "Outline Delegate methods"
- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item;
- (void)outlineViewItemWillExpand:(NSNotification *)notification;
- (void)outlineViewItemDidExpand:(NSNotification *)notification;
- (void)outlineViewItemWillCollapse:(NSNotification *)notification;
- (void)outlineViewItemDidCollapse:(NSNotification *)notification;

@end
