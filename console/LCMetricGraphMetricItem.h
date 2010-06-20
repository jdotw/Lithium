//
//  LCMetricGraphMetricItem.h
//  Lithium Console
//
//  Created by James Wilson on 23/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class LCMetric, LCMetricHistory;

@interface LCMetricGraphMetricItem : NSObject 
{
	LCMetric *metric;
	LCMetricHistory *history;
	NSColor *color;				/* For non-standard colours */
}

+ (LCMetricGraphMetricItem *) itemForMetric:(LCMetric *)initMetric;
- (id) initWithMetric:(LCMetric *)initMetric;

@property (retain) LCMetric *metric;
@property (retain) LCMetricHistory *history;
@property (copy) NSColor *color;

@end
