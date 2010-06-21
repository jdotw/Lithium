//
//  LCReportMetric.h
//  Lithium Console
//
//  Created by Liam Elliott on 16/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCReportEntity.h"
#import "LCReportMetricHistory.h"

@interface LCReportMetric : NSObject {
	
	LCReportEntity *entity;
	LCReportMetricHistory *metricHistory;
	LCReportMetricHistory *capPlanHistory;
	
	NSDate *referenceDate;
	int dataPeriod;
	
	BOOL calcDeltaValues;
	
	BOOL metricRefreshCompleted;
	BOOL dataCollectionCompleted;
	BOOL deltaMetricRefreshCompleted;
	BOOL baseEntityRefreshCompleted;
	BOOL capacityPlanningRefreshCompleted;
}

#pragma mark "Initialisation"
+(LCReportMetric *) reportMetricWithEntity:(LCEntity *)newEntity andParent:(id *)newParent;

-(LCReportMetric *) init;
-(LCReportMetric *) initWithEntity:(LCEntity *)newEntity andParent:(id *)newParent;

-(void) dealloc;

#pragma mark "Data Collection"
-(void) collectReportDataForDate:(NSDate *)referenceDate 
				  withDataPeriod:(int)dataPeriod 
		 andCalculateDeltaValues:(BOOL) calcDeltaValues;

-(void) collectCapacityPlanningDataForDate:(NSDate *)referenceDate 
							withDataPeriod:(int)dataPeriod;

#pragma mark "Accessors"
-(LCReportEntity *)entity;
-(LCReportValue *)minimum;
-(LCReportValue *)average;
-(LCReportValue *)maximum;

-(NSString *)reportDescription;
-(void)setReportDescription:(NSString *)value;

-(NSString *)entityName;

-(NSString *)units;
-(void) setUnits:(NSString *)value;
				  

-(id *) parent;
-(void) setParent:(id *)value;

-(void)cancelRefresh;

-(NSDate *)referenceDate;
-(void) setReferenceDate:(NSDate *)value;

-(int)dataPeriod;
-(void)setDataPeriod:(int)value;

-(BOOL)calcDeltaValues;
-(void)setCalcDeltaValues:(BOOL)flag;

#pragma mark "Refresh complete Flags"
-(BOOL) metricRefreshCompleted;
-(void) setMetricRefreshCompleted:(BOOL)flag;

-(BOOL) baseEntityRefreshCompleted;
-(void) setBaseEntityRefreshCompleted:(BOOL)flag;

-(BOOL) capacityPlanningRefreshCompleted;
-(void) setCapacityPlanningRefreshCompleted:(BOOL)flag;

-(BOOL) deltaMetricRefreshCompleted;
-(void) setDeltaMetricRefreshCompleted:(BOOL)flag;

@property (retain,getter=entity) LCReportEntity *entity;
@property (nonatomic,retain) LCReportMetricHistory *metricHistory;
@property (nonatomic,retain) LCReportMetricHistory *capPlanHistory;
@property (assign,getter=referenceDate,setter=setReferenceDate:) NSDate *referenceDate;
@property (getter=dataPeriod,setter=setDataPeriod:) int dataPeriod;
@property (getter=calcDeltaValues,setter=setCalcDeltaValues:) BOOL calcDeltaValues;
@property (getter=metricRefreshCompleted,setter=setMetricRefreshCompleted:) BOOL metricRefreshCompleted;
@property BOOL dataCollectionCompleted;
@property (getter=deltaMetricRefreshCompleted,setter=setDeltaMetricRefreshCompleted:) BOOL deltaMetricRefreshCompleted;
@property (getter=baseEntityRefreshCompleted,setter=setBaseEntityRefreshCompleted:) BOOL baseEntityRefreshCompleted;
@property (getter=capacityPlanningRefreshCompleted,setter=setCapacityPlanningRefreshCompleted:) BOOL capacityPlanningRefreshCompleted;
@end
