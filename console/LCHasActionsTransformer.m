//
//  LCHasActionsTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 24/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCHasActionsTransformer.h"
#import "LCAction.h"
#import "LCIncident.h"

@implementation LCHasActionsTransformer

+ (Class) transformedValueClass
{
	return [NSImage class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (NSImage *) transformedValue:(LCIncident *)inc
{
//	if ([inc actionCount] > 0)
//	{
//		NSDate *now = [NSDate date];
//		if ([inc hasRecurringActions])
//		{ return [NSImage imageNamed:@"tools_16.tif"]; }
//		else if ([now laterDate:[inc allActionsExecuteBy]] != now)
//		{ return [NSImage imageNamed:@"tools_16.tif"]; }
//		else
//		{ return [NSImage imageNamed:@"tools_grey_16.tif"]; }
//	}
	return nil;
}

@end
