//
//  LCNilIsAllTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 3/12/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCNilIsAllTransformer.h"


@implementation LCNilIsAllTransformer

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
	if (!value) return @"All";
	return value;
}

@end
