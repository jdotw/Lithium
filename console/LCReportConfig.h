//
//  LCReportConfig.h
//  Lithium Console
//
//  Created by Liam Elliott on 23/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCReportDevice.h"
#import "LCReportCapacityPlanning.h"

@interface LCReportConfig : NSObject {
	
	//List of all devices in this report
	NSMutableArray *devices;
	
	//report config variables
	BOOL calcDeltaValues;
	BOOL generateCapPlan;
	BOOL saveAsWord;
	BOOL deleteXML;
	BOOL previewWord;
		
	NSString *templateFilename;
	NSString *reportTitle;
	NSImage *headerImage;
	NSString *headerFilename;
	NSString *headerTitle;
	NSImage *footerImage;
	NSString *footerFilename;
	NSString *footerTitle;
	
	int year;
	NSString *month;
	NSString *shortMonth;
	
	BOOL reportFinished;
	BOOL currentConfigSaved;

    //Lists for the Capacity planning
	NSMutableArray *threeMth;
	NSMutableArray *sixMth;
	NSMutableArray *nineMth;
	NSMutableArray *twelveMth;
		
	//Filename for the final report (XML file)
	NSString *reportFilename;
	
	//List of all devices being refreshed
	NSMutableArray *outstandingRefreshes;
	
	//used to calculate progress as a percent
	int refreshesInProgress;
	int maxRefreshes;
	
	BOOL dataCollectionCompleted;
	BOOL devicesRefreshed;
	BOOL capacityPlanningCompleted;
}

#pragma mark "Initialisation"

-(LCReportConfig *)init;
-(void)dealloc;


#pragma mark "Accessors"
-(NSMutableArray *)devices;
-(void)setDevice:(NSMutableArray *)value;

-(BOOL)currentConfigSaved;
-(void)setCurrentConfigSaved:(BOOL)flag;

-(int)refreshesInProgress;
-(void)setRefreshesInProgress:(int)value;
-(int)maxRefreshes;
-(void)setMaxRefreshes:(int)value;

-(int)currentProgress;
	
-(NSString *)currentProgressString;

-(BOOL)calcDeltaValues;
-(void)setCalcDeltaValues: (BOOL)flag;
-(BOOL)generateCapPlan;
-(void)setGenerateCapPlan:(BOOL)flag;
-(BOOL)saveAsWord;
-(void)setSaveAsWork:(BOOL)flag;
-(BOOL)deleteXML;
-(void)setDeleteXML:(BOOL)flag;
-(BOOL)previewWordDocument;
-(void)setPreviewWordDocument:(BOOL)flag;

-(NSString *)templateFilename;
-(void)setTemplateFilename:(NSString *)value;

-(NSString *)reportTitle;
-(void)setReportTitle:(NSString *)value;

-(NSImage *)headerImage;
-(void)setHeaderImage:(NSImage *)value;

-(NSString  *)headerFilename;
-(void)setHeaderFilename:(NSString *)value;

-(NSImage *)footerImage;
-(void)setFooterImage:(NSImage *)value;

-(NSString  *)footerFilename;
-(void)setFooterFilename:(NSString *) value;

-(BOOL)reportFinished;
-(void)setReportFinished:(BOOL)flag;

-(BOOL)devicesRefreshed;
-(void)setDevicesRefreshed:(BOOL)flag;

-(BOOL)dataCollectionComplete;
-(void)setDataCollectionComplete:(BOOL)flag;

-(BOOL)capacityPlanningCompleted;
-(void)setCapacityPlanningCompleted:(BOOL)flag;

#pragma mark "Report Generation"
-(void)refreshDevices;
-(void)getDataForDate:(NSDate *)refDate withDataPeriod:(int)period;

-(void)getCapacityPlanningForDate:(NSDate *)refDate withDataPeriod:(int)period;

-(void)generateReportXMLWithFilename:(NSString *)filename;

#pragma mark "Misc"

-(LCReportConfig *)clone;
-(void)saveAsWordDoc:(NSString *)filename;
-(void)cancelReport;

@property (assign,getter=devices,setter=setDevices:) NSMutableArray *devices;
@property (getter=calcDeltaValues,setter=setCalcDeltaValues:) BOOL calcDeltaValues;
@property (getter=generateCapPlan,setter=setGenerateCapPlan:) BOOL generateCapPlan;
@property (getter=saveAsWord,setter=setSaveAsWork:) BOOL saveAsWord;
@property (getter=deleteXML,setter=setDeleteXML:) BOOL deleteXML;
@property (getter=previewWordDocument,setter=setPreviewWordDocument:) BOOL previewWord;
@property (assign,getter=templateFilename,setter=setTemplateFilename:) NSString *templateFilename;
@property (assign,getter=reportTitle,setter=setReportTitle:) NSString *reportTitle;
@property (assign,getter=headerImage,setter=setHeaderImage:) NSImage *headerImage;
@property (assign,getter=headerFilename,setter=setHeaderFilename:) NSString *headerFilename;
@property (nonatomic,retain) NSString *headerTitle;
@property (assign,getter=footerImage,setter=setFooterImage:) NSImage *footerImage;
@property (assign,getter=footerFilename,setter=setFooterFilename:) NSString *footerFilename;
@property (nonatomic,retain) NSString *footerTitle;
@property int year;
@property (nonatomic,retain) NSString *month;
@property (nonatomic,retain) NSString *shortMonth;
@property (getter=reportFinished,setter=setReportFinished:) BOOL reportFinished;
@property (getter=currentConfigSaved,setter=setCurrentConfigSaved:) BOOL currentConfigSaved;
@property (nonatomic,retain) NSMutableArray *threeMth;
@property (nonatomic,retain) NSMutableArray *sixMth;
@property (nonatomic,retain) NSMutableArray *nineMth;
@property (nonatomic,retain) NSMutableArray *twelveMth;
@property (nonatomic,retain) NSString *reportFilename;
@property (nonatomic,retain) NSMutableArray *outstandingRefreshes;
@property (getter=refreshesInProgress,setter=setRefreshesInProgress:) int refreshesInProgress;
@property (getter=maxRefreshes,setter=setMaxRefreshes:) int maxRefreshes;
@property (getter=dataCollectionCompleted,setter=setDataCollectionComplete:) BOOL dataCollectionCompleted;
@property (getter=devicesRefreshed,setter=setDevicesRefreshed:) BOOL devicesRefreshed;
@property (getter=capacityPlanningCompleted,setter=setCapacityPlanningCompleted:) BOOL capacityPlanningCompleted;
@end
