//
//  LCMetricGraphCompareButton.h
//  Lithium Console
//
//  Created by James Wilson on 6/02/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCMetricGraphView.h"

@interface LCMetricGraphCompareButton : NSPopUpButton 
{
	NSTrackingArea *trackingArea;
	IBOutlet LCMetricGraphView *graphView;
}

@end
