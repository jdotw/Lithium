//
//  LCXSDriveView.m
//  Lithium Console
//
//  Created by James Wilson on 4/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXSDriveView.h"


@implementation LCXSDriveView

#pragma mark "Constructors"

- (id) initWithDrive:(LCObject *)initDrive slot:(int)index inFrame:(NSRect)frame 
{
    [super initWithFrame:frame];
	
	driveObject = [initDrive retain];
	slot = index;
	
	driveImage = [[NSImage imageNamed:@"xsdrive.png"] retain];

    return self;
}

- (void) dealloc
{
	[driveObject release];
	[super dealloc];
}

#pragma mark "View Management"

- (void) removeFromSuperview
{
	[super removeFromSuperview];
}

#pragma mark "Drawing"

- (void) drawRect:(NSRect)rect
{
	/* Draw drive image */
	[driveImage drawInRect:NSMakeRect(0,0,NSWidth([self bounds]),NSHeight([self bounds]))
				  fromRect:NSMakeRect(0,0,[driveImage size].width,[driveImage size].height)
				 operation:NSCompositeSourceOver
				  fraction:1.0];

	/* Status Dot */
	NSRect statusRect = NSMakeRect(4, NSMaxY([self bounds])-10, 6, 6);
	NSBezierPath *statusPath = [NSBezierPath bezierPathWithOvalInRect:statusRect];
	[[driveObject opStateColor] setFill];
	[statusPath fill];
	[[NSColor grayColor] setStroke];
	[statusPath stroke];

	/* Disk ID */
	NSString *str = [driveObject valueForMetricNamed:@"diskid"];
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.7], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Lucida Grande" size:9.0], NSFontAttributeName,
		nil];
	[str drawAtPoint:NSMakePoint(NSMinX([self bounds])+11, NSMinY([self bounds])+49) withAttributes:attr];	
	
	/* Throughput Window */
	NSRect tputRect = NSMakeRect (NSMinX([self bounds])+10, NSMinY([self bounds])+25, 65, 24);
	[[NSColor colorWithDeviceRed:0.1 green:0.1 blue:0.1 alpha:1.0] setFill];
	NSRectFill(tputRect);

	str = [NSString stringWithFormat:@"R:%@", [driveObject valueForMetricNamed:@"read_bps"]];
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithDeviceRed:119/256.0 green:231/256.0 blue:255/256.0 alpha:0.5], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"DotMatrix" size:8.1], NSFontAttributeName,
		nil];
	[str drawAtPoint:NSMakePoint(NSMinX(tputRect)+3, NSMinY(tputRect)+2.5) withAttributes:attr];

	str = [NSString stringWithFormat:@"W:%@", [driveObject valueForMetricNamed:@"write_bps"]];
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithDeviceRed:119/256.0 green:231/256.0 blue:255/256.0 alpha:0.5], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"DotMatrix" size:8.1], NSFontAttributeName,
		nil];
	[str drawAtPoint:NSMakePoint(NSMinX(tputRect)+3, NSMinY(tputRect)+12) withAttributes:attr];

	/* Capacity */
	str = [driveObject valueForMetricNamed:@"capacity"];
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.7], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Lucida Grande" size:7.0], NSFontAttributeName,
		nil];
	[str drawAtPoint:NSMakePoint(NSMinX([self bounds])+19, NSMinY([self bounds])+15) withAttributes:attr];	
	
	
}

@end
