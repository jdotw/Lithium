//
//  LCInspectorContentTableView.m
//  Lithium Console
//
//  Created by James Wilson on 30/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspectorContentTableView.h"


@implementation LCInspectorContentTableView

- (void) drawBackgroundInClipRect:(NSRect)clipRect
{
	int i;
	for (i=0; i < (NSHeight([self bounds]) / ([self rowHeight] + 2)); i++)
	{
		NSRect cellFrame = NSMakeRect(NSMinX([self bounds]),NSMinY([self bounds]) + (i * ([self rowHeight] + 2)),
									  NSWidth([self bounds]), [self rowHeight]+2);
		NSBezierPath *path;
		path = [NSBezierPath bezierPathWithRect:NSMakeRect(NSMinX(cellFrame),NSMinY(cellFrame),
														   NSWidth(cellFrame),NSHeight(cellFrame))];
			
		if (i % 2)
		{
			[[NSColor colorWithCalibratedWhite:0.0 alpha:0.02] setFill];
			[path fill];		
		}
	}	
}

@end
