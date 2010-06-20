//
//  LCVRackCablePath.h
//  Lithium Console
//
//  Created by James Wilson on 16/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCVRackCable.h"

@interface LCVRackCablePath : NSObject 
{
	NSPoint startPoint;
	NSPoint endPoint;
	NSPoint controlPoint1;
	NSPoint controlPoint2;
	NSBezierPath *path;
	LCVRackCable *cable;
	NSColor *cableColor;
	float cableAlpha;
	int speedIndex;
	float indicatorT;
}

- (LCVRackCablePath *) initWithStartPoint:(NSPoint)initStartPoint endPoint:(NSPoint)initEndPoint path:(NSBezierPath *)initPath cable:(LCVRackCable *)initCable;

- (NSPoint) pointOnPathAt:(float)t;

- (NSPoint) startPoint;

- (NSPoint) endPoint;

- (NSPoint) controlPoint1;
- (void) setControlPoint1:(NSPoint)point;

- (NSPoint) controlPoint2;
- (void) setControlPoint2:(NSPoint)point;

- (float) indicatorT;
- (void) setIndicatorT:(float)t;

- (NSBezierPath *) path;
- (void) setPath:(NSBezierPath *)newPath;

- (LCVRackCable *) cable;
- (void) setCable:(LCVRackCable *)newCable;

- (NSColor *) cableColor;
- (void) setCableColor:(NSColor *)color;

- (float) cableAlpha;
- (void) setCableAlpha:(float)alpha;

@property (retain,getter=cableColor,setter=setCableColor:) NSColor *cableColor;
@property (getter=cableAlpha,setter=setCableAlpha:) float cableAlpha;
@property (getter=speedIndex,setter=setSpeedIndex:) int speedIndex;
@property (getter=indicatorT,setter=setIndicatorT:) float indicatorT;
@end
