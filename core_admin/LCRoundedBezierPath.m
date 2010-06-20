//
//  LCRoundedBezierPath.m
//  Lithium Console
//
//  Created by James Wilson on 17/10/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import "LCRoundedBezierPath.h"


@implementation LCRoundedBezierPath

#pragma mark "Outline Path"

+ (NSBezierPath *) pathInRect:(NSRect)frame
{
	float arcWidth=15;
	float x = frame.origin.x;
	float y = frame.origin.y;
	NSBezierPath *path = [NSBezierPath bezierPath];
	
	/* Bottom line */
	[path moveToPoint:NSMakePoint(x+arcWidth,y)];
	[path lineToPoint:NSMakePoint((NSMaxX(frame)-arcWidth), y)];
	
	/* Bottom right */
	[path curveToPoint:NSMakePoint(NSMaxX(frame),y+arcWidth) controlPoint1:NSMakePoint(NSMaxX(frame),y) controlPoint2:NSMakePoint(NSMaxX(frame),y)];
	
	/* Right line */
	[path lineToPoint:NSMakePoint(NSMaxX(frame),NSMaxY(frame)-arcWidth)];
	
	/* Top right */
	[path curveToPoint:NSMakePoint(NSMaxX(frame)-arcWidth, NSMaxY(frame)-1) controlPoint1:NSMakePoint(NSMaxX(frame), NSMaxY(frame)) controlPoint2:NSMakePoint(NSMaxX(frame), NSMaxY(frame))];
	
	/* Top line */
	[path lineToPoint:NSMakePoint(x+arcWidth, NSMaxY(frame)-1)];
	
	/* Top leftt */
	[path curveToPoint:NSMakePoint(x, NSMaxY(frame)-arcWidth) controlPoint1:NSMakePoint(x, NSMaxY(frame)) controlPoint2:NSMakePoint(x, NSMaxY(frame))];
	
	/* Left line */
	[path lineToPoint:NSMakePoint(x, y+arcWidth)];
	
	/* Bottom left */
	[path curveToPoint:NSMakePoint(x+arcWidth, y) controlPoint1:NSMakePoint(x, y) controlPoint2:NSMakePoint(x, y)];
	
	return path;
}

@end
