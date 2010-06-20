//
//  LCInspectorViewCell.m
//  Lithium Console
//
//  Created by James Wilson on 4/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspectorViewCell.h"


@implementation LCInspectorViewCell

#pragma Drawing Methods

- (void) drawWithFrame:(NSRect) cellFrame inView:(NSView *) controlView
{
	NSRect viewRect;
	CGFloat cellWidth = NSWidth([controlView bounds]) - 3.0;
	
	if (lastRow && !headerRow)
	{
		/* Last row */
		viewRect = NSMakeRect(1.0, NSMinY(cellFrame), cellWidth, NSHeight([controlView bounds]) - NSMinY(cellFrame));
	}
	else
	{
		/* Normal */
		viewRect = NSMakeRect(1.0,NSMinY(cellFrame), cellWidth,NSHeight(cellFrame));
	}

	if (headerRow)
	{
		NSRect backgroundFrame = NSMakeRect(NSMinX(viewRect), NSMinY(viewRect),
											NSWidth([[controlView enclosingScrollView] frame]), 
											NSHeight(viewRect));	
		NSBezierPath *backgroundPath = [NSBezierPath bezierPathWithRect:backgroundFrame];
		[[NSColor colorWithCalibratedWhite:68.0/256.0 alpha:1.0] setFill];
		[backgroundPath fill];
		
		NSImage *headerGradient = [NSImage imageNamed:@"ObjectHeaderGradient.png"];
		[headerGradient drawInRect:backgroundFrame
						  fromRect:NSMakeRect(0.0, 0.0, 18.0, 18.0) 
						 operation:NSCompositeSourceOver 
						  fraction:1.0];
		
		/* Top Bars */
		NSBezierPath *line = [NSBezierPath bezierPath];
		[line moveToPoint:NSMakePoint(NSMinX(viewRect), NSMinY(cellFrame)-0.5)];
		[line lineToPoint:NSMakePoint(NSWidth([[controlView enclosingScrollView] frame]), NSMinY(cellFrame)-0.5)];
		[line setLineWidth:1.0];
		[[NSColor colorWithCalibratedWhite:42.0/256.0 alpha:0.76] setStroke];
		[line stroke];		
	}
	else
	{
		/* Content */
		NSImage *bottomGradient = [NSImage imageNamed:@"IndentTopGradient.png"];	/* It's flipped, hence the mismatch filename */
		[bottomGradient drawInRect:NSMakeRect(NSMinX(viewRect), NSMaxY(viewRect) - 3.0f, NSWidth([[controlView enclosingScrollView] frame]), 3.0f)
						  fromRect:NSMakeRect(0.0, 0.0, 6.0, 6.0)
						 operation:NSCompositeSourceOver 
						  fraction:0.1];
		NSImage *topGradient = [NSImage imageNamed:@"IndentBottomGradient.png"];	/* It's flipped, hence the mismatch filename */
		[topGradient drawInRect:NSMakeRect(NSMinX(viewRect), NSMinY(viewRect), NSWidth([[controlView enclosingScrollView] frame]), 4.0f)
					   fromRect:NSMakeRect(0.0, 0.0, 6.0, 6.0)
					  operation:NSCompositeSourceOver 
					   fraction:0.18];
	}		
		
    [[self subView] setFrame:viewRect];
	
    if ([[self subView] superview] != controlView)
    {
		[controlView addSubview: [self subView]];
    }
}

- (BOOL) lastRow
{ return lastRow; }

- (void) setLastRow:(BOOL)flag
{ lastRow = flag; }

- (BOOL) headerRow
{ return headerRow; }

- (void) setHeaderRow:(BOOL)flag
{ headerRow = flag; }

@end
