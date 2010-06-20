//
//  LCEpochTimeTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 12/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCEpochTimeTransformer.h"


@implementation LCEpochTimeTransformer

+ (Class) transformedValueClass
{
	return [NSString class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (NSString *) transformedValue:(NSString *)value
{
	if (!value || [value floatValue] == 0.0) return nil;
	NSDate *date = [NSDate dateWithTimeIntervalSince1970:[value floatValue]];
	
	return [date description];
}

@end
