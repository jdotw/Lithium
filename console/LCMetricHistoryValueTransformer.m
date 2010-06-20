//
//  LCMetricHistoryValueTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 20/07/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMetricHistoryValueTransformer.h"


@implementation LCMetricHistoryValueTransformer

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
	{ return [NSString stringWithFormat:@"%.2f", [value floatValue]]; }
	else if ([value floatValue] < m)
	{ return [NSString stringWithFormat:@"%.2fK", ([value floatValue] / k)]; }
	else if ([value floatValue] < g)
	{ return [NSString stringWithFormat:@"%.2fM", ([value floatValue] / m)]; }
	else if ([value floatValue] < t)
	{ return [NSString stringWithFormat:@"%.2fG", ([value floatValue] / g)]; }
	else 
	{ return [NSString stringWithFormat:@"%.2fT", ([value floatValue] / t)]; }
}

@end
