//
//  LCGraphFlowController.h
//  Lithium Console
//
//  Created by James Wilson on 4/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <QuartzCore/QuartzCore.h>

#import "LCObject.h"
#import "LCMetric.h"
#import "LCBrowserHorizontalScroller.h"

@interface LCGraphFlowController : NSObject 
{
	/* Metrics */
	LCEntity *target;
	NSArray *metrics;
	LCMetric *focusMetric;
	double scrollValue;

	/* Cards */
	NSMutableArray *cards;
	NSMutableDictionary *cardDictionary;
	
	/* Layer */
	CALayer *rootLayer;
	
	/* Scroller */
	LCBrowserHorizontalScroller *scroller;
}

- (void) updateCards;

@property (nonatomic,retain) LCEntity *target;
@property (nonatomic,copy) NSArray *metrics;
@property (nonatomic, assign) LCMetric *focusMetric;
@property (nonatomic, assign) double scrollValue;
- (void) scrollToMetric:(LCMetric *)metric;
- (void) scrollToObject:(LCObject *)object;
@property (readonly) NSMutableArray *cards;
- (void) insertObject:(id)obj inCardsAtIndex:(unsigned int)index;
- (void) removeObjectFromCardsAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *cardDictionary;
@property (readonly) CALayer *rootLayer;
@property (nonatomic, assign) LCBrowserHorizontalScroller *scroller;

@end
