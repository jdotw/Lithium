//
//  LCAnalysisTrigger.m
//  Lithium Console
//
//  Created by Liam Elliott on 29/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCAnalysisTrigger.h"

@implementation LCAnalysisTrigger

+ (id) analysisTriggerWithEntity:(LCEntity *)entity
{
	return [[[LCAnalysisTrigger alloc] initWithEntity:entity] autorelease];
}

-(LCAnalysisTrigger *) initWithEntity:(LCEntity *)entity
{
	[super init];

	self.baseEntity = entity;
	
	return self;
}

- (void) dealloc
{
	[minDate release];
	[avgDate release];
	[maxDate release];
	[baseEntity release];
	[super dealloc];
}

@synthesize minGradient;
@synthesize minIntercept;
@synthesize avgGradient;
@synthesize avgIntercept;
@synthesize maxGradient;
@synthesize maxIntercept;

-(double)xTriggerValue
{
	return [[[baseEntity properties] valueForKey:@"xval"] doubleValue];
}

-(double)yTriggerValue
{
	return [[[baseEntity properties] valueForKey:@"yval"] doubleValue];
}

@synthesize minDate;
@synthesize avgDate;
@synthesize maxDate;

@synthesize baseEntity;
-(id)entity
{ return baseEntity; }

-(id)triggers
{ return nil; }

-(BOOL)isLeaf
{ return YES; }

@end
