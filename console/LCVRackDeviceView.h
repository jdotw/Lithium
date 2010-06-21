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

@property (nonatomic,retain) LCVRackDevice *rackDevice;
@property (nonatomic, assign) LCBrowserVRackContentController *rackController;
@property (nonatomic, assign) id rackView;
@property (nonatomic, assign) BOOL isZoomView;
@property (readonly) float pointsPerRU;
@property (nonatomic, assign) BOOL selected;

@end
