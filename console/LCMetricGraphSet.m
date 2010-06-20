//
//  LCMetricGraphSet.m
//  Lithium Console
//
//  Created by James Wilson on 6/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMetricGraphSet.h"
#import "LCMetricGraphMetricItem.h"

@implementation LCMetricGraphSet

#pragma mark "Initialisation"

+ (LCMetricGraphSet *) graphSet
{
	return [[[LCMetricGraphSet alloc] init] autorelease];
}

- (LCMetricGraphSet *) initWithCoder:(NSCoder *)decoder
{
	[super init];

	properties = [[decoder decodeObjectForKey:@"properties"] retain];
	
	NSArray *storedMetrics = [decoder decodeObjectForKey:@"metrics"];
	metrics = [[NSMutableArray array] retain];
	for (id obj in storedMetrics)
	{
		if ([[obj class] isSubclassOfClass:[LCEntity class]])
		{
			/* Importing old-style document */
			LCMetricGraphMetricItem *graphItem = [[LCMetricGraphSet new] autorelease];
			[metrics addObject:graphItem]; 
		}
		else 
		{
			[metrics addObject:obj];
		}
	}
			
	devices = [[decoder decodeObjectForKey:@"devices"] retain];
	
    return self;
}

- (LCMetricGraphSet *) init
{
	[super init];
	
	properties = [[NSMutableDictionary dictionary] retain];
	metrics = [[NSMutableArray array] retain];
	devices = [[NSMutableArray array] retain];

	return self;
}

- (void) dealloc
{
	[metrics release];
	[devices release];
	[super dealloc];
}

#pragma mark "Encoding"

- (void)encodeWithCoder:(NSCoder *)encoder
{
	[encoder encodeObject:properties forKey:@"properties"];
	[encoder encodeObject:metrics forKey:@"metrics"];
	[encoder encodeObject:devices forKey:@"devices"];
}

#pragma mark "Accessor Methods"

@synthesize metrics;
@synthesize devices;
@synthesize properties;

- (NSString *) desc
{ return [properties objectForKey:@"desc"]; }

- (void) setDesc:(NSString *)value
{ 
	if (value) [properties setObject:[[value copy] autorelease] forKey:@"desc"]; 
	else [properties removeObjectForKey:@"desc"]; 
}

@end
