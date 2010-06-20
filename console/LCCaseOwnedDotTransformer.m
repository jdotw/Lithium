//
//  LCCaseOwnedDotTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 17/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCaseOwnedDotTransformer.h"
#import "LCCase.h"
#import "LCAuthenticator.h"

@implementation LCCaseOwnedDotTransformer

+ (Class) transformedValueClass
{
	return [NSImage class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (NSImage *) transformedValue:(LCCase *)value
{
	if (value && [value customer] && [value owner])
	{
		LCAuthenticator *auth = [LCAuthenticator authForCustomer:[value customer]];
		if ([[value owner] isEqualToString:[auth username]])
		{ 
			NSImage *blueDot = [NSImage imageNamed:@"BlueDotFlipped.tiff"];
			return blueDot; 
		}
	}
	
	return [NSImage imageNamed:@"GreyDot.tiff"];
}

@end
