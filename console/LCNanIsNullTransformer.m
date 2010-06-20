//
//  LCNanIsNullTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 19/07/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCNanIsNullTransformer.h"

@implementation LCNanIsNullTransformer

+ (Class) transformedValueClass
{
	return [NSString class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (id) transformedValue:(id)valueObject
{
	float value = [valueObject floatValue];
	if ([[NSString stringWithFormat:@"%.2f", value] isEqualToString:@"nan"]) 
	{
		return @"";
	}
	return [NSString stringWithFormat:@"%f", value];
}

@end
