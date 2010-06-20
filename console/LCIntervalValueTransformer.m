//
//  LCIntervalValueTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 15/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCIntervalValueTransformer.h"


@implementation LCIntervalValueTransformer

+ (Class) transformedValueClass
{
	return [NSString class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (id) transformedValue:(id)value
{
	time_t interval;
	unsigned int days;
	unsigned int hours;
	unsigned int minutes;
	
	if (!value) return NULL; 
	
	interval = [value intValue];

	if (interval >= 60)
	{
		days = interval / (60 * 60 * 24);
		interval %= (60 * 60 * 24);

		hours = interval / (60 * 60);
		interval %= (60 * 60);

		minutes = interval / 60;

		return [NSString stringWithFormat:@"%id %ih %im", days, hours, minutes];	
	}
	else
	{
		return [NSString stringWithFormat:@"%li sec", interval];
	}
}

@end
