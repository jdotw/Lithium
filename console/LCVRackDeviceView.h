//
//  LCVRackDeviceView.h
//  Lithium Console
//
//  Created by James Wilson on 30/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowserVRackContentController.h"

@interface LCVRackDeviceView : NSView 
{
	LCVRackDevice *rackDevice;
	LCBrowserVRackContentController *rackController;
	id rackView;

	NSTrackingArea *trackingArea;
	BOOL mouseEntered;
	
	BOOL isZoomView;
	
	BOOL selected;
}

#pragma mark "Constructors"
- (id)initWithDevice:(LCVRackDevice *)newDevice inFrame:(NSRect)frame;
- (void) removeViewAndContent;

@property (retain) LCVRackDevice *rackDevice;
@property (assign) LCBrowserVRackContentController *rackController;
@property (assign) id rackView;
@property (assign) BOOL isZoomView;
@property (readonly) float pointsPerRU;
@property (assign) BOOL selected;

@end
