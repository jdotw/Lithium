//
//  MBRateMetric.m
//  ModuleBuilder
//
//  Created by James Wilson on 22/02/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MBRateMetric.h"

#import "MBRateMetricViewController.h"
#import "MBContainer.h"
#import "ModuleDocument.h"

@implementation MBRateMetric

#pragma mark "Constructors"

- (id) init
{
	[super init];
	[self setRecordMethod:[NSNumber numberWithInt:1]];
	return self;
}

- (MBRateMetric *) initWithCoder:(NSCoder *)decoder
{
	[super initWithCoder:decoder];
	
	/* Counter Metric */
	NSString *counterMetricContainerDesc = [decoder decodeObjectForKey:@"counterMetricContainerDesc"];
	if (counterMetricContainerDesc) [properties setObject:counterMetricContainerDesc forKey:@"counterMetricContainerDesc"];
	NSString *counterMetricDesc = [decoder decodeObjectForKey:@"counterMetricDesc"];
	if (counterMetricDesc) [properties setObject:counterMetricDesc forKey:@"counterMetricDesc"];
	
	return self;
}

- (void) encodeWithCoder:(NSCoder *)encoder
{
	[super encodeWithCoder:encoder];
	
	[encoder encodeObject:[[counterMetric parent] desc] forKey:@"counterMetricContainerDesc"];
	[encoder encodeObject:[counterMetric desc] forKey:@"counterMetricDesc"];
}

#pragma mark "View Controller"

- (NSViewController *) viewController
{ 
	return [MBRateMetricViewController viewForMetric:self]; 
}

- (float) rowHeight
{ 
	return 63.0;
}

#pragma mark "Dependent Metric Binding"

- (void) bindDependents
{
	bindingDependents = YES;
	
	/* Counter Metric */
	ModuleDocument *moduleDocument = [[self parent] document];
	NSString *counterMetricContainerDesc = [properties objectForKey:@"counterMetricContainerDesc"];
	NSString *counterMetricDesc = [properties objectForKey:@"counterMetricDesc"];
	if (counterMetricContainerDesc && counterMetricDesc)
	{
		MBContainer *counterContainer = nil;
		for (MBContainer *container in [moduleDocument containers])
		{
			if ([[container desc] isEqualToString:counterMetricContainerDesc])
			{
				counterContainer = container;
				break;
			}
		}
		if (counterContainer)
		{
			for (MBMetric *metric in [counterContainer children])
			{ 
				if ([[metric desc] isEqualToString:counterMetricDesc])
				{
					self.counterMetric = metric;
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
	NSXMLElement *rootnode = (NSXMLElement *) [super xmlNode];
	
	[rootnode addChild:[NSXMLNode elementWithName:@"met_type" stringValue:@"1"]];
	[rootnode addChild:[NSXMLNode elementWithName:@"countermet_desc" stringValue:[counterMetric desc]]];
	
	return rootnode;
}


#pragma mark "Properties"

@synthesize counterMetric;
- (void) setCounterMetric:(MBMetric *)value
{
	if (!bindingDependents) [[[document undoManager] prepareWithInvocationTarget:self] setCounterMetric:(id)[self counterMetric]];
	counterMetric = value;
	if (!bindingDependents) [[document undoManager] setActionName:@"Set Counter Metric"];
}

- (BOOL) convertBytesToBits
{ return [[properties objectForKey:@"convertBytesToBits"] boolValue]; }

- (void) setConvertBytesToBits:(BOOL)value
{
	[[[document undoManager] prepareWithInvocationTarget:self] setConvertBytesToBits:[self convertBytesToBits]];
	[properties setObject:[NSNumber numberWithBool:value] forKey:@"convertBytesToBits"];
	[[document undoManager] setActionName:@"Set Byte to Bit Conversion"];
}	

@end
