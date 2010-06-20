//
//  LCMetricGraphView.h
//  Lithium Console
//
//  Created by James Wilson on 1/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

#import "LCEntity.h"

#import "LCMetricGraphController.h"

@interface LCMetricGraphView : NSView 
{
	IBOutlet LCMetricGraphController *graphController;
	
	IBOutlet NSScrollView *scrollView;
	IBOutlet NSArrayController *entityArrayController;
	BOOL showTooManySourcesWarning;
	
	NSString *overlayLabel;

	NSImageView *primaryImageView;
	NSImageView *baselineImageView;
	
	int style;
	BOOL highlightBaselineView;
	float fontSize;
	BOOL showDates;
	
	id dragSource;
	
	BOOL dragAndDropEnabled;
	
	CALayer *rootLayer;
	CAScrollLayer *bodyLayer;
	CALayer *primaryImageLayer;
	CALayer *baselineImageLayer;
}

#pragma mark "Graph manipulation"
- (void) blankView;

#pragma mark "Properties"
@property (readonly) float timeLineHeight;
@property (assign) float fontSize;
@property (readonly) float border;
@property (readonly) NSRect graphRect;
@property (readonly) NSRect graphImageRect;
@property (assign) int style;
@property (assign) BOOL showDates;

#define GVSTYLE_DEFAULT 0
#define GVSTYLE_STADIUM 1

@property (assign) LCMetricGraphController *graphController;
@property (assign) BOOL showTooManySourcesWarning;
@property (copy) NSString *overlayLabel;
@property (retain) NSImageView *primaryImageView;
@property (retain) NSImageView *baselineImageView;

@property (assign) BOOL highlightBaselineView;

@property (assign) BOOL dragAndDropEnabled;

@end
