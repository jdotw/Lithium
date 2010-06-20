//
//  LCReportValue.m
//  Lithium Console
//
//  Created by Liam Elliott on 13/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCReportValue.h"
#import <math.h>


@implementation LCReportValue

#pragma mark "Initialisation"

-(LCReportValue *)init
{
	[super init];
	
	return self;
}

-(LCReportValue *)initWithUnits:(NSString *)newValue
{
	[self init];
	
	units = newValue;
	
	return self;
}

-(void) dealloc
{
	[units release];
	
	[super dealloc];
}
  
#pragma mark "Accessors"
-(double)value
{ return value;}

-(void)setValue:(double)newValue
{ value = newValue;}

-(NSString *)valueString
{ return [NSString stringWithFormat:@"%.2f%@",value,units];}

-(NSString *)valueVolumeString
{return [NSString stringWithFormat:@"%@%@",[self volumeString:value],units];}

-(double) valueDelta
{ return valueDelta;}

-(void) setValueDelta:(double)newValue
{ valueDelta = newValue;}

-(NSString *)valueDeltaString
{ 
	if (valueDelta < 0)
	{
		double temp = -valueDelta;
		return [NSString stringWithFormat:@"%.2-",temp];
	}
	
	return [NSString stringWithFormat:@"%.2+", percentDelta];
}

- (double) percent
{return percent;}

-(void) setPercent:(double)newValue
{ percent = newValue;}

-(NSString *)percentString
{ return [NSString stringWithFormat:@"%.2f%%",value];}

-(double) percentDelta
{ return percentDelta; }

-(void) setPercentDelta:(double)newValue
{ percentDelta = newValue;}

-(NSString *)percentDeltaString
{
	if (percentDelta < 0)
	{
		double temp = -percentDelta;
    	return [NSString stringWithFormat:@"%.2-",temp];
	}
	else
	{
		return [NSString stringWithFormat:@"%.2+",percentDelta];
	}
}

-(NSString *)units
{ return units;}

-(void) setUnits:(NSString *) newValue
{ units = newValue;}

-(NSString *)volumeString:(double) newValue
{
		if (isnan(value))
		{ return @"Nan"; }
		
		double k = 1000.0;
		double m = 1000000.0;
		double g = 1000000000.0;
		double t = 1000000000000.0;
		
		
		if (newValue == 0) return @"0.00";
		else if (newValue < k)
		{ return [NSString stringWithFormat:@"%1.2f",newValue];}
		else if (newValue < m)
		{ return [NSString stringWithFormat:@"1.2fK",(newValue / k)];}
		else if (newValue < g)
		{ return [NSString stringWithFormat:@"1.2fM",(newValue / m)];}
		else if (newValue < t)
		{ return [NSString stringWithFormat:@"1.2fG",(newValue / g)];}
		else
		{ return [NSString stringWithFormat:@"1.2fT",(newValue / t)];}
}

@end
