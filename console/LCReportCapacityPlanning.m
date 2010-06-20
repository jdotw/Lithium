//
//  LCReportCapacityPlanning.m
//  Lithium Console
//
//  Created by Liam Elliott on 23/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCReportCapacityPlanning.h"


@implementation LCReportCapacityPlanning


#pragma mark "Initilisation"
+(LCReportCapacityPlanning *)ReportCapacityPlanningWithEntity:(LCEntity *)initEntity andPeriod:(int)value
{
	LCReportCapacityPlanning *temp = [[[LCReportCapacityPlanning alloc]initWithEntity:initEntity]autorelease];
	[temp setPeriod:value];
	
	return temp;
}

-(LCReportCapacityPlanning *)initWithEntity:(LCEntity *)initEntity
{
	[super init];
	baseEntity = initEntity;
	
	NSString *desc = [[[baseEntity container] desc] lowercaseString];
	
	if ([desc rangeOfString:"memory"].location != NSNotFound)
	{	
		text = [NSString stringWithString:"Chassis Upgrade (Memory Utilisation)"];
	}
	else if ([desc rangeOfString:"interface"].location != NSNotFound)
	{
		text = [NSString stringWithString:"Chassis Upgrade (Interface Utilisation)"];
	}
	else if ([desc rangeOfString:"cpu"].location != NSNotFound)
	{
		text = [NSString stringWithString:"Chassis Upgrade (CPU Utilisation)"];
	}
	
	includeInReport = YES;
	return self;
}

-(void)dealloc
{
	[baseEntity release];
	
	[super dealloc];
}

#pragma mark "Accessors"
-(LCEntity *)baseEntity
{ return baseEntity;}

-(int)period
{return period;}
-(void)setPeriod:(int)value
{period = value;}

-(NSString *)description
{return text;}
-(void)setDescription:(NSString *)value
{
	text = value;	
	[self setMasterDescription:[NSString stringWithFormat:@"%@: %@",[[baseEntity device] desc], value]];
}

-(NSString *)masterDescription
{return masterText;}
-(void)setMasterDescription:(NSString *)value
{ masterText = value;}
	
-(NSString *)device
{ return [[baseEntity device] desc];}
	
-(NSString *)object
{	return [NSString stringWithFormat:@"%@ %@",[[baseEntity container] desc],[[baseEntity object] desc]];}

-(BOOL)includeInReport
{ return includeInReport;}
	
-(void)setIncludeInReport:(BOOL)flag
{includeInReport = flag;}

@synthesize baseEntity;
@end
