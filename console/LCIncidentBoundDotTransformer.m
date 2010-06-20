//
//  LCIncidentBoundDotTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 17/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCIncidentBoundDotTransformer.h"


@implementation LCIncidentBoundDotTransformer

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
	if (value && [value intValue] > 0) return [NSImage imageNamed:@"BlueDot.tiff"];
	
	return nil;
}


@end
