//
//  LCSSceneOverlayView.h
//  Lithium Console
//
//  Created by James Wilson on 27/08/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCSSceneOverlay.h"
#import "LCSSceneScrollerView.h"

@interface LCSSceneOverlayView : NSView 
{
	LCSSceneOverlay *overlay;
	BOOL mouseEntered;
	BOOL forceVisible;
	BOOL autoExpanded;

	NSColor *overlayBackColor;

	NSTrackingArea *trackingArea;

	NSPoint moveFromPoint;
	BOOL inResize;
	BOOL inMove;
	NSRect resizeCursorRect;
	
	LCSSceneScrollerView *scrollView;
	
	LCEntity *observedEntity;	
	
	BOOL selected;
}

#pragma mark "Constructors"
- (LCSSceneOverlayView *) initWithOverlay:(LCSSceneOverlay *)initOverlay inFrame:(NSRect)frame;
- (void) removeViewAndContent;
- (void) dealloc;

#pragma mark "Drawing Method"
- (void) drawRect:(NSRect)rect;

#pragma mark "Outline Path"
- (NSBezierPath *) outlinePath:(NSRect)frame;

#pragma mark "Context Menu Actions"
- (IBAction) removeOverlayClicked:(id)sender;
- (IBAction) removeEntityClicked:(id)sender;
- (IBAction) browseToClicked:(id)sender;
- (IBAction) faultHistoryClicked:(id)sender;
- (IBAction) metricHistoryClicked:(id)sender;
- (IBAction) graphEntityClicked:(id)sender;

#pragma mark "Accessors"
- (BOOL) forceVisible;
- (void) setForceVisible:(BOOL)flags;
- (LCSSceneOverlay *) overlay;
- (BOOL) autoExpanded;
- (NSColor *) overlayBackColor;
- (void) setOverlayBackColor:(NSColor *)newColor;
+ (NSColor *) defaultBackColor;
- (LCEntity *) observedEntity;
- (void) setObservedEntity:(LCEntity *)newEntity;

@property (retain,getter=overlay) LCSSceneOverlay *overlay;
@property BOOL mouseEntered;
@property (getter=forceVisible,setter=setForceVisible:) BOOL forceVisible;
@property (getter=autoExpanded) BOOL autoExpanded;
@property BOOL inResize;
@property BOOL inMove;
@property (retain) LCSSceneScrollerView *scrollView;
@property (assign) BOOL selected;

@end
