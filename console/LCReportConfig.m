//
//  LCReportConfig.m
//  Lithium Console
//
//  Created by Liam Elliott on 23/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCReportConfig.h"
#import "LCReportDevice.h"
#import "LCReportCapacityPlanning.h"
#import "LCEntity.h"

@implementation LCReportConfig


#pragma mark "Initialisation"

-(LCReportConfig *)init
{
	[super init];
	devices = [NSMutableArray array];
	
	threeMth = [NSMutableArray array];
	sixMth = [NSMutableArray array];
	nineMth = [NSMutableArray array];
	twelveMth = [NSMutableArray array];
	
	outstandingRefreshes = [NSMutableArray array];
	
	calcDeltaValues = NO;
	generateCapPlan = NO;
	
	saveAsWord = NO;
	deleteXML = NO;
	previewWord = NO;
	
	currentConfigSaved = YES;
	reportFinished = NO;
	
	/****To-DO***
	**	Add Default Template
	*******/
	
	return self;
}

-(void)dealloc
{
	[devices release];
	[threeMth release];
	[sixMth release];
	[nineMth release];
	[twelveMth release];


	[super dealloc];
}

#pragma mark "Accessors"
-(NSMutableArray *)devices
{return devices;}
-(void)setDevices:(NSMutableArray *)value
{devices = value;}

-(BOOL)currentConfigSaved
{return currentConfigSaved;}
-(void)setCurrentConfigSaved:(BOOL)flag
{currentConfigSaved = flag;}

-(int)refreshesInProgress
{return refreshesInProgress;}
-(void)setRefreshesInProgress:(int)value
{refreshesInProgress = value;}
-(int)maxRefreshes
{return maxRefreshes;}
-(void)setMaxRefreshes:(int)value
{maxRefreshes = value;}

-(int)currentProgress
{
	if (refreshesInProgress == 0)
	{
		return 100;
	}
	else if (maxRefreshes == 0)
	{
		return 1;
	}
	else
	{
		int returnValue = (int) (((float)refreshesInProgress / (float)maxRefreshes) * 100);
		if (100 - returnValue <= 0)
			return 1;
		
		return 100 - returnValue;
	}
}

-(NSString *)currentProgressString
{return [NSString stringWithFormat:@"%i%%", [self currentProgress]];}

-(BOOL)calcDeltaValues
{return calcDeltaValues;}
-(void)setCalcDeltaValues: (BOOL)flag
{calcDeltaValues = flag;}

-(BOOL)generateCapPlan
{return generateCapPlan;}
-(void)setGenerateCapPlan:(BOOL)flag
{generateCapPlan = flag;}

-(BOOL)saveAsWord
{return saveAsWord;}
-(void)setSaveAsWork:(BOOL)flag
{saveAsWord = flag;}

-(BOOL)deleteXML
{return deleteXML;}
-(void)setDeleteXML:(BOOL)flag
{deleteXML = flag;}

-(BOOL)previewWordDocument
{return previewWord;}
-(void)setPreviewWordDocument:(BOOL)flag
{previewWord = flag;}

-(NSString *)templateFilename
{return templateFilename;}
-(void)setTemplateFilename:(NSString *)value
{templateFilename = value;}

-(NSString *)reportTitle
{return reportTitle;}
-(void)setReportTitle:(NSString *)value
{reportTitle = value;}

-(NSImage *)headerImage
{return headerImage;}
-(void)setHeaderImage:(NSImage *)value
{ headerImage = value;}

-(NSString  *)headerFilename
{return headerFilename;}	
-(void)setHeaderFilename:(NSString *)value
{
	headerFilename = value;
	if (headerImage)
	{
		[headerImage release];
	}
	[self setHeaderImage:[[[NSImage alloc]initWithContentsOfFile:headerFilename]retain]];
	
	NSArray *temp = [headerFilename componentsSeparatedByString:@"// "];
	headerTitle = [temp lastObject];
}
		
-(NSImage *)footerImage
{return footerImage;}
-(void)setFooterImage:(NSImage *)value
{footerImage = value;}

-(NSString  *)footerFilename
{return footerFilename;}
-(void)setFooterFilename:(NSString *) value
{
	footerFilename = value;
	if (footerImage)
	{
		[footerImage release];
	}
	[self setFooterImage:[[[NSImage alloc]initWithContentsOfFile:footerFilename]retain]];
	
	NSArray *temp = [footerFilename componentsSeparatedByString:@"//"];
		footerTitle = [temp lastObject];
}


-(BOOL)reportFinished
{return reportFinished;}
-(void)setReportFinished:(BOOL)flag
{reportFinished = flag;}

-(BOOL)devicesRefreshed
{return devicesRefreshed;}
-(void)setDevicesRefreshed:(BOOL)flag
{devicesRefreshed = flag;}

-(BOOL)dataCollectionCompleted
{return dataCollectionCompleted;}
-(void)setDataCollectionComplete:(BOOL)flag
{dataCollectionCompleted = flag;} 

-(BOOL)capacityPlanningCompleted
{return capacityPlanningCompleted;}
-(void)setCapacityPlanningCompleted:(BOOL)flag
{capacityPlanningCompleted = flag;}

#pragma mark "Report Generation"
-(void)refreshDevices
{
	int temp = 0;
	
	NSEnumerator *deviceEnumerator;
	LCReportDevice *curDevice;
	
	while (curDevice = [deviceEnumerator nextObject])
	{
		if (![[curDevice device] initialRefreshPerformed] || ![[curDevice device] refreshInProgress])
		{
			[curDevice addObserver:self 
					 forKeyPath:@"refreshInProgress" 
						options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
						context:nil];
			
			[[curDevice device] refresh:XMLREQ_PRIO_NORMAL];
			temp++;
		}
	}
	maxRefreshes = temp;
	refreshesInProgress = temp;
	
}

-(void)deviceRefreshComplete
{
	refreshesInProgress--;
	
	if (refreshesInProgress == 0)
	{
		[self setDevicesRefreshed:YES];
	}
}	

-(void)getDataForDate:(NSDate *)refDate withDataPeriod:(int)period
{
	int temp=0;
	[self setMaxRefreshes:0];
	
	LCReportDevice *curDevice;
	
	for (curDevice in devices)
	{
		if ([curDevice includeInReport])
		{
			[outstandingRefreshes addObject:curDevice];
			
			[curDevice addObserver:self 
						forKeyPath:@"dataCollectionCompleted" 
						   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
						   context:nil];
			
			[curDevice collectDataForDate:refDate andPeriod:period andCalculateDeltaValues:calcDeltaValues];
			
			temp += [curDevice outstandingRefreshes];
		}
	}
	[self setMaxRefreshes:temp];
	
	NSCalendarDate *calDate = [refDate dateWithCalendarFormat:nil timeZone:nil];
	
	year = [calDate yearOfCommonEra];
	month = [calDate descriptionWithCalendarFormat:@"%B"];
	shortMonth = [calDate descriptionWithCalendarFormat:@"%b"];
}

-(void)deviceDataCollectionCompleted:(id)curDevice
{
	[outstandingRefreshes removeObject:curDevice];
	
	if ([outstandingRefreshes count] == 0)
	{
		[self setDataCollectionCompleted:YES];
	}
}
		
-(void)getCapacityPlanningForDate:(NSDate *)refDate withDataPeriod:(int)period
{
	NSCalendarDate *referenceDate = [refDate dateWithCalendarFormat:nil timeZone:nil];
	
	//Ensure the capacity planning is not being run on the current year when you are in Jan -> Mar
	if ([referenceDate monthOfYear] <= 3)
		referenceDate = [referenceDate dateByAddingYears:-1 months:0 days:0 hours:0 minutes:0 seconds:0];
	
	int temp = 0;
	maxRefreshes = 0;
	
	LCReportDevice *curDevice;
	for (curDevice in devices)
	{
		if ([curDevice includeInReport])
		{
			[curDevice addObserver:self 
						forKeyPath:@"capacityPlanningCompleted" 
						   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
						   context:nil];
			
			[curDevice collectCapacityPlanningDataForDate:referenceDate andPeriod:period];
			[outstandingRefreshes addObject:curDevice];
			
			temp += [curDevice outstandingRefreshes];
		}
	}
	
	[self setMaxRefreshes:temp];
	
}

-(void)deviceCapacityPlanningCompleted:(id)curDevice
{
	NSEnumerator *capPlanEnumerator = [[curDevice  capacityPlanning3Mth] objectEnumerator];
	LCReportCapacityPlanning *plan;
	
	while (plan = [capPlanEnumerator nextObject])
	{
		[threeMth addObject:plan];
	}
	
	capPlanEnumerator = [[curDevice  capacityPlanning6Mth] objectEnumerator];
	while (plan = [capPlanEnumerator nextObject])
	{
		[sixMth addObject:plan];
	}
	capPlanEnumerator = [[curDevice  capacityPlanning9Mth] objectEnumerator];
	while (plan = [capPlanEnumerator nextObject])
	{
		[nineMth addObject:plan];
	}
	capPlanEnumerator = [[curDevice  capacityPlanning12Mth] objectEnumerator];
	while (plan = [capPlanEnumerator nextObject])
	{
		[twelveMth addObject:plan];
	}
	
	[outstandingRefreshes removeObject:curDevice];
	if ([outstandingRefreshes count] == 0)
		[self setCapacityPlanningComplete:YES];
}

-(void)generateReportXMLWithFilename:(NSString *)filename
{
	/* Liam Elliott (24/10/06)
	** Bugzilla bug number: Bug 117
	**
	** Not completed as yet... 
	** MS Word:Mac doesn't support XML file saving and opening.
	** to be finished when XML support is introduced (possibly 2007)
	*/
}

#pragma mark "Misc"
/*These Still need to be completed.... 24/10/06
-(LCReportConfig *)clone
{
}
-(void)saveAsWordDoc:(NSString *)filename
{
}
*/
-(void)cancelReport
{
	LCReportDevice *device;
	
	for (device in devices)
	{
		[device cancelRefresh];
	}
}

@synthesize headerTitle;
@synthesize footerTitle;
@synthesize year;
@synthesize month;
@synthesize shortMonth;
@synthesize threeMth;
@synthesize sixMth;
@synthesize nineMth;
@synthesize twelveMth;
@synthesize reportFilename;
@synthesize outstandingRefreshes;
@end
