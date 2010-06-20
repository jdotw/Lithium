//
//  LCActivityPriorityTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 23/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCActivityPriorityTransformer.h"


@implementation LCActivityPriorityTransformer

+ (Class) transformedValueClass
{
	return [NSImage class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (NSString *) transformedValue:(NSNumber *)value
{
	NSString *string = nil;
	
	if (!value) return @"Unknown";
	
	switch ([value intValue])
	{
		case 0:
			string = @"Normal";
			break;
		case -1:
			string = @"Low";
			break;
		case 1:
			string = @"High";
			break;
		default:
			string = @"Unknown";	
	}
	
	return string;
}

@end
