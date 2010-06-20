//
//  LCEntityTypeTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 12/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCEntityTypeTransformer.h"


@implementation LCEntityTypeTransformer

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
	if (!value) return @"?";
	
	switch ([value intValue])
	{
		case 0:
			return @"?";
		case 1:
			return @"C";
		case 2:
			return @"S";
		case 3:
			return @"D";
		case 4:
			return @"C";
		case 5:
			return @"O";
		case 6:
			return @"M";
		case 7:
			return @"T";
	}
	
	return @"?";
}

@end
