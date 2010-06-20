//
//  LCTriangleBezierPath.h
//  Lithium Console
//
//  Created by James Wilson on 18/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCTriangleBezierPath : NSObject 
{

}

+ (NSBezierPath *) upTriangleInRect:(NSRect)rect;
+ (NSBezierPath *) downTriangleInRect:(NSRect)rect;
+ (NSBezierPath *) leftTriangleInRect:(NSRect)rect;
+ (NSBezierPath *) rightTriangleInRect:(NSRect)rect;

@end
