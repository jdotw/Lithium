//
//  LCWAPViewClient.h
//  Lithium Console
//
//  Created by James Wilson on 12/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCObject.h"
//#import "LCWAPViewLinkPath.h"

@interface LCWAPViewClient : NSObject 
{
	LCObject *object;
	float lastSignalStrength;		/* Strength this was last drawn for */
	NSRect clientRect;				/* The rect the client dot was drawn in */
	NSBezierPath *clientPath;		/* Path of the client dot */
	float distance;					/* Calculated 'distance' */
	
	/* Connector path */
	NSBezierPath *connectorPath;	/* Path back to center */
	NSPoint startPoint;
	NSPoint endPoint;
	NSPoint controlPoint1;
	NSPoint controlPoint2;
	int speedIndex;
	float speedIndicatorT;
}

#pragma mark "Constructor"
+ (id) clientWithObject:(LCObject *)initObject;
- (id) initWithObject:(LCObject *)initObject;

#pragma mark "Connector Path"
- (NSBezierPath *) connectorPath;
- (void) setConnectorPath:(NSBezierPath *)value;
- (NSBezierPath *) connectorPath;
- (void) setConnectorPath:(NSBezierPath *)value;
- (NSPoint) startPoint;
- (void) setStartPoint:(NSPoint)point;
- (NSPoint) endPoint;
- (void) setEndPoint:(NSPoint)point;
- (NSPoint) controlPoint1;
- (void) setControlPoint1:(NSPoint)point;
- (NSPoint) controlPoint2;
- (void) setControlPoint2:(NSPoint)point;
- (int) speedIndex;
- (void) setSpeedIndex:(int)index;
- (float) speedIndicatorT;
- (void) setSpeedIndicatorT:(float)t;
- (void) incrementSpeedIndicatorT;
- (NSPoint) pointOnConnectorPathAt:(float)t;

#pragma mark "Accessors"
- (LCObject *) object;
- (void) setObject:(LCObject *)value;
- (NSRect) clientRect;
- (void) setClientRect:(NSRect)value;
- (float) lastSignalStrength;
- (void) setLastSignalStrength:(float)value;
- (NSBezierPath *) clientPath;
- (void) setClientPath:(NSBezierPath *)value;
- (float) distance;
- (void) setDistance:(float)value;

@property (retain,getter=object,setter=setObject:) LCObject *object;
@property (getter=lastSignalStrength,setter=setLastSignalStrength:) float lastSignalStrength;
@property (retain,getter=clientPath,setter=setClientPath:) NSBezierPath *clientPath;
@property (getter=distance,setter=setDistance:) float distance;
@property (retain,getter=connectorPath,setter=setConnectorPath:) NSBezierPath *connectorPath;
@property (getter=speedIndex,setter=setSpeedIndex:) int speedIndex;
@property (getter=speedIndicatorT,setter=setSpeedIndicatorT:) float speedIndicatorT;
@end

