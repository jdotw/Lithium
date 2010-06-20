//
//  LCReportEntity.m
//  Lithium Console
//
//  Created by Liam Elliott on 16/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCReportEntity.h"
#import "LCEntity.h"
#import "LCReportValue.h"



@implementation LCReportEntity

#pragma mark "Initialisation"
+(LCReportEntity *)reportEntityWithEntity:(LCEntity *)newEntity andParent:(id *)newParent
{
	return [[[LCReportEntity alloc] initWithEntity:newEntity andParent:newParent]autorelease];
}
	
-(LCReportEntity *)initWithEntity:(LCEntity *)newEntity andParent:(id *)newParent
{
	[self init];
	baseEntity = newEntity;
	parent = newParent;
	
	return self;
}
	
-(LCReportEntity *)init
{
	[super init];
	
	minimum = [[[LCReportValue alloc]init]retain];
	average = [[[LCReportValue alloc]init]retain];
	maximum = [[[LCReportValue alloc]init]retain];
	
	reportDescription = nil;
	parent = nil;
	
	referenceDate = [NSDate date];
	dataPeriod = 5;
	
	return self;
}

-(void) dealloc
{
	[minimum release];
	[average release];
	[maximum release];
	[referenceDate release];
	
	[super dealloc];
}

#pragma mark "Accessors"

-(LCEntity *) baseEntity
{ return baseEntity;}

-(void) setBaseEntity:(LCEntity *)entity
{ baseEntity = entity;}


-(NSString *)entityName
{
	return [NSString stringWithFormat:@"%@ %@ %@",
					[[baseEntity container] desc], 
					[[baseEntity object] desc],
					[baseEntity desc]];
}

-(NSString *) reportDescription
{ return reportDescription;}

-(void)setReportDescription:(NSString *)newDescription
{ reportDescription = newDescription;}

-(LCReportValue *) minimum
{ return minimum; }
-(void) setMinimum:(LCReportValue *)newMinimum
{minimum = newMinimum;}

-(LCReportValue *) average
{ return average;}
-(void) setAverage:(LCReportValue *)newAverage
{ average = newAverage;}

-(LCReportValue *) maximum
{return maximum;}
-(void) setMaximum:(LCReportValue *)newMaximum
{ maximum = newMaximum;}

-(NSDate *)criticalDate
{ return criticalDate;}
-(void) setCriticalDate:(NSDate *)newDate
{ criticalDate = newDate;}

-(id *) parent
{return parent;}
-(void) setParent:(id *)newParent
{ parent = newParent;}

-(NSDate *)referenceDate
{ return referenceDate;}
-(void) setReferenceDate:(NSDate *)newDate
{ referenceDate = newDate;}

-(int) dataPeriod
{ return dataPeriod;}
-(void) setDataPeriod:(int) newDataPeriod
{ dataPeriod = newDataPeriod; }

-(NSString *) units
{ 
	if (minimum)
	{
		return [minimum units];
	}
	return nil;
}

-(void) setUnits:(NSString *)newUnits
{
	if (minimum)
	{
		[minimum setUnits:newUnits];
		[average setUnits:newUnits];
		[maximum setUnits:newUnits];
	}
}
	
@end