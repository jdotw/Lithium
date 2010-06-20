//
//  LCMetric.m
//  Lithium Console
//
//  Created by James Wilson on 27/09/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMetric.h"

#import "LCTrigger.h"

@implementation LCMetric

- (LCMetric *) init
{
	[super init];
	
	metricValues = [[NSMutableArray array] retain];
	metricValueDict = [[NSMutableDictionary dictionary] retain];
	
	[self.xmlTranslation setObject:@"units" forKey:@"units"];
	[self.xmlTranslation setObject:@"metricType" forKey:@"met_type_int"];
	[self.xmlTranslation setObject:@"kBase" forKey:@"kbase"];
	[self.xmlTranslation setObject:@"recordMethod" forKey:@"record_method"];
	[self.xmlTranslation setObject:@"recordEnabled" forKey:@"record_enabled"];
	[self.xmlTranslation setObject:@"lastRefreshTime" forKey:@"lastrefresh_sec"];
	[self.xmlTranslation setObject:@"lastNormalTime" forKey:@"lastnormal_sec"];
	[self.xmlTranslation setObject:@"lastValueChangeTime" forKey:@"lastvalchange_sec"];
	[self.xmlTranslation setObject:@"lastStateChangeTime" forKey:@"laststatechange_sec"];
	[self.xmlTranslation setObject:@"maximumValueCount" forKey:@"val_list_maxsize"];
	[self.xmlTranslation setObject:@"allocationUnits" forKey:@"alloc_unit"];
	[self.xmlTranslation setObject:@"minimumValue" forKey:@"min_valstr"];
	[self.xmlTranslation setObject:@"maximumValue" forKey:@"max_valstr"];
	[self.xmlTranslation setObject:@"showInSummary" forKey:@"summary_flag"];
	[self.xmlTranslation setObject:@"aggregateDelta" forKey:@"aggregate_delta"];
	
	return self;
}

- (void) dealloc
{
	[metricValues release];
	[metricValueDict release];
	[units release];
	[currentValue release];
	[super dealloc];
}

- (void) updateEntityUsingXMLNode:(LCXMLNode *)node
{
	[super updateEntityUsingXMLNode:node];
	
	for (LCXMLNode *childNode in node.children)
	{
		if ([childNode.name isEqualToString:@"value"])
		{
			/* Parse Value */
			LCMetricValue *value = [[LCMetricValue alloc] initWithXmlNode:childNode];
			
			/* Check for existence */
			if (![metricValueDict objectForKey:[NSNumber numberWithUnsignedLong:value.timestamp]])
			{
				/* Enqueue value */
				[self insertObject:value inMetricValuesAtIndex:0];
			}
			[value autorelease];		
		}
	}
}

- (Class) childClass
{ return [LCTrigger class]; }

#pragma mark "Graph Methods"

- (BOOL) isGraphable
{
	if (self.recordMethod == 1 && self.recordEnabled)
	{ return YES; }
	else
	{ return NO; }
}

#pragma mark "Properties"

@synthesize metricType;
@synthesize kBase;
@synthesize recordMethod;
@synthesize recordEnabled;
@synthesize allocationUnits;
@synthesize units;

@synthesize minimumValue;
@synthesize maximumValue;
- (void) setMaximumValue:(float)value
{
	maximumValue = value;
	self.hasValueRange = YES;
}
@synthesize hasValueRange;

@synthesize lastRefreshTime;
@synthesize lastNormalTime;
@synthesize lastValueChangeTime;
@synthesize lastStateChangeTime;

@synthesize currentValue;

- (void) setCurrentValue:(LCMetricValue *)value
{
	[currentValue release];
	currentValue = [value retain];
	
	self.displayValue = [currentValue valueString];
}

@synthesize maximumValueCount;
@synthesize metricValues;

- (void) setMetricValues:(NSMutableArray *)array
{
	[metricValues release];
	metricValues = [array mutableCopy];
}

- (void) insertObject:(LCMetricValue *)obj inMetricValuesAtIndex:(unsigned int)index
{
	[metricValues insertObject:obj atIndex:index];
	[metricValueDict setObject:obj forKey:[NSNumber numberWithUnsignedLong:obj.timestamp]];
	self.currentValue = obj;
	
	while ([metricValues count] > self.maximumValueCount)
	{ 
		[self removeObjectFromMetricValuesAtIndex:([metricValues count]-1)]; 
	}	
}

- (void) removeObjectFromMetricValuesAtIndex:(unsigned int)index
{
	[metricValueDict removeObjectForKey:[NSNumber numberWithUnsignedLong:((LCMetricValue *)[metricValues objectAtIndex:index]).timestamp]];
	[metricValues removeObjectAtIndex:index];
}

- (void) insertObject:(LCEntity *)entity inChildrenAtIndex:(unsigned int)index
{
	[super insertObject:entity inChildrenAtIndex:index];
	if (!self.hasTriggers) self.hasTriggers = YES;
}

- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{
	[super removeObjectFromChildrenAtIndex:index];
	if (children.count < 1 && self.hasTriggers) self.hasTriggers = NO;
}

@synthesize showInSummary;
@synthesize aggregateDelta;
@synthesize hasTriggers;

- (NSString *) metricLongDisplayString
{
	return [NSString stringWithFormat:@"%@ %@ %@", [self.container desc], [self.object desc], self.desc];
}

@end
