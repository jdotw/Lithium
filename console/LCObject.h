//
//  LCObject.h
//  Lithium Console
//
//  Created by James Wilson on 27/09/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"

#import "LCMetric.h"

@interface LCObject : LCEntity 
{
}

- (LCObject *) init;
- (Class) childClass;

#pragma mark "Object Entity Methods"
- (NSString *) valueForMetricNamed:(NSString *)metricName;
- (NSString *) rawValueForMetricNamed:(NSString *)metricName;

#pragma mark "Aggregate Delta"
@property (readonly) float highestAggregateDelta;

@end
