//
//  LCTriggersetTrgEnabledTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 8/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCTriggersetTrgEnabledTransformer.h"


@implementation LCTriggersetTrgEnabledTransformer

+ (Class) transformedValueClass
{
	return [NSImage class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (NSImage *) transformedValue:(NSString *)value
{
	if ([value intValue] == 0) return [NSImage imageNamed:@"BlueDot.tiff"];
	else return [NSImage imageNamed:@"GreyDot.tiff"];
}

@end
