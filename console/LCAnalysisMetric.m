//
//  LCAnalysisMetric.m
//  Lithium Console
//
//  Created by Liam Elliott on 29/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCAnalysisMetric.h"
#import "LCAnalysisTrigger.h"
#import "LCMetricAnalysisValue.h"
#import "LCTrigger.h"
#import "math.h"

@implementation LCAnalysisMetric

#pragma mark "Initilisation"

+ (id) analysisMetricWithMetric:(LCMetric *)initMetric
{
	return [[[LCAnalysisMetric alloc] initWithMetric:initMetric] autorelease];
}

- (id) initWithMetric:(LCMetric *)initMetric
{
	self = [self init];
	if (!self) return nil;
	
	self.metric = initMetric;
	
	for (LCTrigger *trigger in metric.children)
	{
		[triggers addObject:[LCAnalysisTrigger analysisTriggerWithEntity:trigger]];
	}
	
	entityHistory = [[LCMetricHistory historyWithMetric:initMetric] retain];
	[entityHistory addObserver:self
					forKeyPath:@"refreshInProgress" 
					   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
					   context:nil];
		
	return self;
}

- (id) init
{
	self = [super init];
	if (!self) return nil;
	
	minValues = [[NSMutableArray array] retain];
	avgValues = [[NSMutableArray array] retain];
	maxValues = [[NSMutableArray array] retain];
	
	triggers = [[NSMutableArray array] retain];
	
	return self;
}

- (void) dealloc
{
	[entityHistory removeObserver:self forKeyPath:@"refreshInProgress"];
	[entityHistory release];
	
	[minValues release];
	[avgValues release];
	[maxValues release];
	
	[triggers release];
	[metric release];
	
	[referenceDate release];
	
	[super dealloc];
}

#pragma mark "Accessors"

@synthesize metric;
-(id)entity
{ return metric; }
@synthesize refreshInProgress;
@synthesize triggers;
@synthesize referenceDate;
@synthesize datePeriod;

#pragma mark "Populate and Process Matrices"
-(void) populateAMatrixWithValues:(NSArray *)values
{
	int rows = [values count];
	
	Amatrix = [LCGeneralMatrix matrixWithRowSize:rows andColSize:2];
	int i=0;
	
	for (LCMetricAnalysisValue *value in values)
	{
		[Amatrix setElementAtRow:i andCol:0 toValue:[value timestamp]];
		[Amatrix setElementAtRow:i andCol:1 toValue:1];
		i++;
	}
}

-(void) populateMatrix:(LCGeneralMatrix *)matrix withValues:(NSArray *)values
{
	int i=0;
	LCMetricAnalysisValue *val;
	
	for (val in values)
	{
		if (!isnan([val value]))
		{
			[matrix setElementAtRow:i andCol:0 toValue:[val value]];
			i++;
		}
	}
}

-(LCGeneralMatrix *) calculateResultsFor:(LCGeneralMatrix *)Y andMatrix:(LCGeneralMatrix *)A
{
	//Calculating the Line of best fit
	//			y= mx + b
	// where y= value, m = gradient, x = timestamp, b = y-intercept
	//
	//Using the formula: X=((At * A) ^ -1) * At * Y
	//Where: X = result matrix. First value = gradient, second value = y-intercept
	//		 A = Amatrix
	//		 Y = Ymatrix
	//		 At= transpose of Amatrix
	//
	//	Trust me, it works according to the textbook...
	
	
	LCGeneralMatrix *AtY = [[A transpose] multiply:Y];
	LCGeneralMatrix *AtA = [[A transpose] multiply:A];
	
	LCGeneralMatrix *invA = [AtA inverse];
	LCGeneralMatrix *Result = [invA multiply:AtY];

	return Result;
}

-(void)processMetric
{
	[self processMetricForPeriod:datePeriod atDate:referenceDate];
}
		

-(void)processMetricForPeriod:(int)period atDate:(NSDate *)date
{
	self.refreshInProgress = YES;
	entityHistory.graphPeriod = period;
	entityHistory.referenceDate = date;
}


-(void)processMetricForRows:(NSMutableArray *)values
{
	if ([values count] < 1) return;
	
	[minValues removeAllObjects];
	[avgValues removeAllObjects];
	[maxValues removeAllObjects];
	
	for (LCMetricHistoryRow *row in values)
	{
		NSNumber *ts = [NSNumber numberWithDouble:[[row timeStamp] timeIntervalSince1970]];
		
		if (!isnan([row minimum]))
		{
			[minValues addObject:[LCMetricAnalysisValue valueWithTimestamp:[ts doubleValue]
																 andValue:[[row minimumNumber] doubleValue]]];
			[avgValues addObject:[LCMetricAnalysisValue valueWithTimestamp:[ts doubleValue]
																  andValue:[[row averageNumber] doubleValue]]];
			[maxValues addObject:[LCMetricAnalysisValue valueWithTimestamp:[ts doubleValue]
																  andValue:[[row maximumNumber] doubleValue]]];
		}
	}
	
	int rows = [minValues count];
	
	Ymin = [LCGeneralMatrix matrixWithRowSize:rows andColSize:1];
	Yavg = [LCGeneralMatrix matrixWithRowSize:rows andColSize:1];
	Ymax = [LCGeneralMatrix matrixWithRowSize:rows andColSize:1];
	
	[self populateAMatrixWithValues:minValues];
	
	[self populateMatrix:Ymin withValues:minValues];
	[self populateMatrix:Yavg withValues:avgValues];
	[self populateMatrix:Ymax withValues:maxValues];
	
	for (LCAnalysisTrigger *trigger in triggers)
	{
		LCGeneralMatrix *result;
		double minGradient;
		double minIntercept;
		double avgGradient;
		double avgIntercept;
		double maxGradient;
		double maxIntercept;
		
		result = [self calculateResultsFor:Ymin andMatrix:Amatrix];
		minGradient = [result getElementForRow:0 andCol:0];
		minIntercept = [result getElementForRow:1 andCol:0];
		
		result = [self calculateResultsFor:Yavg andMatrix:Amatrix];
		avgGradient = [result getElementForRow:0 andCol:0];
		avgIntercept = [result getElementForRow:1 andCol:0];
		
		result = [self calculateResultsFor:Ymax andMatrix:Amatrix];
		maxGradient = [result getElementForRow:0 andCol:0];
		maxIntercept = [result getElementForRow:1 andCol:0];
		
		double q = 0.0;
		if (minGradient != 0)
		{
			switch ([[[[trigger entity] properties] valueForKey:@"trg_type_num"] intValue])
			{
				case 1: q = ([trigger xTriggerValue] - minIntercept) / minGradient;
					break;
				case 2: q = (([trigger xTriggerValue] - 1) - minIntercept) / minGradient;
					break;
				case 3: q = (([trigger xTriggerValue] + 1) - minIntercept)/minGradient;
					break;
				case 5: q = (([trigger xTriggerValue] + 1) - minIntercept) / minGradient;
					break;
			}
			[trigger setMinDate:[NSDate dateWithTimeIntervalSince1970:q]];
		}	
		else // (minGradient == 0) return a  > 5 years in future date to ensure a positive result
		{
			[trigger setMinDate:[NSDate dateWithTimeIntervalSinceNow:157690000]];		
		}
		
		q = 0.0;
		if (avgGradient != 0)
		{
			switch ([[[[trigger entity] properties] valueForKey:@"trg_type_num"] intValue])
			{
				case 1: q = ([trigger xTriggerValue] - avgIntercept) / avgGradient;
					break;
				case 2: q = (([trigger xTriggerValue] - 1) - avgIntercept) / avgGradient;
					break;
				case 3: q = (([trigger xTriggerValue] + 1) - avgIntercept) / avgGradient;
					break;
				case 5: q = (([trigger xTriggerValue] + 1) - avgIntercept) / avgGradient;
					break;
			}
			[trigger setAvgDate:[NSDate dateWithTimeIntervalSince1970:q]];
		}	
		else // (minGradient == 0) return a  > 5 years in future date to ensure a positive result
		{
			[trigger setAvgDate:[NSDate dateWithTimeIntervalSinceNow:157690000]];
		}
		
		q = 0.0;
		if (maxGradient != 0)
		{
			switch ([[[[trigger entity] properties] valueForKey:@"trg_type_num"] intValue])
			{
				case 1: q = ([trigger xTriggerValue] - maxIntercept) / maxGradient;
					break;
				case 2: q = (([trigger xTriggerValue] - 1) - maxIntercept) / maxGradient;
					break;
				case 3: q = (([trigger xTriggerValue] + 1) - maxIntercept) / maxGradient;
					break;
				case 5: q = (([trigger xTriggerValue] + 1) - maxIntercept) / maxGradient;
					break;
			}
			[trigger setMaxDate:[NSDate dateWithTimeIntervalSince1970:q]];
		}	
		else // (minGradient == 0) return a  > 5 years in future date to ensure a positive result
		{
			[trigger setMaxDate:[NSDate dateWithTimeIntervalSinceNow:157690000]];
		}
	}
	
	self.refreshInProgress = NO;

}

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	LCMetricHistory *tempObj = object;
	if ([tempObj refreshInProgress] ==  NO)
	{
		[self processMetricForRows:[entityHistory rows]];
	}
}

#pragma mark "Placeholder Properties for Tree"

-(BOOL)isLeaf
{ return NO; }

-(NSDate *) minDate
{ return nil;}

-(NSDate *)avgDate
{return nil;}

-(NSDate *)maxDate
{return nil;}

@end
