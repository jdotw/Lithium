//
//  LCTriggersetNullIsAll.m
//  Lithium Console
//
//  Created by James Wilson on 8/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCTriggersetNullIsAll.h"


@implementation LCTriggersetNullIsAll

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
	if (value) return value;
	else return @"*ALL*";
}

@end
