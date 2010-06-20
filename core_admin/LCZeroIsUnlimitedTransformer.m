//
//  LCZeroIsUnlimitedTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 23/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCZeroIsUnlimitedTransformer.h"


@implementation LCZeroIsUnlimitedTransformer

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
	if ([value intValue] == 0) return @"Unlimited";
	else return value;
}

@end
