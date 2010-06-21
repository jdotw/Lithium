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


@property (nonatomic, assign) int metricType;
@property (nonatomic, assign) int kBase;
@property (nonatomic, assign) int recordMethod;
@property (nonatomic, assign) BOOL recordEnabled;
@property (nonatomic, assign) unsigned long allocationUnits;
@property (nonatomic,copy) NSString *units;
@property (nonatomic, assign) float maximumValue;
@property (nonatomic, assign) float minimumValue;
@property (nonatomic, assign) BOOL hasValueRange;
@property (nonatomic, assign) BOOL showInSummary;
@property (nonatomic, assign) float aggregateDelta;
@property (nonatomic, assign) BOOL hasTriggers;

@property (nonatomic, assign) unsigned long lastRefreshTime;
@property (nonatomic, assign) unsigned long lastNormalTime;
@property (nonatomic, assign) unsigned long lastValueChangeTime;
@property (nonatomic, assign) unsigned long lastStateChangeTime;

@property (nonatomic,retain) LCMetricValue *currentValue;
@property (nonatomic, assign) int maximumValueCount;
@property (readonly) NSMutableArray *metricValues;

@property (readonly) BOOL isGraphable;

- (void) insertObject:(LCMetricValue *)obj inMetricValuesAtIndex:(unsigned int)index;
- (void) removeObjectFromMetricValuesAtIndex:(unsigned int)index;

@property (readonly) NSString *metricLongDisplayString;


@end
