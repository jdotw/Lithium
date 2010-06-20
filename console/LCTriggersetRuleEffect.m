//
//  LCTriggersetRuleEffect.m
//  Lithium Console
//
//  Created by James Wilson on 8/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCTriggersetRuleEffect.h"


@implementation LCTriggersetRuleEffect

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
	if ([value intValue] == 1) return @"Apply Trigger Set";
	else return @"Do NOT Apply Trigger Set";
}

@end
