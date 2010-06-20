//
//  LCSSceneView.h
//  Lithium Console
//
//  Created by James Wilson on 27/08/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCSSceneDocument.h"
#import "LCSSceneOverlay.h"
#import "LCInsetView.h"

@interface LCSSceneView : LCInsetView 
{
	LCSSceneDocument *sceneDocument;
	
	/* Mouse Tracking */
	NSTrackingArea *trackingArea;	
	bool mouseEntered;
	bool drawBox;
	NSPoint drawBoxMouseStart;
	NSPoint drawBoxMouseCurrent;
	
	/* Views */
	NSMutableArray *overlayViews;
	NSMutableDictionary *overlayViewDict;
	bool forceAllViewsVisible;
	
	/* Geometry */
	NSRect imageRect;
	float xScale;
	float yScale;

	/* Controller */
	IBOutlet id controller;
}

#pragma mark "DrawBox Methods"
- (NSPoint) drawBoxStartPoint;
- (NSSize) drawBoxSize;

#pragma mark Overlay Management
- (void) removeOverlay:(LCSSceneOverlay *)overlay;
- (void) resetOverlayViews;
- (bool) forceAllViewsVisible;
- (void) setForceAllViewsVisible:(BOOL)flag;
- (void) setForceUnboundVisible:(BOOL)flag;
- (NSRect) restoreOverlayRect:(NSRect)translated;

#pragma mark "Accessors"
@property (retain) LCSSceneDocument *sceneDocument;
@property (assign) id controller;
@property (readonly) NSMutableArray *overlayViews;
@property (readonly) NSMutableDictionary *overlayViewDict;

@end
