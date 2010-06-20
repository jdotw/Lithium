//
//  LCMetricAnalysisValue.m
//  Lithium Console
//
//  Created by Liam Elliott on 31/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMetricAnalysisValue.h"

@implementation LCMetricAnalysisValue

+(LCMetricAnalysisValue *) valueWithTimestamp:(double)ts andValue:(double)val
{
	LCMetricAnalysisValue *temp = [[[LCMetricAnalysisValue alloc] init]autorelease];
	
	[temp setTimestamp:ts];
	[temp setValue:val];
	
	return temp;
}

-(LCMetricAnalysisValue *) init
{
	[super init];
	
	return self;
}

-(void) dealloc
{
	[super dealloc];
}

-(double) timestamp
{ return timestamp;}

-(void) setTimestamp:(double)val
{timestamp = val;}

-(double)value
{return value;}

-(void)setValue:(double)val
{value= val;}
@end
