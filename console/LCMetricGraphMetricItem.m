//
//  LCMetricGraphMetricItem.m
//  Lithium Console
//
//  Created by James Wilson on 23/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCMetricGraphMetricItem.h"

#import "LCMetric.h"
#import "LCMetricHistory.h"

@implementation LCMetricGraphMetricItem

+ (LCMetricGraphMetricItem *) itemForMetric:(LCMetric *)initMetric
{
	return [[[LCMetricGraphMetricItem alloc] initWithMetric:initMetric] autorelease];
}

- (id) initWithMetric:(LCMetric *)initMetric
{
	self = [self init];
	if (!self) return nil;
	
	self.metric = initMetric;
	
	return self;
}

- (LCMetricGraphMetricItem *) initWithCoder:(NSCoder *)decoder
{
	[self init];
	
	metric = [[decoder decodeObjectForKey:@"metric"] retain];
	color = [[decoder decodeObjectForKey:@"color"] retain];
	
    return self;
}

- (void) dealloc
{
	[metric release];
	[history release];
	[color release];
	[super dealloc];
}

#pragma mark "Encoding"

- (void)encodeWithCoder:(NSCoder *)encoder
{
	[encoder encodeObject:color forKey:@"color"];
	[encoder encodeObject:metric forKey:@"metric"];
}

@synthesize metric;
- (void) setMetric:(LCMetric *)value
{
	[metric release];
	metric = [value retain];
	
	self.history = [LCMetricHistory historyWithMetric:metric];
}
@synthesize history;
@synthesize color;

@end
