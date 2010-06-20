//
//  LCReportInterface.h
//  Lithium Console
//
//  Created by Liam Elliott on 18/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCReportEntity.h"
#import "LCEntity.h"
#import "LCReportDevice.h"

@interface LCReportInterface : NSObject {
	
	LCReportEntity *inputEntity;
	LCReportEntity *outputEntity;
	
	LCEntity *baseEntity;
	
	NSMutableArray *outstandingRefreshes;
	
	LCReportDevice *parent;
	
	NSDate *referenceDate;
	int dataPeriod;
	
	BOOL calcDeltaValues;
	NSDate *criticalDate;
	
	NSString *reportDescription;

	NSNumber *interfaceSpeed;
	
	BOOL metricRefreshCompleted;
	BOOL dataCollectionCompleted;
	BOOL deltaMetricRefreshCompleted;
	BOOL baseEntityRefreshCompleted;
	BOOL capacityPlanningRefreshCompleted;
}

#pragma mark "Initialisation"
+(LCReportInterface *) reportInterfaceWithEntity:(LCEntity *)entity 
									   andParent:(LCReportDevice *) parent;
+(LCReportInterface *) reportInterfaceWithEntity:(LCEntity *)entity 
							   reportDescription:(NSString *)description
									   andParent:(LCReportDevice *) parent;

-(LCReportInterface *) init;

-(void) dealloc;

#pragma mark "Accessors"
-(LCReportEntity *) inputEntity;
-(LCReportEntity *) outputEntity;

-(NSString *)reportDescription;
-(void)setReportDesctiption:(NSString *)value;

-(NSString *) entityName;
-(LCEntity *) baseEntity;

-(NSNumber *) interfaceSpeed;
-(void) setInterfaceSpeed:(NSNumber *)value;
-(float) interfaceSpeedAsFloat;

-(NSDate *) criticalDate;

-(LCReportDevice *)parent;
-(void)setParent:(LCReportDevice *) value;

-(BOOL) calcDeltaValues;
-(void) setCalcDeltaValues:(BOOL)flag;

-(NSDate *)referenceDate;
-(void) setReferenceDate:(NSDate *)value;

-(int)dataPeriod;
-(void) setDataPeriod:(int) value;

#pragma mark "Data Collection"
-(void) collectReportDataForDate:(NSDate *)referenceDate 
				  withDataPeriod:(int)dataPeriod
		 andCalculateDeltaValues:(BOOL)calcDelta;

-(void) collectCapacityPlanningDataForDate:(NSDate *)referenceDate
							withDataPeriod:(int)dataPeriod;

#pragma mark "MISC"
-(void) cancelRefresh;

#pragma mark "Refresh complete Flags"
-(BOOL) metricRefreshCompleted;
-(void) setMetricRefreshCompleted:(BOOL)flag;

-(BOOL) baseEntityRefreshCompleted;
-(void) setBaseEntityRefreshCompleted:(BOOL)flag;

-(BOOL) capacityPlanningRefreshCompleted;
-(void) setCapacityPlanningRefreshCompleted:(BOOL)flag;

-(BOOL) deltaMetricRefreshCompleted;
-(void) setDeltaMetricRefreshCompleted:(BOOL)flag;

@property (retain,getter=inputEntity) LCReportEntity *inputEntity;
@property (retain,getter=outputEntity) LCReportEntity *outputEntity;
@property (retain,getter=baseEntity) LCEntity *baseEntity;
@property (retain) NSMutableArray *outstandingRefreshes;
@property (assign,getter=parent,setter=setParent:) LCReportDevice *parent;
@property (assign,getter=referenceDate,setter=setReferenceDate:) NSDate *referenceDate;
@property (getter=dataPeriod,setter=setDataPeriod:) int dataPeriod;
@property (getter=calcDeltaValues,setter=setCalcDeltaValues:) BOOL calcDeltaValues;
@property (retain,getter=criticalDate) NSDate *criticalDate;
@property (assign,getter=reportDescription,setter=setReportDesctiption:) NSString *reportDescription;
@property (assign,getter=interfaceSpeed,setter=setInterfaceSpeed:) NSNumber *interfaceSpeed;
@property (getter=metricRefreshCompleted,setter=setMetricRefreshCompleted:) BOOL metricRefreshCompleted;
@property BOOL dataCollectionCompleted;
@property (getter=deltaMetricRefreshCompleted,setter=setDeltaMetricRefreshCompleted:) BOOL deltaMetricRefreshCompleted;
@property (getter=baseEntityRefreshCompleted,setter=setBaseEntityRefreshCompleted:) BOOL baseEntityRefreshCompleted;
@end