//
//  MBMetric.h
//  ModuleBuilder
//
//  Created by James Wilson on 12/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MBEntity.h"
#import "MBOid.h"

/* Metric types -- from induction/src/metric.h */
#define METRIC_INTEGER 1                /* Signed 32bit integer value */
#define METRIC_COUNT 2                  /* Unsigned 32bit integer wrapping value */
#define METRIC_GAUGE 3                  /* Unsigned 32bit integer non-wrapping value */
#define METRIC_FLOAT 4                  /* Floating point value */
#define METRIC_STRING 5                 /* NULL terminated string value */
#define METRIC_OID 6                    /* OID value */
#define METRIC_IP 7                     /* IP Address */
#define METRIC_INTERVAL 8               /* A time interval in seconds */
#define METRIC_HEXSTRING 9              /* A hex string in XX:XX:XX:XX format */
#define METRIC_COUNT_HEX64 10           /* Unsigned 64bit integer wrapping value derived from hex string */
#define METRIC_GAUGE_HEX64 11           /* Unsigned 64bit integer non-wrapping value derived from hex string */
#define METRIC_COUNT64 12				/* Unsigned 64bit integer wrapping value */
#define METRIC_DATA 100                 /* Opaque data value */

@interface MBMetric : MBEntity 
{
	BOOL webViewFlag;
	NSString *oidDisplayString;
}

#pragma mark Constructors
+ (id) metricWithOid:(MBOid *)oid;
+ (id) metric;
- (id) initWithOid:(MBOid *)oid;
- (id) initWithProperties:(NSMutableDictionary *)initProperties;
- (id) init;

#pragma mark Accessors
- (int) valType;
- (MBOid *) oid;
- (void) setOid:(MBOid *)oid;
- (void) updateOidDisplayString;
- (NSNumber *) recordMethod;
- (void) setRecordMethod:(NSNumber *)value;
- (NSString *) maximumValue;
- (void) setMaximumValue:(NSString *)value;
- (BOOL) webViewFlag;
- (void) setWebViewFlag:(BOOL)flag;
- (NSString *) units;
- (void) setUnits:(NSString *)value;
- (NSNumber *) multiplier;
- (void) setMultiplier:(NSNumber *)value;
- (BOOL) showInSummary;
- (void) setShowInSummary:(BOOL)flag;
@property (copy) NSString *oidDisplayString;
@property (copy) NSMutableArray *enumerators;
- (void) insertObject:(id)obj inEnumeratorsAtIndex:(unsigned int)index;
- (void) removeObjectFromEnumeratorsAtIndex:(unsigned int)index;

@end
