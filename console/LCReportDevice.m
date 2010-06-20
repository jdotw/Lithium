//
//  LCReportDevice.m
//  Lithium Console
//
//  Created by Liam Elliott on 16/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCReportDevice.h"
#import "LCReportInterface.h"
#import "LCReportCapacityPlanning.h"

@implementation LCReportDevice


#pragma mark "Initialisation"
+(LCReportDevice *)reportDeviceWithEntity:(LCEntity *)entity
{
	LCReportDevice *temp = [[[LCReportDevice alloc] init] autorelease];
	[temp setDevice:[entity device]];
	
	return temp;
}

-(LCReportDevice *)init
{ 
	[super init];

	metrics = [NSMutableArray array];
	interfaces = [NSMutableArray array];
	
	includeInReport = YES;
	
	capacityPlanning3mth = [NSMutableArray array];
	capacityPlanning6mth = [NSMutableArray array];
	capacityPlanning9mth = [NSMutableArray array];
	capacityPlanning12mth = [NSMutableArray array];
	
	outstandingEntities = [NSMutableArray array];
	outstandingInterfaces = [NSMutableArray array];
	
	return self;
}

-(void)dealloc
{
	[device release];
	[super dealloc];
}

#pragma mark "Accessors"
-(LCEntity *) device
{return device;}
-(void) setDevice:(LCEntity *)value
{device = value;}

-(NSString *)type
{return type;}
-(void)setType:(NSString *)value
{type = value;}

-(NSString *)os
{return os;}
-(void)setOS:(NSString *)value
{os = value;}

-(NSString *)processor
{return processor;}
-(void)setProcessor:(NSString *)value
{processor=value;}

-(NSString *)purpose
{return purpose;}
-(void)setPurpose:(NSString *)value
{purpose = value;}

-(NSString *)memory
{return memory;}
-(void)setMemory:(NSString *)value
{memory = value;}

-(NSString *)interfaceDesc
{return interfacesDesc;}
-(void)setInterfaceDesc:(NSString *)value
{interfacesDesc = value;}

-(NSMutableArray *)metrics
{return metrics;}
-(void)setMetrics:(NSMutableArray *)value
{metrics = value;}

-(NSMutableArray *)interfaces
{return interfaces;}
-(void)setInterfaces:(NSMutableArray *)value
{interfaces = value;}

-(NSMutableArray *)capacityPlanning3Mth
{ return capacityPlanning3mth;}
-(void)setCapacityPlanning3Mth:(NSMutableArray *)value
{capacityPlanning3mth = value;}

-(NSMutableArray *)capacityPlanning6Mth
{ return capacityPlanning6mth;}
-(void)setCapacityPlanning6Mth:(NSMutableArray *)value
{capacityPlanning6mth = value;}

-(NSMutableArray *)capacityPlanning9Mth
{return capacityPlanning9mth;}
-(void)setCapacityPlanning9Mth:(NSMutableArray *)value
{capacityPlanning9mth = value;}

-(NSMutableArray *)capacityPlanning12Mth
{return capacityPlanning12mth;}
-(void)setCapacityPlanning12Mth:(NSMutableArray *)value
{capacityPlanning12mth = value;}

-(BOOL) includeInReport
{return includeInReport;}
-(void) setIncludeInReport:(BOOL)flag
{includeInReport = flag;}

-(NSString *) longMonth
{
	NSCalendarDate *date = [referenceDate dateWithCalendarFormat:nil timeZone:nil];
	return [date descriptionWithCalendarFormat:@"%B"];
}
	
-(NSString *) shortMonth
{
	NSCalendarDate *date = [referenceDate dateWithCalendarFormat:nil timeZone:nil];
	return [date descriptionWithCalendarFormat:@"%b"];
}

-(int) year
{
	NSCalendarDate *date = [referenceDate dateWithCalendarFormat:nil timeZone:nil];
	return [date descriptionWithCalendarFormat:@"%Y"];
}

-(NSDate *) referenceDate
{ return referenceDate;}
-(void)setReferenceDate:(NSDate *)value
{referenceDate = value;}

-(int) dataPeriod
{return dataPeriod;}
-(void)setDataPeriod:(int)value
{dataPeriod = value;}

-(BOOL)calcDeltaValues
{return calcDeltaValues;}
-(void)setCalcDeltaValues:(BOOL)flag
{calcDeltaValues = flag;}

-(LCReportMetric *)availability
{return availability;}

-(NSString *) imageFileName
{return imageFilename;}
-(void)setImageFilename:(NSString *)value
{
	imageFilename = value;
	
	if (image)
	{
		NSImage *temp = [[[NSImage alloc] initWithContentsOfFile:imageFilename]retain];
		if (temp)
		{
			[image release];
			[self setImage:temp];
			
			NSArray *tempStr = [imageFilename componentsSeparatedByString:@"//"];
				[self setImageTitle:[tempStr lastObject]];
			
		}
	}
}
		
-(NSString *) imageTitle
{return imageTitle;}
-(void)setImageTitle:(NSString *)value
{imageTitle = value;}

-(NSImage *)image
{ return image;}
-(void)setImage:(NSImage *)value
{image = value;}

-(int)outstandingRefreshes
{return ([outstandingEntities count] + [outstandingInterfaces count]);}

-(BOOL)dataCollectionCompleted
{return dataCollectionComplete;}
-(void)setDataCollectionComplete:(BOOL)flag
{	dataCollectionComplete = flag;}

-(BOOL)capacityPlanningCompleted
{return capacityPlanningCompleted;}
-(void)setCapacityPlanningCompleted:(BOOL)flag
{ capacityPlanningCompleted = flag;}
	
#pragma mark "Data Collection"
-(void) collectDataForDate:(NSDate *)refDate andPeriod:(int)dataPeriod andCalculateDeltaValues:(BOOL)calcDelta
{
	[self setReferenceDate:refDate];
	[self setDataPeriod:dataPeriod];
	[self setCalcDeltaValues:calcDelta];
	
	availability = [LCReportMetric reportMetricWithEntity:[[[[[[device childrenDictionary] valueForKey:@"avail"] childrenDictionary] valueForKey:@"master"] childrenDictionary] valueForKey:@"ok_pc"]
												andParent:self];
	[availability setUnits:@"%"];
		
	[outstandingEntities addObject:availability];
	
	[availability addObserver:self 
					forKeyPath:@"refreshComplete" 
					   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
					   context:nil];
	
	[availability collectReportDataForDate:referenceDate withDataPeriod:dataPeriod andCalculateDeltaValues:calcDeltaValues];

	LCReportMetric *metric;
	for (metric in metrics)
	{
		[outstandingEntities addObject:metric];
		
		[metric addObserver:self 
				 forKeyPath:@"refreshComplete" 
				    options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
					context:nil];
		
		[metric collectReportDataForDate:referenceDate
						  withDataPeriod:dataPeriod 
				 andCalculateDeltaValues:calcDeltaValues];
	}
	
	LCReportInterface *interface;
	for (interface in interfaces)
	{
		[outstandingInterfaces addObject:interface];
		
		[interface addObserver:self 
					forKeyPath:@"refreshComplete" 
					  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
					  context:nil];
		
		
		[interface collectReportDataForDate:referenceDate 
							 withDataPeriod:dataPeriod
					andCalculateDeltaValues:calcDeltaValues];
	}
}

-(void) interfaceDataCollectionCompleted:(id)iface
{
	[outstandingInterfaces removeObject: iface];
	
	if ([outstandingInterfaces count] == 0 && [outstandingEntities count] == 0)
	{
		[self setDataCollectionComplete:YES];
	}
}
-(void) metricDataCollectionCompleted:(id)metric
{
	[outstandingEntities removeObject:metric];
	
	if ([outstandingEntities count] == 0 && [outstandingInterfaces count] == 0)
	{
		[self setDataCollectionComplete:YES];
	}
}
	

#pragma mark "Capacity Planning"
-(void) collectCapacityPlanningDataForDate:(NSDate *)refDate andPeriod:(int)dataPeriod
{
	LCReportMetric *metric;
	
	for (metric in metrics)
	{
		[outstandingEntities addObject:metric];
		
		[metric collectCapacityPlanningDataForDate:referenceDate withDataPeriod:dataPeriod];
	}
	
	LCReportInterface *interface;
	
	for (interface in interfaces)
	{
		[outstandingInterfaces addObject:interface];
		
		[interface collectCapacityPlanningDataForDate:referenceDate withDataPeriod:dataPeriod];
	}
}

-(void) interfaceCapacityPlanningComplete:(id)iface
{
	[outstandingInterfaces removeObject:iface];
	
	if ([outstandingInterfaces count]==0 && [outstandingEntities count] == 0)
	{
		[self setCapacityPlanningCompleted:YES];
	}
}

-(void) metricCapacityPlanningComplete:(id)metric
{
	[outstandingEntities removeObject:metric];
	
	if([outstandingEntities count] == 0 && [outstandingInterfaces count] == 0)
	{
		[self setCapacityPlanningCompleted:YES];
	}
}

-(void) calculateCapacityPlanningForMetric:(LCEntity *) entity withCriticalDate:(NSDate *)criticalDate
{
	int days = [criticalDate timeIntervalSinceNow] / 86400;

	// 3 Months will cover between 0 mths and 5 mths
	if (days > 0 && days < 150)
	{
		[capacityPlanning3mth addObject:[LCReportCapacityPlanning ReportCapacityPlanningWithEntity:entity andPeriod:3]];
	}
	//6 Months will cover between 5 mths and 8 mths
	else if (days >= 150 && days < 240)
	{		
		[capacityPlanning6mth addObject:[LCReportCapacityPlanning ReportCapacityPlanningWithEntity:entity andPeriod:6]];
	}
	//9 Months will cover between 8 mths and 11 mths
	else if (days >= 240 && days < 330)
	{
		[capacityPlanning9mth addObject:[LCReportCapacityPlanning ReportCapacityPlanningWithEntity:entity andPeriod:9]];
	}	
	//12 Months will cover between 11mths and 13 mths
	else if (days >= 330 && days < 390)
	{
		[capacityPlanning12mth addObject:[LCReportCapacityPlanning ReportCapacityPlanningWithEntity:entity andPeriod:12]];
	}	
}

#pragma mark "Misc"
-(void) cancelRefresh
{
	int i;
	
	for (i = 0; i < [outstandingEntities count]; )
	{
		[[outstandingEntities objectAtIndex:i] cancelRefresh];
	}
	for (i=0; i < [outstandingInterfaces count]; )
	{
		[[outstandingInterfaces objectAtIndex:i] cancelRefresh];
	}
}


/***********************************UP TO HERE***********************/
/*

-(void) calculateCapacityPlanningForMetric:(LCEntity *) entity withCriticalDate:(NSDate *)criticalDate;

*/

@synthesize availability;
@synthesize outstandingEntities;
@synthesize outstandingInterfaces;
@end
