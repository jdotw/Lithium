//
//  LCColourDotTransformer2.m
//  Lithium Console
//
//  Created by James Wilson on 20/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCColourDotTransformer2.h"


@implementation LCColourDotTransformer2

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
	switch ([value intValue])
	{
		case -3: 
			return [NSImage imageNamed:@"NoDot.tiff"];	
		case -2: 
			return [NSImage imageNamed:@"BlueDot.tiff"];
		case 0:
			return [NSImage imageNamed:@"GreenDot.tiff"];
		case 1:
			return [NSImage imageNamed:@"YellowDot.tiff"];
		case 2:
			return [NSImage imageNamed:@"YellowDot.tiff"];
		case 3:
			return [NSImage imageNamed:@"RedDot.tiff"];
		default:
			return [NSImage imageNamed:@"GreyDot.tiff"];	
	}
}

@end
