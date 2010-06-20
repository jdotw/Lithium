//
//  LCRoundedBezierPath.m
//  Lithium Console
//
//  Created by James Wilson on 17/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCRoundedBezierPath.h"


@implementation LCRoundedBezierPath

#pragma mark "Outline Path"

+ (NSBezierPath *) pathInRect:(NSRect)frame
{
	return [NSBezierPath bezierPathWithRoundedRect:frame xRadius:5.0 yRadius:5.0];
}

@end
