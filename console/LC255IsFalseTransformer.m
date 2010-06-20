//
//  LC255IsFalseTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 26/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LC255IsFalseTransformer.h"


@implementation LC255IsFalseTransformer

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
	if (value && [value intValue] == 255) return [NSNumber numberWithBool:NO];
	else return [NSNumber numberWithBool:YES];
}



@end
