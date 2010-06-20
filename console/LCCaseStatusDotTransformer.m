//
//  LCCaseStatusDotTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 17/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCaseStatusDotTransformer.h"
#import "LCColourDotTransformer.h"
#import "LCCase.h"

@implementation LCCaseStatusDotTransformer

+ (Class) transformedValueClass
{
	return [NSImage class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (NSImage *) transformedValue:(NSArray *)array
{
	int highestStatus = -3;
	
	if (!array || [array count] < 1) return [NSImage imageNamed:@"GreyDot.tiff"];
		
	for (LCEntity *entity in array)
	{
		
		if ([[entity opstateInteger] intValue] > highestStatus)
		{ highestStatus = [[entity opstateInteger] intValue]; }
	}
	
	LCColourDotTransformer *dotTransformer = [[[LCColourDotTransformer alloc] init] autorelease];
	
	return [dotTransformer transformedValue:[NSString stringWithFormat:@"%i", highestStatus]];
}

@end
