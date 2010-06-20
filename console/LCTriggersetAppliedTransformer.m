//
//  LCTriggersetAppliedTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 8/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCTriggersetAppliedTransformer.h"


@implementation LCTriggersetAppliedTransformer

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
	if ([value intValue] == 1) return [NSImage imageNamed:@"BlueDot.tiff"];
	else return [NSImage imageNamed:@"GreyDot.tiff"];
}

@end
