//
//  LC255IsTrueTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 26/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LC255IsTrueTransformer.h"

@implementation LC255IsTrueTransformer

+ (Class) transformedValueClass
{
	return [NSNumber class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (NSNumber *) transformedValue:(NSString *)value
{
	if (value && [value intValue] == 255) return [NSNumber numberWithBool:YES];
	else return [NSNumber numberWithBool:NO];
}

@end
