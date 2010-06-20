//
//  LCXSDeviceView.h
//  Lithium Console
//
//  Created by James Wilson on 4/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXSDriveCageView.h"
#import "LCXSLidView.h"
#import "LCXSAirFlowView.h"
#import "LCXSPowerVisorView.h"
#import "LCDeviceView.h"

@interface LCXSDeviceView : LCDeviceView 
{
	/* Images */
	NSImage *xsBaseImage;	
	
	/* Views */
	NSMutableArray *driveViews;
	NSMutableArray *cpuViews;
	NSMutableArray *ramStickViews;
	NSMutableArray *blowerViews;
	NSMutableArray *powerViews;
	
	/* Drive cage */
	LCXSDriveCageView *driveCageView;
	
	/* Lid */
	LCXSLidView *lidView;
	BOOL hideLid;
	
	/* Air flow */
	LCXSAirFlowView *airFlowView;
	
	/* Powervisor View */
	LCXSPowerVisorView *powerVisorView;
	BOOL hidePowerVisor;
}

#pragma mark "Constructors"
- (LCXSDeviceView *) initWithDevice:(LCEntity *)initDevice inFrame:(NSRect)frame;
- (void) dealloc;

#pragma mark "Geometry"
- (float) xsWidth;
- (float) xsHeight;
- (NSRect) xsRect;
- (NSRect) driveBay1Rect;
- (NSRect) driveBay2Rect;
- (NSRect) driveBay3Rect;
- (NSRect) cpu1Rect;
- (NSRect) cpu2Rect;
- (NSRect) ramStickRect:(int)index;

#pragma mark "Drawing"
- (void) drawRect:(NSRect)rect;

#pragma mark "KVO Methods"
- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context;

#pragma mark "Resizing"
- (void)setFrameSize:(NSSize)newSize;

#pragma mark "View Management"
- (void) removeFromSuperview;
- (void) resetDriveViews;
- (void) resetBlowerViews;
- (void) resetCPUViews;
- (void) resetRamStickViews;
- (void) resetPowerViews;
- (void) resetPowerVisorView;
- (void) resetLidView;
- (void) resetDriveCageView;


@property (retain) NSImage *xsBaseImage;
@property (retain) NSMutableArray *driveViews;
@property (retain) NSMutableArray *cpuViews;
@property (retain) NSMutableArray *ramStickViews;
@property (retain) NSMutableArray *blowerViews;
@property (retain) NSMutableArray *powerViews;
@property (retain) LCXSDriveCageView *driveCageView;
@property (retain) LCXSLidView *lidView;
@property BOOL hideLid;
@property (retain) LCXSAirFlowView *airFlowView;
@property (retain) LCXSPowerVisorView *powerVisorView;
@property BOOL hidePowerVisor;
@end
