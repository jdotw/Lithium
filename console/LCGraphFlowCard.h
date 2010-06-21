//
//  LCGraphFlowCard.h
//  Lithium Console
//
//  Created by James Wilson on 4/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

#import "LCMetric.h"
#import "LCGraphFlowController.h"
#import "LCMetricGraphController.h"

#define STACKED_LEFT 0
#define FLAT 1
#define STACKED_RIGHT 2

@interface LCGraphFlowCard : NSObject 
{
	CALayer *cardLayer;
	CALayer *imageLayer;
	CALayer *reflectionLayer;
	CALayer *gradientLayer;
	CATextLayer *objLabelLayer;
	CATextLayer *metLabelLayer;
	
	BOOL stacked;			// Is the card stacked (or flat)
	int orientation;		// 0=Left 2=Flat 3=Right
	
	LCMetric *metric;
	LCGraphFlowController *flowController;
	LCMetricGraphController *graphController;
}

@property (readonly) CALayer *cardLayer;
@property (readonly) CALayer *imageLayer;
@property (readonly) CALayer *reflectionLayer;
@property (readonly) CALayer *gradientLayer;
@property (readonly) CATextLayer *objLabelLayer;
@property (readonly) CATextLayer *metLabelLayer;

@property (nonatomic,retain) LCMetric *metric;
@property (nonatomic, assign) LCGraphFlowController *flowController;
@property (readonly) LCMetricGraphController *graphController;

@property (nonatomic, assign) int orientation;

@end
