//
//  LCObject.m
//  Lithium Console
//
//  Created by James Wilson on 27/09/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCObject.h"
#import "LCMetric.h"
#import "LCContainer.h"

@implementation LCObject

- (LCObject *) init
{
	[super init];
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

- (Class) childClass
{ return [LCMetric class]; }

- (void) setDesc:(NSString *)string
{
	if (![string isEqualToString:desc])
	{
		[desc release];
		desc = [string copy];

		self.displayString = self.desc;
	}
}

- (NSString *) displayString
{
	if ([self.name isEqualToString:@"master"])
	{ return [self.parent displayString]; }
	else
	{ return [super displayString]; }
}

#pragma mark "Object Entity Methods"

- (NSString *) valueForMetricNamed:(NSString *)metricName
{
	return [[(LCMetric *)[self childNamed:metricName] currentValue] valueString];
}

- (NSString *) rawValueForMetricNamed:(NSString *)metricName
{
	return [[(LCMetric *)[self childNamed:metricName] currentValue] rawValueString];
}

#pragma mark "Aggregate Delta"

- (float) highestAggregateDelta
{
	float highestAggregateDelta = 0.0;
	for (LCMetric *metric in children)
	{ if (metric.aggregateDelta > highestAggregateDelta) highestAggregateDelta = metric.aggregateDelta; }
	return highestAggregateDelta;
}

@end
