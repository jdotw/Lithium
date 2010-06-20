//
//  LCReportMetric.m
//  Lithium Console
//
//  Created by Liam Elliott on 16/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCReportMetric.h"
#import "LCEntity.h"


@implementation LCReportMetric

#pragma mark "Initialisation"
+(LCReportMetric *) reportMetricWithEntity:(LCEntity *)newEntity andParent:(id *)newParent
{
	return [[[self alloc]initWithEntity:newEntity andParent:newParent] autorelease];
}

-(LCReportMetric *)init
{
	[super init];
	
    metricRefreshCompleted = NO;
	dataCollectionCompleted = NO;
	deltaMetricRefreshCompleted = NO;
	baseEntityRefreshCompleted = NO;
	capacityPlanningRefreshCompleted = NO;
	
	return self;
}

-(LCReportMetric *)initWithEntity:(LCEntity *) newEntity andParent:(id *)newParent
{
	[super init]; 
	entity = [[[LCReportEntity alloc] initWithEntity:newEntity andParent:newParent]retain];
	
	return self;
}

-(void) dealloc
{
	[entity release];
	[super dealloc];
}

#pragma mark "Data Collection"
-(void) collectReportDataForDate:(NSDate *)newDate 
				  withDataPeriod:(int)newPeriod
		 andCalculateDeltaValues:(BOOL)newFlag
{
	[self setReferenceDate:newDate];
	[self setDataPeriod:newPeriod];
	[self setCalcDeltaValues:newFlag];
	
	metricHistory = [LCReportMetricHistory reportMetricHistoryWithMetric:[entity baseEntity] 
													   withReferenceDate:referenceDate 
														   andDataPeriod:dataPeriod];
	
	[metricHistory addObserver:self 
					forKeyPath:@"refreshComplete" 
					   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
					   context:nil];
	
	[metricHistory refresh:XMLREQ_PRIO_NORMAL];
}

-(void)completeMetricRefresh
{
	[[entity minimum] setValue:[metricHistory minimum]];
	[[entity average] setValue:[metricHistory average]];
	[[entity maximum] setValue:[metricHistory maximum]];
	
	[self setMetricRefreshCompleted:YES];
	
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
		[metricHistory release];
		
		double newDate = [referenceDate timeIntervalSince1970];
		newDate -= day;
		
		metricHistory = [LCReportMetricHistory reportMetricHistoryWithMetric:[entity baseEntity]
														   withReferenceDate: [NSDate dateWithTimeIntervalSince1970:newDate]
															   andDataPeriod:dataPeriod];
		[metricHistory addObserver:self 
						forKeyPath:@"refreshComplete" 
						   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
						   context:nil];
		
		[metricHistory refresh:XMLREQ_PRIO_NORMAL];
	}
	else
	{
		[self setDeltaMetricRefreshCompleted:YES];
	}
	
}

-(void) completeDeltaMetricRefresh
{
	[[entity minimum] setValueDelta: ([[entity minimum] value] - [metricHistory minimum])];
	[[entity average] setValueDelta: ([[entity average] value] - [metricHistory average])];
	[[entity maximum] setValueDelta: ([[entity maximum] value] - [metricHistory maximum])];
	
	[metricHistory release];
	[self setDeltaMetricRefreshCompleted:YES];
}

#pragma mark "Collect Capacity Planning"

-(void) collectCapacityPlanningDataForDate:(NSDate *)theDate withDataPeriod:(int)thePeriod
{
	[self setReferenceDate:theDate];
	[self setDataPeriod:thePeriod];
	
	[[entity baseEntity] addObserver:self 
					forKeyPath:@"refreshInProgress" 
					   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
					   context:nil];
	
	[[entity baseEntity] refreshWithPriority:XMLREQ_PRIO_NORMAL];
}

-(void)completeBaseEntityRefresh
{
	capPlanHistory = [LCReportMetricHistory reportMetricHistoryWithMetric:[entity baseEntity]
														withReferenceDate:referenceDate andDataPeriod:dataPeriod];
	[capPlanHistory addObserver:self 
					 forKeyPath:@"refreshComplete"
						options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
						context:nil];
	
	[capPlanHistory refresh:XMLREQ_PRIO_NORMAL];
}

-(void)completeCapacityplanningRefresh
{
	[entity setCriticalDate:[capPlanHistory findCriticalDate]];
	[capPlanHistory release];
	
	[self setCapacityPlanningRefreshCompleted:YES];
}


#pragma mark "Accessors"
-(LCReportEntity *)entity
{ return entity;}

-(LCReportValue *)minimum
{	return [entity minimum];}

-(LCReportValue *)average
{	return [entity average];}
	
-(LCReportValue *)maximum
{ return [entity maximum];}

-(NSString *)reportDescription
{ return [entity reportDescription];}

-(void)setReportDescription:(NSString *)value
{ [entity setReportDescription:value];}

-(NSString *)entityName
{	 return [entity entityName];}

-(NSString *)units
{ return [entity units];}

-(void) setUnits:(NSString *)value
{ [entity setUnits:value];}

-(id *) parent
{ return [entity parent];}

-(void) setParent:(id *)value
{	[entity setParent:value];}

-(NSDate *)referenceDate
{ return referenceDate;}
-(void) setReferenceDate:(NSDate *)value
{ referenceDate = value;}

-(int)dataPeriod
{ return dataPeriod;}
-(void)setDataPeriod:(int)value
{dataPeriod = value;}

-(BOOL)calcDeltaValues
{ return calcDeltaValues;}
-(void)setCalcDeltaValues:(BOOL)flag
{ calcDeltaValues = flag;}

#pragma mark "Key-Value Observing"
-(void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{

	if ([keyPath compare:@"refreshComplete"] == NSOrderedSame)
	{
		if (metricRefreshCompleted)
		{
			[self completeMetricRefresh];
		}
		else if (deltaMetricRefreshCompleted)
		{
			[self completeDeltaMetricRefresh];
		}
	}
	else if ([keyPath compare:@"refreshInProgress"] == NSOrderedSame)
	{
		[self completeBaseEntityRefresh];
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

-(BOOL) capacityPlanningRefreshCompleted
{	return capacityPlanningRefreshCompleted; }
-(void) setCapacityPlanningRefreshCompleted:(BOOL)flag
{ capacityPlanningRefreshCompleted = flag;}

-(BOOL) deltaMetricRefreshCompleted
{ return deltaMetricRefreshCompleted;}
-(void) setDeltaMetricRefreshCompleted:(BOOL)flag
{ deltaMetricRefreshCompleted = flag;}

	
-(void)cancelRefresh;
{
	if (metricHistory)
		[metricHistory cancelRefresh];
	if (capPlanHistory)
		[capPlanHistory cancelRefresh];
}

@synthesize entity;
@synthesize metricHistory;
@synthesize capPlanHistory;
@synthesize dataCollectionCompleted;
@end
