//
//  LCWAPViewClient.m
//  Lithium Console
//
//  Created by James Wilson on 12/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCWAPViewClient.h"


@implementation LCWAPViewClient

#pragma mark "Constructor"

+ (id) clientWithObject:(LCObject *)initObject
{
	return [[[LCWAPViewClient alloc] initWithObject:initObject] autorelease];
}

- (id) initWithObject:(LCObject *)initObject
{
	[super init];
	
	[self setObject:initObject];
	
	return self;
}

- (void) dealloc
{
	[object release];
	[super dealloc];
}

#pragma mark "Connector Path"

- (NSBezierPath *) connectorPath
{ return connectorPath; }

- (void) setConnectorPath:(NSBezierPath *)value
{ 
	[connectorPath release];
	connectorPath = [value retain];
}

- (NSPoint) startPoint
{ return startPoint; }

- (void) setStartPoint:(NSPoint)point
{ startPoint = point; }

- (NSPoint) endPoint
{ return endPoint; }

- (void) setEndPoint:(NSPoint)point
{ endPoint = point; }

- (NSPoint) controlPoint1
{ return controlPoint1; }

- (void) setControlPoint1:(NSPoint)point
{ controlPoint1 = point; }

- (NSPoint) controlPoint2
{ return controlPoint2; }

- (void) setControlPoint2:(NSPoint)point
{ controlPoint2 = point; }

- (int) speedIndex
{ return speedIndex; }
- (void) setSpeedIndex:(int)index
{ speedIndex = index; }

- (float) speedIndicatorT
{ return speedIndicatorT; }
- (void) setSpeedIndicatorT:(float)t
{ speedIndicatorT = t; }

- (void) incrementSpeedIndicatorT
{ 
	speedIndicatorT = speedIndicatorT + ([[object valueForMetricNamed:@"rate"] floatValue] / 30000.0);
	if (speedIndicatorT >= 1) speedIndicatorT = 0.0;
}

- (NSPoint) pointOnConnectorPathAt:(float)t
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

#pragma mark "Accessors"

- (LCObject *) object
{ return object; }

- (void) setObject:(LCObject *)value
{
	[object release];
	object = [value retain];
}

- (NSRect) clientRect
{ return clientRect; }

- (void) setClientRect:(NSRect)value
{ clientRect = value; }

- (float) lastSignalStrength
{ return lastSignalStrength; }

- (void) setLastSignalStrength:(float)value
{ lastSignalStrength = value; }

- (NSBezierPath *) clientPath
{ return clientPath; }

- (void) setClientPath:(NSBezierPath *)value
{ 
	[clientPath release];
	clientPath = [value retain];
}

- (float) distance
{ return distance; }

- (void) setDistance:(float)value
{ distance = value; }


@end
