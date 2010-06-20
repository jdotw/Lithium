//
//  LCXSAirFlowView.m
//  Lithium Console
//
//  Created by James Wilson on 19/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXSAirFlowView.h"


@implementation LCXSAirFlowView

#pragma mark "Constructors"

- (LCXSAirFlowView *) initWithDevice:(LCEntity *)initDevice inFrame:(NSRect)frame 
{
    [super initWithFrame:frame];	
	device = [initDevice retain];
    return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark "View Management"

- (void) removeFromSuperview
{
	[super removeFromSuperview];
}

#pragma mark "Geometry"

- (float) inlet1MinY
{ return NSMaxY([self bounds])-105; }

- (float) inlet1MaxY
{ return NSMaxY([self bounds])-90; }

- (float) inlet2MinY
{ return NSMinY([self bounds])+80; }

- (float) inlet2MaxY 
{ return NSMinY([self bounds])+95; }

- (float) exitMaxY
{ return NSMaxY([self bounds])-100; }

- (float) exitMinY
{ return NSMinY([self bounds])+10; }

- (NSPoint) blowerPoint:(int)index
{
	return NSMakePoint(NSMinX([self bounds])+162,NSMinY([self bounds])+(index*39)+18);
}

#pragma mark "Drawing"

- (void) drawRect:(NSRect)rect
{
	/* Disabled */
	return;
	
	
	[[NSColor colorWithDeviceWhite:1.0 alpha:0.2] setStroke];
	
	/* Fan Control Points */
	NSBezierPath *testPath;
	int i;
	for (i=0; i < 7; i++)
	{
		/* Blower 1 Point */
		[[NSColor redColor] setFill];
		NSPoint testPoint = NSMakePoint(NSMinX([self bounds])+162,NSMinY([self bounds])+30+(i*33));
		testPath = [NSBezierPath bezierPathWithOvalInRect:NSMakeRect(testPoint.x,testPoint.y,5,5)];
		[testPath fill];
	}

	/* CPU 1 */
	[[NSColor redColor] setFill];
	NSPoint testPoint = NSMakePoint(NSMinX([self bounds])+240,NSMinY([self bounds])+63);
	testPath = [NSBezierPath bezierPathWithOvalInRect:NSMakeRect(testPoint.x,testPoint.y,5,5)];
	[testPath fill];

	/* CPU 1 */
	[[NSColor redColor] setFill];
	testPoint = NSMakePoint(NSMinX([self bounds])+240,NSMaxY([self bounds])-63);
	testPath = [NSBezierPath bezierPathWithOvalInRect:NSMakeRect(testPoint.x,testPoint.y,5,5)];
	[testPath fill];
	
	/*
	 * Inlet 1 -- Upper inlet
	 */
	
	for (i=0; i < 35; i++)
	{
		float inletHeight = [self inlet1MaxY] - [self inlet1MinY];
		float inletSpacing = inletHeight / 35.0;
		float exitHeight = [self exitMaxY] - [self exitMinY];
		float exitSpacing = exitHeight / 70.0;
		NSBezierPath *flowPath = [NSBezierPath bezierPath];
		NSPoint cp1 = NSZeroPoint;

		/* Straight
		if (i >= 0 && i < 20)
		{ cp1 = [self blowerPoint:3]; }
		if (i >= 20 && i < 40)
		{ cp1 = [self blowerPoint:4]; }
		if (i >= 40 && i < 60)
		{ cp1 = [self blowerPoint:5]; }
		if (i >= 60 && i < 70)
		{ cp1 = [self blowerPoint:3]; }
		*/
		
		/* Fancy */
		switch (i % 7)
		{
			case 0:
				cp1 = [self blowerPoint:3];				
				break;
			case 1:
			case 2:
				cp1 = [self blowerPoint:4];
				break;
			case 3:
			case 4:
				cp1 = [self blowerPoint:5];
				break;
			case 5:
			case 6:
				cp1 = [self blowerPoint:6];
				break;
		}
		
		[flowPath moveToPoint:NSMakePoint(NSMinX([self bounds]), [self inlet1MinY]+(inletSpacing*i))];		
		[flowPath curveToPoint:NSMakePoint(NSMaxX([self bounds]), [self exitMinY]+(exitSpacing*i)+(exitHeight * 0.5))
				 controlPoint1:cp1
				 controlPoint2:cp1];
		[flowPath stroke];
	}

	/*
	 * Inlet 2 -- Upper inlet
	 */
	
	for (i=0; i < 35; i++)
	{
		float inletHeight = [self inlet2MaxY] - [self inlet2MinY];
		float inletSpacing = inletHeight / 35.0;
		float exitHeight = [self exitMaxY] - [self exitMinY];
		float exitSpacing = exitHeight / 70.0;
		NSBezierPath *flowPath = [NSBezierPath bezierPath];
		NSPoint cp1 = NSZeroPoint;
		
		/* Fancy */
		switch (i % 7)
		{
			case 0:
			case 1:
				cp1 = [self blowerPoint:0];				
				break;
			case 2:
			case 3:
				cp1 = [self blowerPoint:1];
				break;
			case 4:
			case 5:
				cp1 = [self blowerPoint:2];
				break;
			case 6:
				cp1 = [self blowerPoint:3];
				break;
		}
		
		[flowPath moveToPoint:NSMakePoint(NSMinX([self bounds]), [self inlet2MinY]+(inletSpacing*i))];		
		[flowPath curveToPoint:NSMakePoint(NSMaxX([self bounds]), [self exitMinY]+(exitSpacing*i))
				 controlPoint1:cp1
				 controlPoint2:cp1];
		[flowPath stroke];		
	}
	
	
}


@synthesize device;
@end
