//
//  LCReportInterface.m
//  Lithium Console
//
//  Created by Liam Elliott on 18/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCReportInterface.h"
#import "LCReportMetricHistory.h"


@implementation LCReportInterface

#pragma mark "Initialisation"
+(LCReportInterface *) reportInterfaceWithEntity:(LCEntity *)entity 
									   andParent:(LCReportDevice *) parent
{
	LCReportInterface *temp = [[[LCReportInterface alloc]init] autorelease];
		
	[temp setParent:parent];
	return temp;
}
	
	
+(LCReportInterface *) reportInterfaceWithEntity:(LCEntity *)entity 
							   reportDescription:(NSString *)description
									   andParent:(LCReportDevice *) parent
{
	LCReportInterface *temp = [[[LCReportInterface alloc] init] autorelease];
	[temp setParent:parent];
	[temp setReportDescription];
	
	return temp;
}

-(LCReportInterface *) init
{
	[super init];
	
	outstandingRefreshes = [NSMutableArray array];
	
	metricRefreshCompleted = NO;
	dataCollectionCompleted = NO;
	deltaMetricRefreshCompleted = NO;
	baseEntityRefreshCompleted = NO;
	capacityPlanningRefreshCompleted = NO;
	
	return self;
}

-(void) dealloc
{
	[outstandingRefreshes release];
		
	[super dealloc];
}

#pragma mark "Accessors"
-(LCReportEntity *) inputEntity
{ return inputEntity; }
-(LCReportEntity *) outputEntity
{ return outputEntity;}

-(NSString *)reportDescription
{return reportDescription;}
-(void)setReportDesctiption:(NSString *)value
{ reportDescription = value;}

-(NSString *) entityName
{ return [[baseEntity object] description];}
-(LCEntity *) baseEntity
{ return baseEntity;}

-(NSNumber *) interfaceSpeed
{ return interfaceSpeed;}
-(void) setInterfaceSpeed:(NSNumber *)value
{ interfaceSpeed = value;}
-(float) interfaceSpeedAsFloat
{ return [interfaceSpeed floatValue];}

-(NSDate *) criticalDate
{ return criticalDate;}

-(LCReportDevice *)parent
{ return parent;}
-(void)setParent:(LCReportDevice *) value
{ parent = value;}

-(BOOL)calcDeltaValues
{ return calcDeltaValues;}
-(void)setCalcDeltaValues:(BOOL)flag
{ calcDeltaValues = flag;}

-(NSDate *)referenceDate
{return referenceDate;}
-(void) setReferenceDate:(NSDate *)value
{referenceDate = value;}

-(int)dataPeriod
{return dataPeriod;}
-(void) setDataPeriod:(int) value
{dataPeriod = value;}

#pragma mark "Data Collection"
-(void) collectReportDataForDate:(NSDate *)newDate 
				  withDataPeriod:(int)newPeriod
		 andCalculateDeltaValues:(BOOL)newDelta
{
	inputEntity = [LCReportEntity reportEntityWithEntity:[[[baseEntity  object] childrenDictionary] valueForKey:@"bps_in"] 
											   andParent:parent];
	[inputEntity setUnits:@"bits/s"];
	
	outputEntity = [LCReportEntity reportEntityWithEntity:[[[baseEntity object] childrenDictionary] valueForKey:@"bps_out"]
												andParent:parent];
	[outputEntity setUnits:@"bits/s"];

	NSNumber *temp = [NSNumber numberWithFloat:[[[[baseEntity object] childrenDictionary] valueForKey:@"speed"] floatValue]];
		
	[self setInterfaceSpeed:temp];
	
	[self setReferenceDate: newDate];
	[self setDataPeriod: newPeriod];
	[self setCalcDeltaValues:newDelta];
	
	LCReportMetricHistory *inputMetric = [LCReportMetricHistory reportMetricHistoryWithEntity:[inputEntity baseEntity]
																		   withReferenceDate:referenceDate 
																			   andDataPeriod:dataPeriod];
	[inputMetric addObserver:self 
					forKeyPath:@"refreshComplete" 
					   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
					   context:nil];

	[inputMetric refresh:XMLREQ_PRIO_NORMAL];

	[outstandingRefreshes addObject:inputMetric];
	
	LCReportMetricHistory *outputMetric = [LCReportMetricHistory reportMetricHistoryWithEntity:[outputEntity baseEntity]
																			withReferenceDate:referenceDate andDataPeriod:dataPeriod];
	[outputMetric addObserver:self 
				  forKeyPath:@"refreshComplete" 
					 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
					  context:nil];
	[outstandingRefreshes addObject:outputMetric];
	
	[outputMetric refresh:XMLREQ_PRIO_NORMAL];
}

-(void)reportMetricHistoryCompleted:(id)object
{
	LCReportMetricHistory *data = object;
	NSString *temp = [[data metric] name];
	NSString *tempStr = [NSString stringWithString:@"in"];

	NSRange result = [temp rangeOfString:tempStr];
	if ( result.location != NSNotFound)
	{
		[[inputEntity minimum] setValue:[data minimum]];				
		[[inputEntity minimum] setPercent:([data minimum] / [interfaceSpeed floatValue]) * 100];
		
		[[inputEntity average] setValue:[data average]];
		[[inputEntity average] setPercent:([data average] / [interfaceSpeed floatValue]) * 100];
		
		[[inputEntity maximum] setValue:[data maximum]];
		[[inputEntity maximum] setPercent:([data maximum] / [interfaceSpeed floatValue]) * 100];
	}
	else
	{
		[[outputEntity minimum] setValue:[data minimum]];
		[[outputEntity minimum] setPercent:([data minimum] / [interfaceSpeed floatValue]) * 100];
		
		[[outputEntity average] setValue:[data average]];
		[[outputEntity average] setPercent:([data average] / [interfaceSpeed floatValue]) * 100];
		
		[[outputEntity maximum] setValue:[data maximum]];
		[[outputEntity maximum] setPercent:([data maximum] / [interfaceSpeed floatValue]) * 100];
	}
	
	[outstandingRefreshes removeObject:data];
	
	if ([outstandingRefreshes count] == 0)
	{
		if (calcDeltaValues)
		{
			int day = 0;
			
			switch (dataPeriod)
			{//amount of days represented in seconds
				case 2: day = 86400;break;		//1d
				case 3: day = 604800; break;	//7d
				case 4: day = 2678400; break;	//31d
				case 5: day = 31536000; break;	//365d
			}
			
			double newDate = [referenceDate timeIntervalSince1970];
			newDate -= day;
			
			LCReportMetricHistory *deltaInput = [LCReportMetricHistory reportMetricHistoryWithEntity:[inputEntity baseEntity]
																				  withReferenceDate:[NSDate dateWithTimeIntervalSince1970:newDate]
																					  andDataPeriod:dataPeriod];
			[deltaInput addObserver:self 
							forKeyPath:@"refreshComplete" 
							   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
							   context:nil];
			[outstandingRefreshes addObject:deltaInput];
			[deltaInput refresh:XMLREQ_PRIO_NORMAL];
			
			LCReportMetricHistory *deltaOutput = [LCReportMetricHistory reportMetricHistoryWithEntity:[outputEntity baseEntity]
																				  withReferenceDate: [NSDate dateWithTimeIntervalSince1970:newDate]
																					  andDataPeriod:dataPeriod];
			[deltaOutput addObserver:self 
						 forKeyPath:@"refreshComplete" 
							options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
							context:nil];
			[outstandingRefreshes addObject:deltaOutput];
			[deltaOutput refresh:XMLREQ_PRIO_NORMAL];
		}
		else
		{
			[self deltaMetricRefreshComplete:YES];
		}
	}
}

-(void) deltaMetricRefreshCompleted:(id) object
{
	LCReportMetricHistory *data = object;
	NSString *temp = [[data metric] name];
	NSString *tempStr = [NSString stringWithString:@"in"];
	
	NSRange result = [temp rangeOfString:tempStr];
	if ( result.location != NSNotFound)
	{
		[[inputEntity minimum] setPercentDelta:([[inputEntity minimum] percent] - (([data minimum] / [interfaceSpeed floatValue]) * 100))];
		[[inputEntity average] setPercentDelta:([[inputEntity average] percent] - (([data average] / [interfaceSpeed floatValue]) * 100))];
		[[inputEntity maximum] setPercentDelta:([[inputEntity maximum] percent] - (([data maximum] / [interfaceSpeed floatValue]) * 100))];
	}
	else
	{
		[[outputEntity minimum] setPercentDelta:([[outputEntity minimum] percent] - (([data minimum] / [interfaceSpeed floatValue]) * 100))];
		[[outputEntity average] setPercentDelta:([[outputEntity average] percent] - (([data average] / [interfaceSpeed floatValue]) * 100))];
		[[outputEntity maximum] setPercentDelta:([[outputEntity maximum] percent] - (([data maximum] / [interfaceSpeed floatValue]) * 100))];
	}

	[outstandingRefreshes removeObject:data];
	
	if ([outstandingRefreshes count] == 0)
	{
		[self deltaMetricRefreshComplete:YES];
	}
}

-(void) collectCapacityPlanningDataForDate:(NSDate *)referenceDate
							withDataPeriod:(int)dataPeriod
{
	[self setReferenceDate:referenceDate];
	[self setDataPeriod:dataPeriod];
	
	[outstandingRefreshes release];
	outstandingRefreshes = [NSMutableArray array];
	
	LCReportMetricHistory *inputMetric = [LCReportMetricHistory reportMetricHistoryWithEntity:[[baseEntity childrenDictionary] valueForKey:@"utilpc_in"]
																		   withReferenceDate:referenceDate 
																			   andDataPeriod:dataPeriod];
	[inputMetric addObserver:self
				  forKeyPath:@"refreshComplete"
					options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
					context:nil];
	
	[outstandingRefreshes addObject:inputMetric];
	[inputMetric refresh:XMLREQ_PRIO_NORMAL];
	
	LCReportMetricHistory *outputMetric = [LCReportMetricHistory reportMetricHistoryWithEntity:[[baseEntity childrenDictionary] valueForKey:@"utilpc_out"]
																			withReferenceDate:referenceDate 
																				andDataPeriod:dataPeriod];
	[outputMetric addObserver:self
				  forKeyPath:@"refreshComplete"
					 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
					 context:nil];
	
	[outstandingRefreshes addObject:outputMetric];
	[outputMetric refresh:XMLREQ_PRIO_NORMAL];
}

-(void)completeCapacityPlanning:(id)data
{
	if (!criticalDate)
	{
		[self setCriticalDate:[data findCriticalDate]];
	}
	else if ([data findCriticalDate] < criticalDate)
	{
		[self setCriticalDate:[data findCriticalDate]];
	}
	
	[outstandingRefreshes removeObject:data];
	
	if ([outstandingRefreshes count] == 0)
	{
		[self capacityPlanningCompleted:YES];
	}
}
	
#pragma mark "MISC"
-(void) cancelRefresh
{
	LCReportMetricHistory *item;
	
	for (item in outstandingRefreshes)
	{
		[item cancelRefresh];
	}
}

#pragma mark "Refresh Complete Accessors"
-(BOOL) metricRefreshCompleted
{ return metricRefreshCompleted;}
-(void) setMetricRefreshCompleted:(BOOL)flag
{ metricRefreshCompleted = flag;} 

-(BOOL) baseEntityRefreshCompleted
{return baseEntityRefreshCompleted;}
-(void) setBaseEntityRefreshCompleted:(BOOL)flag
{ baseEntityRefreshCompleted = flag;}

-(BOOL) capacityPlanningCompleted
{	return capacityPlanningRefreshCompleted; }
-(void) setCapacityPlanningRefreshCompleted:(BOOL)flag
{ capacityPlanningRefreshCompleted = flag;}

-(BOOL) deltaMetricRefreshCompleted
{ return deltaMetricRefreshCompleted;}
-(void) setDeltaMetricRefreshCompleted:(BOOL)flag
{ deltaMetricRefreshCompleted = flag;}


@synthesize inputEntity;
@synthesize outputEntity;
@synthesize baseEntity;
@synthesize outstandingRefreshes;
@synthesize criticalDate;
@synthesize dataCollectionCompleted;
@end