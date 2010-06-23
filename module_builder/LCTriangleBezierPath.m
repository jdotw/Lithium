//
//  LCTriangleBezierPath.m
//  Lithium Console
//
//  Created by James Wilson on 18/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCTriangleBezierPath.h"


@implementation LCTriangleBezierPath

+ (NSBezierPath *) upTriangleInRect:(NSRect)rect
{
	NSBezierPath *path = [NSBezierPath bezierPath];
	[path moveToPoint:NSMakePoint(NSMinX(rect),NSMinY(rect))];
	[path lineToPoint:NSMakePoint(NSMaxX(rect),NSMinY(rect))];
	[path lineToPoint:NSMakePoint(NSMinX(rect)+(rect.size.width/2.0), NSMaxY(rect))];
	[path lineToPoint:NSMakePoint(NSMinX(rect),NSMinY(rect))];	
	return path;
}

+ (NSBezierPath *) downTriangleInRect:(NSRect)rect
{
	NSBezierPath *path = [NSBezierPath bezierPath];
	[path moveToPoint:NSMakePoint(NSMinX(rect),NSMaxY(rect))];
	[path lineToPoint:NSMakePoint(NSMaxX(rect),NSMaxY(rect))];
	[path lineToPoint:NSMakePoint(NSMinX(rect)+(rect.size.width/2.0), NSMinY(rect))];
	[path lineToPoint:NSMakePoint(NSMinX(rect),NSMaxY(rect))];	
	return path;
}

+ (NSBezierPath *) leftTriangleInRect:(NSRect)rect
{
	NSBezierPath *path = [NSBezierPath bezierPath];
	[path moveToPoint:NSMakePoint(NSMaxX(rect),NSMinY(rect))];
	[path lineToPoint:NSMakePoint(NSMaxX(rect),NSMaxY(rect))];
	[path lineToPoint:NSMakePoint(NSMinX(rect),NSMinY(rect)+(rect.size.height/2.0))];
	[path lineToPoint:NSMakePoint(NSMaxX(rect),NSMinY(rect))];	
	return path;
}

+ (NSBezierPath *) rightTriangleInRect:(NSRect)rect
{
	NSBezierPath *path = [NSBezierPath bezierPath];
	[path moveToPoint:NSMakePoint(NSMinX(rect),NSMinY(rect))];
	[path lineToPoint:NSMakePoint(NSMinX(rect),NSMaxY(rect))];
	[path lineToPoint:NSMakePoint(NSMaxX(rect),NSMinY(rect)+(rect.size.height/2.0))];
	[path lineToPoint:NSMakePoint(NSMinX(rect),NSMinY(rect))];	
	return path;
}



@end
