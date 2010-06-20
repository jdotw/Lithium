//
//  LCXRDriveIconView.m
//  Lithium Console
//
//  Created by James Wilson on 26/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXRDriveIconView.h"


@implementation LCXRDriveIconView

- (void) drawRect:(NSRect)theRect
{
	NSImage *iconImage = [NSImage imageNamed:@"Harddisk_32x32.png"];
	[iconImage drawInRect:NSMakeRect(0,0,32,32)
				 fromRect:NSMakeRect(0,0,32,32)
				operation:NSCompositeSourceOver
				 fraction:1.0];
	
	NSRect circleRect = NSMakeRect(8,8,16,16);
	NSBezierPath *circlePath = [NSBezierPath bezierPathWithOvalInRect:circleRect];
	[[NSColor colorWithCalibratedRed:0.2 green:0.2 blue:0.2 alpha:0.8] setFill];
	[circlePath fill];
	
	LCEntity *entity = [viewController entity];
	NSString *driveNumber = [NSString stringWithFormat:@"%i", [[[entity parent] children] indexOfObject:entity]+1];
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:1.0], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Arial" size:9.0], NSFontAttributeName,
		nil];
	if ([driveNumber length] == 1)
	{ [driveNumber drawAtPoint:NSMakePoint(13,11) withAttributes:attr]; }
	else
	{ [driveNumber drawAtPoint:NSMakePoint(10.8,11) withAttributes:attr]; }		
}

@synthesize viewController;
@end
