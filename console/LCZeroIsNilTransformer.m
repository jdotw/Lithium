//
//  LCZeroIsNilTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 15/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCZeroIsNilTransformer.h"


@implementation LCZeroIsNilTransformer

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
	if ([value intValue] == 0) return nil;
	return value;
}


@end
