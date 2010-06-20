//
//  LCUpperCaseTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 14/12/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCUpperCaseTransformer.h"


@implementation LCUpperCaseTransformer

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
	return [(NSString *)value uppercaseString];
}

@end
