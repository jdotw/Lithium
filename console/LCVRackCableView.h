//
//  LCVRackCableView.h
//  Lithium Console
//
//  Created by James Wilson on 11/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCVRackCable.h"
#import "LCVRackCablePopupView.h"

@interface LCVRackCableView : NSView 
{
	/* Related Objects */
	id rackController;

	/* Paths */
	NSMutableArray *paths;
	BOOL pathsValid;

	/* Hit */
	NSView *hitView;
	
	/* Animation */
	NSTimer *animationTimer;
	int frame;	
	
	/* Mouse Tracking */
	NSTrackingArea *trackingArea;
	NSTimer *popupTimer;
	LCVRackCablePopupView *cablePopup;
	LCVRackCable *hoverCable;
	NSPoint popupPoint;
}

#pragma mark Constructors
- (id) initWithFrame:(NSRect)initFrame;

#pragma mark Drawing
- (void) drawRect:(NSRect)rect;
- (void) drawThroughputIndicatorsForCable:(LCVRackCable *)cable 
								   onPath:(id)path 
									speed:(NSString *)speed 
									 util:(NSString *)util 
						  speedIndicatorT:(float)speedIndicatorT 
							   cableColor:(NSColor *)cableColor 
								  xOffset:(float)xOffset 
								  yOffset:(float)yOffset
							   cableWidth:(float)cableWidth;

#pragma mark "Mouse Event Handlers"
- (NSView *)hitTest:(NSPoint)aPoint;

#pragma mark Cable Location
- (LCVRackCable *) cableAtPoint:(NSPoint)point;

#pragma mark NSResponder Methods
- (BOOL) acceptsFirstResponder;
- (BOOL) becomeFirstResponder;

#pragma mark "Accessors"
@property (assign) id rackController;
@property (readonly) NSMutableArray *paths;
@property (assign) BOOL pathsValid;
@property (assign) NSView *hitView;
@property (retain) LCVRackCable *hoverCable;


@end
