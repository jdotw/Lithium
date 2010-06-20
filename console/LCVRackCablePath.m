//
//  LCVRackCablePath.m
//  Lithium Console
//
//  Created by James Wilson on 16/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCVRackCablePath.h"

@implementation LCVRackCablePath

- (LCVRackCablePath *) initWithStartPoint:(NSPoint)initStartPoint endPoint:(NSPoint)initEndPoint path:(NSBezierPath *)initPath cable:(LCVRackCable *)initCable
{
	[super init];
	
	startPoint = initStartPoint;
	endPoint = initEndPoint;
	path = [initPath retain];
	cable = [initCable retain];
	speedIndex = 1;
	indicatorT = 0.0;
	
	return self;
}

- (void) dealloc
{
	[path release];
	[cable release];
	[super dealloc];
}

- (NSPoint) pointOnPathAt:(float)t
{
    float   ax, bx, cx;
    float   ay, by, cy;
    float   tSquared, tCubed;
	NSPoint result;
	
    /* calculate the polynomial coefficients */
	
    cx = 3.0 * (controlPoint1.x - startPoint.x);
    bx = 3.0 * (controlPoint2.x - controlPoint1.x) - cx;
    ax = endPoint.x - startPoint.x - cx - bx;
	
    cy = 3.0 * (controlPoint1.y - startPoint.y);
    by = 3.0 * (controlPoint2.y - controlPoint1.y) - cy;
    ay = endPoint.y - startPoint.y - cy - by;
	
    /* calculate the curve point at parameter value t */
	
    tSquared = t * t;
    tCubed = tSquared * t;
	
    result.x = (ax * tCubed) + (bx * tSquared) + (cx * t) + startPoint.x;
    result.y = (ay * tCubed) + (by * tSquared) + (cy * t) + startPoint.y;
	
    return result;	
}

- (NSPoint) startPoint
{ return startPoint; }

- (NSPoint) endPoint
{ return endPoint; }

- (NSPoint) controlPoint1
{ return controlPoint1; }
- (void) setControlPoint1:(NSPoint)point
{ controlPoint1 = point; }

- (NSPoint) controlPoint2
{ return controlPoint2; }
- (void) setControlPoint2:(NSPoint)point
{ controlPoint2 = point; }

- (NSBezierPath *) path
{ return path; }
- (void) setPath:(NSBezierPath *)newPath
{ 
	if (path) [path release];
	path = [newPath retain]; 
}

- (int) speedIndex
{ return speedIndex; }
- (void) setSpeedIndex:(int)index
{ speedIndex = index; }

- (float) indicatorT
{ return indicatorT; }
- (void) setIndicatorT:(float)t
{ indicatorT = t; }
- (void) incrementIndicatorT
{ indicatorT = indicatorT + 0.02; }

- (LCVRackCable *) cable
{ return cable; }
- (void) setCable:(LCVRackCable *)newCable
{
	if (cable) [cable release];
	cable = [newCable retain];
}

- (NSColor *) cableColor
{ return cableColor; }
- (void) setCableColor:(NSColor *)color
{ 
	[cableColor release];
	cableColor = [color retain];
}

- (float) cableAlpha
{ return cableAlpha; }
- (void) setCableAlpha:(float)alpha
{ cableAlpha = alpha; }


@end
