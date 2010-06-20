//
//  LCTriangleBezierPath.h
//  Lithium Console
//
//  Created by James Wilson on 18/10/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
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
