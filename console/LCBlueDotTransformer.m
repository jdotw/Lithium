//
//  LCBlueDotTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 27/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBlueDotTransformer.h"


@implementation LCBlueDotTransformer

+ (Class) transformedValueClass
{
	return [NSImage class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (NSNumber *) transformedValue:(NSString *)value
{
	if (value && [value intValue] == 1) return [NSImage imageNamed:@"BlueDot.tiff"];
	else return [NSImage imageNamed:@"GreyDot.tiff"];
}

@end
