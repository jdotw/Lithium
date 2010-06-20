//
//  LCVRackView.h
//  Lithium Console
//
//  Created by James Wilson on 11/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCVRackCableView.h"
#import "LCVRackUnit.h"
#import "LCVRackCable.h"
#import "LCVRackDevice.h"

@interface LCVRackView : NSView 
{	
	float pointsPerRU;
	float railX1;
	float railX2;
	float margin;
	
	NSTrackingArea *trackingArea;
	bool mouseEntered;
	
	BOOL draggingEntered;
	NSDragOperation dragOperation;
	int dropDeviceSize;

	IBOutlet id controller;
}

#pragma mark Rack Unit Methods
- (LCVRackUnit *) rackUnitAtPoint:(NSPoint)locationInWindow;

#pragma mark Selection Handling 
- (void) calculateSelection:(NSPoint)locationInWindow;

#pragma mark Sizes
- (void) setFrame:(NSRect)rect;
- (void) updateSizes;

#pragma mark Rectangle for Device Method
- (NSRect) rectForDevice:(LCVRackDevice *)rDev;

#pragma mark Rectangle for Rack 
- (NSRect) rectForRack;

#pragma mark General Accessors
@property (assign) float pointsPerRU;
@property (retain) id controller;

@end
