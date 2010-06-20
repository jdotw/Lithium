//
//  LCActivityPriorityDotTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 23/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCActivityPriorityDotTransformer.h"


@implementation LCActivityPriorityDotTransformer

+ (Class) transformedValueClass
{
	return [NSImage class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (NSImage *) transformedValue:(NSNumber *)value
{
	NSImage *image = nil;
	
	if (!value) return [NSImage imageNamed:@"GreyDot.tiff"];
	
	switch ([value intValue])
	{
		case 0:
			image = [NSImage imageNamed:@"YellowDot.tiff"];
			break;
		case -1:
			image = [NSImage imageNamed:@"GreenDot.tiff"];
			break;
		case 1:
			image = [NSImage imageNamed:@"RedDot.tiff"];
			break;
		default:
			image = [NSImage imageNamed:@"GreyDot.tiff"];	
	}
	
	return image;
}


@end
