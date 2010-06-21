//
//  LCReportDevice.h
//  Lithium Console
//
//  Created by Liam Elliott on 16/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCEntity.h"
#import "LCReportMetric.h"

@interface LCReportDevice : NSObject {

	LCEntity *device;
	
	NSMutableArray *metrics;
	NSMutableArray *interfaces;
	LCReportMetric *availability;
	
	NSMutableArray *capacityPlanning3mth;
	NSMutableArray *capacityPlanning6mth;
	NSMutableArray *capacityPlanning9mth;
	NSMutableArray *capacityPlanning12mth;
	
	NSDate *referenceDate;
	int dataPeriod;
	
	NSString *purpose;
	NSString *type;
	NSString *os;
	NSString *processor;
	NSString *memory;
	NSString *interfacesDesc;
	NSString *imageFilename;
	NSString *imageTitle;
	NSImage *image;

	BOOL includeInReport;
	BOOL calcDeltaValues;
	
	NSMutableArray *outstandingEntities;
	NSMutableArray *outstandingInterfaces;
	
	BOOL dataCollectionComplete;
	BOOL capacityPlanningCompleted;
}

#pragma mark "Initialisation"
+(LCReportDevice *)reportDeviceWithEntity:(LCEntity *)entity;

-(LCReportDevice *)init;
-(void)dealloc;

#pragma mark "Accessors"
-(LCEntity *) device;
-(void) setDevice:(LCEntity *)value;

-(NSString *)type;
-(void)setType:(NSString *)value;

-(NSString *)os;
-(void)setOS:(NSString *)value;

-(NSString *)processor;
-(void)setProcessor:(NSString *)value;

-(NSString *)purpose;
-(void)setPurpose:(NSString *)value;

-(NSString *)memory;
-(void)setMemory:(NSString *)value;

-(NSString *)interfaceDesc;
-(void)setInterfaceDesc:(NSString *)value;

-(NSMutableArray *)metrics;
-(void)setMetrics:(NSMutableArray *)value;

-(NSMutableArray *)interfaces;
-(void)setInterfaces:(NSMutableArray *)value;

-(NSMutableArray *)capacityPlanning3Mth;
-(void)setCapacityPlanning3Mth:(NSMutableArray *)value;

-(NSMutableArray *)capacityPlanning6Mth;
-(void)setCapacityPlanning6Mth:(NSMutableArray *)value;

-(NSMutableArray *)capacityPlanning9Mth;
-(void)setCapacityPlanning9Mth:(NSMutableArray *)value;

-(NSMutableArray *)capacityPlanning12Mth;
-(void)setCapacityPlanning12Mth:(NSMutableArray *)value;

-(BOOL) includeInReport;
-(void) setIncludeInReport:(BOOL)flag;

-(NSString *) longMonth;
-(NSString *) shortMonth;
-(int) year;

-(NSDate *) referenceDate;
-(void)setReferenceDate:(NSDate *)value;
-(int)dataPeriod;
-(void)setDataPeriod:(int)value;
-(BOOL)calcDeltaValues;
-(void)setCalcDeltaValues:(BOOL)flag;

-(LCReportMetric *)availability;

-(NSString *) imageFileName;
-(void)setImageFilename:(NSString *)value;
-(NSString *) imageTitle;
-(void)setImageTitle:(NSString *)value;
-(NSImage *)image;
-(void)setImage:(NSImage *)value;

-(int)outstandingRefreshes;


-(BOOL) dataCollectionComplete;
-(void) setDataCollectionComplete:(BOOL) flag;

-(BOOL) capacityPlanningCompleted;
-(void) setCapacityPlanningCompleted:(BOOL)flag;

#pragma mark "Data Collection"
-(void) collectDataForDate:(NSDate *)refDate andPeriod:(int)dataPeriod andCalculateDeltaValues:(BOOL)calcDelta;
-(void) collectCapacityPlanningDataForDate:(NSDate *)refDate andPeriod:(int)dataPeriod;
-(void) calculateCapacityPlanningForMetric:(LCEntity *) entity withCriticalDate:(NSDate *)criticalDate;

#pragma mark "Misc"
-(void) cancelRefresh;
@property (assign,getter=device,setter=setDevice:) LCEntity *device;
@property (assign,getter=metrics,setter=setMetrics:) NSMutableArray *metrics;
@property (assign,getter=interfaces,setter=setInterfaces:) NSMutableArray *interfaces;
@property (retain,getter=availability) LCReportMetric *availability;
@property (assign,getter=capacityPlanning3Mth,setter=setCapacityPlanning3Mth:) NSMutableArray *capacityPlanning3mth;
@property (assign,getter=capacityPlanning6Mth,setter=setCapacityPlanning6Mth:) NSMutableArray *capacityPlanning6mth;
@property (assign,getter=capacityPlanning9Mth,setter=setCapacityPlanning9Mth:) NSMutableArray *capacityPlanning9mth;
@property (assign,getter=capacityPlanning12Mth,setter=setCapacityPlanning12Mth:) NSMutableArray *capacityPlanning12mth;
@property (assign,getter=referenceDate,setter=setReferenceDate:) NSDate *referenceDate;
@property (getter=dataPeriod,setter=setDataPeriod:) int dataPeriod;
@property (assign,getter=purpose,setter=setPurpose:) NSString *purpose;
@property (assign,getter=type,setter=setType:) NSString *type;
@property (assign,getter=os,setter=setOS:) NSString *os;
@property (assign,getter=processor,setter=setProcessor:) NSString *processor;
@property (assign,getter=memory,setter=setMemory:) NSString *memory;
@property (assign,getter=interfaceDesc,setter=setInterfaceDesc:) NSString *interfacesDesc;
@property (assign,getter=imageFileName,setter=setImageFilename:) NSString *imageFilename;
@property (assign,getter=imageTitle,setter=setImageTitle:) NSString *imageTitle;
@property (assign,getter=image,setter=setImage:) NSImage *image;
@property (getter=includeInReport,setter=setIncludeInReport:) BOOL includeInReport;
@property (getter=calcDeltaValues,setter=setCalcDeltaValues:) BOOL calcDeltaValues;
@property (nonatomic,retain) NSMutableArray *outstandingEntities;
@property (nonatomic,retain) NSMutableArray *outstandingInterfaces;
@property (getter=capacityPlanningCompleted,setter=setCapacityPlanningCompleted:) BOOL capacityPlanningCompleted;
@end