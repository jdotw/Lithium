//
//  LCMetric.h
//  Lithium Console
//
//  Created by James Wilson on 27/09/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCMetricValue.h"

@interface LCMetric : LCEntity 
{
	/*
	 * Properties
	 */
	 
	/* Metric Properties */
	int metricType;
	int kBase;
	int recordMethod;
	BOOL recordEnabled;
	unsigned long allocationUnits;
	NSString *units;
	float maximumValue;
	float minimumValue;
	BOOL hasValueRange;
	BOOL showInSummary;
	float aggregateDelta;
	BOOL hasTriggers;
	
	/* Time Variables */
	unsigned long lastRefreshTime;
	unsigned long lastNormalTime;
	unsigned long lastValueChangeTime;
	unsigned long lastStateChangeTime;

	/* Metric Value Properties */
	int maximumValueCount;
	NSMutableArray *metricValues;
	NSMutableDictionary *metricValueDict;
	LCMetricValue *currentValue;

	/*
	 * Internal variables
	 */
	
	BOOL inValueXML;
	LCMetricValue *curXmlValue;	
}

- (LCMetric *) init;
- (Class) childClass;


@property (assign) int metricType;
@property (assign) int kBase;
@property (assign) int recordMethod;
@property (assign) BOOL recordEnabled;
@property (assign) unsigned long allocationUnits;
@property (copy) NSString *units;
@property (assign) float maximumValue;
@property (assign) float minimumValue;
@property (assign) BOOL hasValueRange;
@property (assign) BOOL showInSummary;
@property (assign) float aggregateDelta;
@property (assign) BOOL hasTriggers;

@property (assign) unsigned long lastRefreshTime;
@property (assign) unsigned long lastNormalTime;
@property (assign) unsigned long lastValueChangeTime;
@property (assign) unsigned long lastStateChangeTime;

@property (retain) LCMetricValue *currentValue;
@property (assign) int maximumValueCount;
@property (readonly) NSMutableArray *metricValues;

@property (readonly) BOOL isGraphable;

- (void) insertObject:(LCMetricValue *)obj inMetricValuesAtIndex:(unsigned int)index;
- (void) removeObjectFromMetricValuesAtIndex:(unsigned int)index;

@property (readonly) NSString *metricLongDisplayString;


@end
