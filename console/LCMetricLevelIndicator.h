//
//  LCMetricLevelIndicator.h
//  Lithium Console
//
//  Created by James Wilson on 15/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCMetric.h"

@interface LCMetricLevelIndicator : NSLevelIndicator
{
	NSTrackingRectTag trackTag;
	LCMetric *metric;
}

@property (assign) NSTrackingRectTag trackTag;
@property (retain) LCMetric *metric;

@end
