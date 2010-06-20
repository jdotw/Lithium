//
//  LCEntityRefreshTransformer.m
//  Lithium Console
//
//  Created by James Wilson on 12/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCEntityRefreshTransformer.h"


@implementation LCEntityRefreshTransformer

+ (Class) transformedValueClass
{
	return [NSString class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (NSString *) transformedValue:(NSMutableDictionary *)properties
{
	if (!properties) return nil;
	
	/* Check type */
	if ([[properties objectForKey:@"type_num"] intValue] != 6) 
	{ 
		/* Not metric */
		return nil;
	}

	NSString *timeStamp = [properties objectForKey:@"refresh_tstamp_sec"];
	if ([timeStamp floatValue] == 0) return nil;
	NSDate *date = [NSDate dateWithTimeIntervalSince1970:[timeStamp floatValue]];
	
	return [date description];
}

@end
