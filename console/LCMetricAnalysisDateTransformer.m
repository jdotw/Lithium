//
//  LCMetricAnalysisDateTransformer.m
//  Lithium Console
//
//  Created by Liam Elliott on 2/11/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMetricAnalysisDateTransformer.h"


@implementation LCMetricAnalysisDateTransformer

+(Class) transformedValueClass
{
	return [NSString class];
}

+(BOOL) allowsReverseTransformation
{
	return NO;
}

-(NSString *) transformedValue:(NSDate *)value
{	
	NSTimeInterval ti = [value timeIntervalSinceNow];

	//5 years == 15768000 secs (Not including leap years)
	if (ti > 157680000)
	{
		return @"> 5 years";
	}
	else if (ti  < 0)
	{
		return @"Past";
	}
	
	return [value descriptionWithCalendarFormat:@"%d %B %Y" timeZone:nil locale:nil];
		
}
@end
