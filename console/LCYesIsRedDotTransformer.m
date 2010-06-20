//
//  LCYesIsRedDotTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 21/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCYesIsRedDotTransformer.h"


@implementation LCYesIsRedDotTransformer

+ (Class) transformedValueClass
{
	return [NSImage class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (NSImage *) transformedValue:(id)value
{
	if ([value boolValue]) return [NSImage imageNamed:@"RedDot.tiff"];
	else return nil;
}

@end
