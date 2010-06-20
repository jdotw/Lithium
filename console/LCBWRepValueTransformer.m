//
//  LCBWRepValueTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 30/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBWRepValueTransformer.h"


@implementation LCBWRepValueTransformer

+ (Class) transformedValueClass
{
	return [NSString class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (id) transformedValue:(NSNumber *)value
{
	/* Check value */
	if (!value) return nil;
	
	/* Check for nan */
	if (isnan([value floatValue]))
	{ return @"nan"; }
	
	float k = 1000.00f;
	float m = 1000000.00f;
	float g = 1000000000.00f;
	float t = 1000000000000.00f;
	
	/* Value checking */
	if ([value floatValue] == 0) return 0;
	else if ([value floatValue] < k)
	{ return [NSString stringWithFormat:@"%.2fbit", [value floatValue]]; }
	else if ([value floatValue] < m)
	{ return [NSString stringWithFormat:@"%.2fKbit", ([value floatValue] / k)]; }
	else if ([value floatValue] < g)
	{ return [NSString stringWithFormat:@"%.2fMbit", ([value floatValue] / m)]; }
	else if ([value floatValue] < t)
	{ return [NSString stringWithFormat:@"%.2fGbit", ([value floatValue] / g)]; }
	else 
	{ return [NSString stringWithFormat:@"%.2fTbit", ([value floatValue] / t)]; }
}

@end
