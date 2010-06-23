//
//  MBPercentMetric.m
//  ModuleBuilder
//
//  Created by James Wilson on 22/02/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MBPercentMetric.h"

#import "MBPercentMetricViewController.h"
#import "MBContainer.h"
#import "ModuleDocument.h"

@implementation MBPercentMetric

#pragma mark "Constructors"

- (id) init
{
	[super init];
	[self setRecordMethod:[NSNumber numberWithInt:1]];
	return self;
}

- (MBPercentMetric *) initWithCoder:(NSCoder *)decoder
{
	[super initWithCoder:decoder];
	
	/* Gauge Metric */
	NSString *gaugeMetricContainerDesc = [decoder decodeObjectForKey:@"gaugeMetricContainerDesc"];
	if (gaugeMetricContainerDesc) [properties setObject:gaugeMetricContainerDesc forKey:@"gaugeMetricContainerDesc"];
	NSString *gaugeMetricDesc = [decoder decodeObjectForKey:@"gaugeMetricDesc"];
	if (gaugeMetricDesc) [properties setObject:gaugeMetricDesc forKey:@"gaugeMetricDesc"];
	
	/* Max Metric */
	NSString *maxMetricContainerDesc = [decoder decodeObjectForKey:@"maxMetricContainerDesc"];
	if (maxMetricContainerDesc) [properties setObject:maxMetricContainerDesc forKey:@"maxMetricContainerDesc"];
	NSString *maxMetricDesc = [decoder decodeObjectForKey:@"maxMetricDesc"];
	if (maxMetricDesc) [properties setObject:maxMetricDesc forKey:@"maxMetricDesc"];
	
	return self;
}

- (void) encodeWithCoder:(NSCoder *)encoder
{
	[super encodeWithCoder:encoder];
	
	[encoder encodeObject:[[gaugeMetric parent] desc] forKey:@"gaugeMetricContainerDesc"];
	[encoder encodeObject:[gaugeMetric desc] forKey:@"gaugeMetricDesc"];
	[encoder encodeObject:[[maxMetric parent] desc] forKey:@"maxMetricContainerDesc"];
	[encoder encodeObject:[maxMetric desc] forKey:@"maxMetricDesc"];
}

#pragma mark "View Controller"

- (NSViewController *) viewController
{ 
	return [MBPercentMetricViewController viewForMetric:self]; 
}

- (float) rowHeight
{ 
	return 88.0;
}

#pragma mark "Dependent Metric Binding"

- (void) bindDependents
{
	bindingDependents = YES;
	
	/* Gauge Metric */
	ModuleDocument *moduleDocument = [[self parent] document];
	NSString *gaugeMetricContainerDesc = [properties objectForKey:@"gaugeMetricContainerDesc"];
	NSString *gaugeMetricDesc = [properties objectForKey:@"gaugeMetricDesc"];
	if (gaugeMetricContainerDesc && gaugeMetricDesc)
	{
		MBContainer *gaugeContainer = nil;
		for (MBContainer *container in [moduleDocument containers])
		{
			if ([[container desc] isEqualToString:gaugeMetricContainerDesc])
			{
				gaugeContainer = container;
				break;
			}
		}
		if (gaugeContainer)
		{
			for (MBMetric *metric in [gaugeContainer children])
			{ 
				if ([[metric desc] isEqualToString:gaugeMetricDesc])
				{
					self.gaugeMetric = metric;
				}
			}
		}
	}

	/* Max Metric */
	NSString *maxMetricContainerDesc = [properties objectForKey:@"maxMetricContainerDesc"];
	NSString *maxMetricDesc = [properties objectForKey:@"maxMetricDesc"];
	if (maxMetricContainerDesc && maxMetricDesc)
	{
		MBContainer *maxContainer = nil;
		for (MBContainer *container in [moduleDocument containers])
		{
			if ([[container desc] isEqualToString:maxMetricContainerDesc])
			{
				maxContainer = container;
				break;
			}
		}
		if (maxContainer)
		{
			for (MBMetric *metric in [maxContainer children])
			{ 
				if ([[metric desc] isEqualToString:maxMetricDesc])
				{
					self.maxMetric = metric;
				}
			}
		}
	}
	
	bindingDependents = NO;
}

#pragma mark "XML Output"

- (NSXMLNode *) xmlNode
{
	/* Create our root node */
	NSXMLElement *rootnode = (NSXMLElement *)[super xmlNode];
	
	[rootnode addChild:[NSXMLNode elementWithName:@"met_type" stringValue:@"2"]];
	[rootnode addChild:[NSXMLNode elementWithName:@"gaugemet_desc" stringValue:[gaugeMetric desc]]];
	[rootnode addChild:[NSXMLNode elementWithName:@"maxmet_desc" stringValue:[maxMetric desc]]];
	
	return rootnode;
}

#pragma mark "Properties"

@synthesize gaugeMetric;
- (void) setGaugeMetric:(MBMetric *)value
{
	if (!bindingDependents) [[[document undoManager] prepareWithInvocationTarget:self] setGaugeMetric:(id)[self gaugeMetric]];
	gaugeMetric = value;
	if (!bindingDependents) [[document undoManager] setActionName:@"Set Gauge Metric"];	
}

@synthesize maxMetric;
- (void) setMaxMetric:(MBMetric *)value
{
	if (!bindingDependents) [[[document undoManager] prepareWithInvocationTarget:self] setMaxMetric:(id)[self maxMetric]];
	maxMetric = value;
	if (!bindingDependents) [[document undoManager] setActionName:@"Set Maximum Metric"];		
}

@end
