//
//  LCXSPowerView.m
//  Lithium Console
//
//  Created by James Wilson on 4/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXSPowerView.h"


@implementation LCXSPowerView

#pragma mark "Constructors"

- (LCXSPowerView *) initWithPower:(LCEntity *)initPower inFrame:(NSRect)frame 
{
    [super initWithFrame:frame];
	
	powerObject = [initPower retain];
	
    return self;
}

- (void) dealloc
{
	[powerObject release];
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
	/* Status Dot */
	NSRect statusRect = NSMakeRect(4, NSMaxY([self bounds])-10, 6, 6);
	NSBezierPath *statusPath = [NSBezierPath bezierPathWithOvalInRect:statusRect];
	[[powerObject opStateColor] setFill];
	[statusPath fill];
	[[NSColor grayColor] setStroke];
	[statusPath stroke];			

	/* Desc */
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.7], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Lucida Grande" size:8.0], NSFontAttributeName,
		nil];
	NSString *description;
	if ([[[powerObject container] name] isEqualToString:@"xsipsu"])
	{ description = [powerObject desc]; }
	else 
	{ description = @"Power Supply"; }
	[description drawAtPoint:NSMakePoint(12,NSMaxY([self bounds])-12) withAttributes:attr];	
}

@synthesize powerObject;
@end
