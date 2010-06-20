//
//  LCBrowserTableCornerView.m
//  Lithium Console
//
//  Created by James Wilson on 7/04/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCBrowserTableCornerView.h"


@implementation LCBrowserTableCornerView

- (void) drawRect:(NSRect)rect
{
	NSImage *backImage = [NSImage imageNamed:@"tablecornerback.png"];
	[backImage drawInRect:[self bounds]
				 fromRect:NSMakeRect(0,0,[backImage size].width,[backImage size].height)
				operation:NSCompositeSourceOver
				 fraction:1.0];
	
//	NSBezierPath *path = [NSBezierPath bezierPathWithRect:NSMakeRect([self bounds].origin.x, [self bounds].origin.y + 1, [self bounds].size.width-1, [self bounds].size.height-2)];
//	[[NSColor colorWithCalibratedRed:75/255.0 green:75/255.0 blue:75/255.0 alpha:1.0] setFill];
//	[path fill];
	
//	NSImage *backImage = [NSImage imageNamed:@"browsertvheadback.png"];
//	[backImage setFlipped:NO];
//	[backImage drawInRect:NSMakeRect([self bounds].origin.x, [self bounds].origin.y, [self bounds].size.width, [self bounds].size.height-1)
//				 fromRect:NSMakeRect(0, 0, [backImage size].width, [backImage size].height) 
//				operation:NSCompositeSourceIn
//				 fraction:1.0];
	
//	path = [NSBezierPath bezierPath];
//	[path moveToPoint:NSMakePoint([self bounds].origin.x, [self bounds].origin.y-1)];
//	[path lineToPoint:NSMakePoint([self bounds].origin.x+[self bounds].size.width, [self bounds].origin.y-1)];
//	[path setLineWidth:0.5];
//	[[NSColor colorWithDeviceWhite:1.0 alpha:0.4] setStroke];
//	[path stroke];	

	
}

@end
