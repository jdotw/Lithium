//
//  LCCaseActiveDotTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 12/01/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCaseActiveDotTransformer.h"


@implementation LCCaseActiveDotTransformer

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
	NSImage *image = nil;
	
	if (!value) return [NSImage imageNamed:@"GreyDot.tiff"];
	
	switch ([value intValue])
	{
		case 0:
			image = [NSImage imageNamed:@"GreyDot.tiff"];
			break;
		case 1:
			image = [NSImage imageNamed:@"BlueDot.tiff"];
			break;
		default:
			image = [NSImage imageNamed:@"GreyDot.tiff"];	
	}
	
	return image;
}

@end
