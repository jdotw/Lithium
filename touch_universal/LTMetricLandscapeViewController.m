//
//  LTMetricLandscapeViewController.m
//  Lithium
//
//  Created by James Wilson on 7/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTMetricLandscapeViewController.h"
#import "LTGraphView.h"
#import <QuartzCore/QuartzCore.h>

#define screenWidth 480.0f
#define screenHeight 320.0f

@implementation LTMetricLandscapeViewController

- (id)initWithMetric:(LTEntity *)initMetric
{
	self = [super initWithNibName:@"LTMetricLandscapeViewController" bundle:nil];
	if (!self) return nil;
		
	self.metric = initMetric;
	
    return self;
}

- (void) viewDidLoad
{
	[super viewDidLoad];
	
	CGRect contentRect = CGRectMake(0.0, 0.0, 8000.0, graphScrollView.frame.size.height);
	NSArray *labels = [NSArray arrayWithObjects:leftMaxLabel, leftAvgLabel, leftMinLabel, rightMaxLabel, rightAvgLabel, rightMinLabel, nil];
	for (UILabel *label in labels)
	{
		label.layer.shadowOffset = CGSizeMake(2.0, 2.0);
		label.layer.shadowRadius = 3.0;
		label.layer.shadowOpacity = 0.8;
	}
	graphView = [[LTGraphView alloc] initWithFrame:contentRect];
	if (leftMinLabel) [graphView.minLabels addObject:leftMinLabel];
	if (rightMinLabel) [graphView.minLabels addObject:rightMinLabel];
	if (leftAvgLabel) [graphView.avgLabels addObject:leftAvgLabel];
	if (rightAvgLabel) [graphView.avgLabels addObject:rightAvgLabel];
	if (leftMaxLabel) [graphView.maxLabels addObject:leftMaxLabel];
	if (rightMaxLabel) [graphView.maxLabels addObject:rightMaxLabel];
	[graphView setMetrics:[NSArray arrayWithObject:self.metric]];
	[graphView setNeedsDisplay];
	[graphScrollView addSubview:graphView];
	graphScrollView.contentSize = graphView.frame.size;	
	graphScrollView.maximumZoomScale = 1.0;
	graphScrollView.minimumZoomScale = 10.0;
	graphScrollView.delegate = self;
	[graphScrollView scrollRectToVisible:CGRectMake(CGRectGetMaxX(contentRect) - CGRectGetWidth(graphScrollView.frame),
													0.0, CGRectGetWidth(graphScrollView.frame), CGRectGetHeight(graphScrollView.frame)) animated:NO];
    
    metricLabel.layer.shadowOffset = CGSizeMake(2.0, 2.0);
    metricLabel.layer.shadowRadius = 3.0;
    metricLabel.layer.shadowOpacity = 0.8;

    metricLabel.text = self.metric.longLocationString;
	
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

- (void)dealloc 
{
    self.metric = nil;
    graphScrollView.delegate = nil;
    
    [graphView release];

    [super dealloc];
}

@synthesize metric;

@end
