//
//  LCActionHasRunTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 24/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCActionHasRunTransformer.h"


@implementation LCActionHasRunTransformer

+ (Class) transformedValueClass
{
	return [NSString class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (NSString *) transformedValue:(NSNumber *)number
{
	if ([number intValue] == 0)
	{ return @"Has not run"; }
	if ([number intValue] == 1)
	{ return @"1 time"; }
	else
	{ return [NSString stringWithFormat:@"%i times", [number intValue]]; }
}

@end
