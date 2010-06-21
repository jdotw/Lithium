//
//  LCShadowMetricField.h
//  Lithium Console
//
//  Created by James Wilson on 28/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCShadowTextField.h"

#import "LCMetric.h"

@interface LCShadowMetricField : LCShadowTextField
{
	LCMetric *metric;
	
	NSTrackingArea *trackArea;
	BOOL mouseOver;
	
	NSButton *historyButton;
	
	NSImage *historyButtonGraphImage;
	NSImage *historyButtonTableImage;
}

@property (nonatomic,retain) LCMetric *metric;
@property (readonly) NSButton *historyButton;

@end
