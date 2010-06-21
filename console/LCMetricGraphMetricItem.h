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

@property (nonatomic,retain) LCMetric *metric;
@property (nonatomic,retain) LCMetricHistory *history;
@property (nonatomic,copy) NSColor *color;

@end
