//
//  LCReportCapacityPlanning.h
//  Lithium Console
//
//  Created by Liam Elliott on 23/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCEntity.h"

@interface LCReportCapacityPlanning : NSObject {

	NSString *text;
	NSString *masterText;
	int period;
	LCEntity *baseEntity;
	BOOL includeInReport;
}

#pragma mark "Initilisation"
+(LCReportCapacityPlanning *)ReportCapacityPlanningWithEntity:(LCEntity *)initEntity andPeriod:(int)value;

-(LCReportCapacityPlanning *)initWithEntity:(LCEntity *)initEntity;
-(void)dealloc;

#pragma mark "Accessors"
-(LCEntity *)baseEntity;

-(int)period;
-(void)setPeriod:(int)value;

-(NSString *)description;
-(void)setDescription:(NSString *)value;

-(NSString *)masterDescription;
-(void)setMasterDescription:(NSString *)value;
-(NSString *)device;
-(NSString *)object;

-(BOOL)includeInReport;
-(void)setIncludeInReport:(BOOL)flag;
@property (assign,getter=description,setter=setDescription:) NSString *text;
@property (assign,getter=masterDescription,setter=setMasterDescription:) NSString *masterText;
@property (getter=period,setter=setPeriod:) int period;
@property (retain,getter=baseEntity) LCEntity *baseEntity;
@property (getter=includeInReport,setter=setIncludeInReport:) BOOL includeInReport;
@end