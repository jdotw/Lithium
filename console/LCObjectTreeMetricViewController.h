//
//  LCObjectTreeMetricViewController.h
//  Lithium Console
//
//  Created by James Wilson on 28/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCMetric.h"
#import "LCShadowMetricField.h"

@interface LCObjectTreeMetricViewController : NSViewController 
{
	LCMetric *metric;

	IBOutlet LCShadowTextField *metricNameField;
	IBOutlet NSImageView *dotImageView;
	IBOutlet LCShadowMetricField *metricValueField;
	IBOutlet NSMenuItem *menuTitleItem;
	
	BOOL dotShown;
}

- (id) initWithMetric:(LCMetric *)initMetric;

@property (retain) LCMetric *metric;
- (void)menuWillOpen:(NSMenu *)menu;
- (void)menuDidClose:(NSMenu *)menu;
- (IBAction) refreshDeviceClicked:(id)sender;
- (IBAction) openMetricInNewWindowClicked:(id)sender;
- (IBAction) graphMetricClicked:(id)sender;
- (IBAction) trendAnalysisClicked:(id)sender;
- (IBAction) openCaseForMetricClicked:(id)sender;
- (IBAction) faultHistoryClicked:(id)sender;
- (IBAction) adjustTriggersClicked:(id)sender;
- (IBAction) metricHistoryClicked:(id)sender;

@end
