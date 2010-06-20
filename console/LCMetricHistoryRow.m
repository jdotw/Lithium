//
//  LCMetricHistoryRow.m
//  Lithium Console
//
//  Created by James Wilson on 18/07/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMetricHistoryRow.h"


@implementation LCMetricHistoryRow

#pragma mark "Initialisation"

+ (LCMetricHistoryRow *) row
{
	return [[[LCMetricHistoryRow alloc] init] autorelease];
}

- (id) init
{
	self = [super init];
	if (!self) return nil;
	
	return self;
}

- (void) dealloc
{
	[timeStamp release];
	[super dealloc];
}

#pragma mark Accessors

@synthesize timeStamp;
@synthesize minimum;
- (NSNumber *) minimumNumber
{ 
	return [NSNumber numberWithFloat:minimum]; 
}
@synthesize average;
- (NSNumber *) averageNumber
{ 
	return [NSNumber numberWithFloat:average]; 
}

@synthesize maximum;
- (NSNumber *) maximumNumber
{
	return [NSNumber numberWithFloat:maximum]; 
}

@end
