//
//  LCReportMetricHistory.m
//  Lithium Console
//
//  Created by Liam Elliott on 16/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCReportMetricHistory.h"
#import <math.h>
#import "LCAnalysisMetric.h"
#import "LCAnalysisTrigger.h"

@implementation LCReportMetricHistory

#pragma mark "Initialisation"
+(LCReportMetricHistory *)reportMetricHistoryWithMetric:(LCEntity *)entity 
									  withReferenceDate:(NSDate *)date 
										  andDataPeriod:(int)period
{
	LCReportMetricHistory *temp = [[[LCReportMetricHistory alloc] initWithMetric:entity]autorelease];

	[temp setReferenceDate:date];
	[temp setGraphPeriod:period];
	
	return temp;
}

-(LCReportMetricHistory *)init
{
	[super init];

	return self;
}
-(LCReportMetricHistory *)initWithMetric:(LCEntity *)initMetric
{
	[super initWithMetric:initMetric];
	
	return self;
}

-(void) dealloc
{
	[super dealloc];
}


#pragma mark "Refresh Methods"
-(void)completeMetricHistoryRefresh
{
	float totalAvg = 0.0F;
	
	int countAvg = 0;
	
	LCMetricHistoryRow *row;
	
	for (row in rows)
	{
		//Store the Minimum of the minimums
		if (!isnan([row minimum]))
		{
			if ([row minimum] < minimum)
				minimum = [row minimum];
		}
		
		//Store the Average of the averages
		if (!isnan([row average]))
		{
			countAvg++;
			totalAvg += [row average];
		}
		
		if (!isnan([row maximum]))
		{
			if ([row maximum] > maximum)
				maximum = [row maximum];
		}
	}
	if (countAvg > 0)
		average = totalAvg / countAvg;
	[self setRefreshComplete:YES];
}

-(NSDate *)findCriticalDate
{
	LCAnalysisMetric *item = [LCAnalysisMetric analysisMetricWithMetric:metric];
	
	[item processMetricForRows:rows];
	
	NSDate *criticalDate;
	
	NSEnumerator *triggerEnumerator = [[item triggers] objectEnumerator];
	LCAnalysisTrigger *trigger;
	
	while (trigger = [triggerEnumerator nextObject])
	{
		NSDate *tempDate = [trigger maxCriticalDate];
		
		criticalDate = [tempDate earlierDate:criticalDate];
	}
	
	return criticalDate;
}
		

#pragma mark "Accessors"
-(BOOL)refreshComplete
{return refreshComplete;}
-(void)setRefreshComplete:(BOOL)value
{ refreshComplete = value;}

-(float) minimum
{return minimum;}
-(NSString *)minimumString;
{return [self volumeString:minimum];}

-(float) average
{return average;}
-(NSString *)averageString
{return [self volumeString:average];}

-(float) maximum
{return maximum;}

-(NSString *)maximumString
{return [self volumeString:maximum];}

#pragma mark "Misc"

-(NSString *)volumeString:(double) newValue
{
	if (isnan(newValue))
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
