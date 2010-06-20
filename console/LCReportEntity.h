//
//  LCReportEntity.h
//  Lithium Console
//
//  Created by Liam Elliott on 16/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCEntity.h"
#import "LCReportValue.h"



@interface LCReportEntity : NSObject {

	LCEntity *baseEntity;
	NSString *reportDescription;
	
	LCReportValue *minimum;
	LCReportValue *average;
	LCReportValue *maximum;
	
	NSDate *criticalDate;
	
	NSDate *referenceDate;
	id *parent;
	int dataPeriod;
}

#pragma mark "Initialisation"
+(LCReportEntity *)reportEntityWithEntity:(LCEntity *)newEntity andParent:(id *)newParent;

-(LCReportEntity *)initWithEntity:(LCEntity *)newEntity andParent:(id *)newParent;
-(LCReportEntity *)init;

-(void) dealloc;
	
#pragma mark "Accessors"
	
-(LCEntity *) baseEntity;
-(void) setBaseEntity:(LCEntity *)entity;

-(NSString *) entityName;
-(NSString *) reportDescription;
-(void)setReportDescription:(NSString *)newDescription;

-(LCReportValue *) minimum;
-(void) setMinimum:(LCReportValue *)newMinimum;
-(LCReportValue *) average;
-(void) setAverage:(LCReportValue *)newAverage;
-(LCReportValue *) maximum;
-(void) setMaximum:(LCReportValue *)newMaximum;

-(NSDate *)criticalDate;
-(void) setCriticalDate:(NSDate *)newDate;
-(NSDate *)referenceDate;
-(void) setReferenceDate:(NSDate *)newDate;
-(int) dataPeriod;
-(void)setDataPeriod:(int)newDataPeriod;

-(id *) parent;
-(void) setParent:(id *)newParent;

-(NSString *) units;
-(void) setUnits:(NSString *)setUnits;
@property (assign,getter=baseEntity,setter=setBaseEntity:) LCEntity *baseEntity;
@property (assign,getter=reportDescription,setter=setReportDescription:) NSString *reportDescription;
@property (assign,getter=minimum,setter=setMinimum:) LCReportValue *minimum;
@property (assign,getter=average,setter=setAverage:) LCReportValue *average;
@property (assign,getter=maximum,setter=setMaximum:) LCReportValue *maximum;
@property (assign,getter=criticalDate,setter=setCriticalDate:) NSDate *criticalDate;
@property (assign,getter=referenceDate,setter=setReferenceDate:) NSDate *referenceDate;
@property (getter=parent,setter=setParent:) id *parent;
@property (getter=dataPeriod,setter=setDataPeriod:) int dataPeriod;
@end
