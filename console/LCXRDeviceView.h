//
//  LCXRDeviceView.h
//  Lithium Console
//
//  Created by James Wilson on 15/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXRDriveView.h"
#import "LCDeviceView.h"

@interface LCXRDeviceView : LCDeviceView 
{
	/* Images */
	NSImage *xsrBackImage;
	NSImage *driveBackImage;
	NSImage *driveBlankImage;
	NSImage *appleLogo;
	NSImage *boltImage;
	NSImage *arrayWindowImage;
	
	/* Drive views */
	NSMutableArray *driveViews;
	
	/* Geometry */
	float scale;
}

#pragma mark "Constructors"
- (LCXRDeviceView *) initWithDevice:(LCEntity *)initDevice inFrame:(NSRect)frame;
- (void) dealloc;

#pragma mark "Geometry"
- (float) scale;
- (void) setScale:(float)val;
- (float) containerViewHight;
- (float) xsrWidth;
- (float) xsrHeight;
- (NSRect) xsrRect;
- (NSRect) driveBay1Rect;
- (NSRect) driveBay2Rect;

#pragma mark "Drawing"
- (void) drawRect:(NSRect)rect;

#pragma mark "Resizing"
- (void)setFrameSize:(NSSize)newSize;

#pragma mark "Drive View Management"
- (void) resetDriveViews;
- (void) driveDidRaise:(LCXRDriveView *)driveView;
- (void) driveDidLower:(LCXRDriveView *)driveView;

@property (retain) NSImage *xsrBackImage;
@property (retain) NSImage *driveBackImage;
@property (retain) NSImage *driveBlankImage;
@property (retain) NSImage *appleLogo;
@property (retain) NSImage *boltImage;
@property (retain) NSImage *arrayWindowImage;
@property (retain) NSMutableArray *driveViews;
@property (getter=scale,setter=setScale:) float scale;
@end
