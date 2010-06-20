//
//  NSString-ScaledValue.m
//  Lithium Console
//
//  Created by James Wilson on 24/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "NSString-ScaledValue.h"


@implementation NSString (ScaledValue)

+ (NSString *) stringForValue:(float)value
{
	if (isnan(value) != 0) return nil;
	
	float k = 1000;
	float m = 1000000;
	float g = 1000000000;
	
	if (value < k)
	{ return [NSString stringWithFormat:@"%.2f", value]; }
	else if (value < m)
	{ return [NSString stringWithFormat:@"%.2fK", value / k]; }
	else if (value < g)
	{ return [NSString stringWithFormat:@"%.2fM", value / m]; }
	else if ((value / 1024) < g)
	{ return [NSString stringWithFormat:@"%.2fG", value / g]; }
	else
	{ return [NSString stringWithFormat:@"%.2fT", (value / g) * (1 / 1000)]; }	
}


@end
