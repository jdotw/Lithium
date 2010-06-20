//
//  LCXRDriveView.h
//  Lithium Console
//
//  Created by James Wilson on 27/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCObject.h"
#import "LCContainer.h"
#import "LCEntityViewController.h"

@interface LCXRDriveView : NSView 
{
	/* Drive */
	int slot;
	LCObject *driveObject;
	
	/* Image */
	NSImage *driveBackImage;
	NSImage *driveBlankImage;
	NSImage *arrayWindowImage;
	NSImage *popupImage;
	
	/* Pop-out */
	NSRect originalFrame;
	NSTrackingRectTag trackTag;
	BOOL trackingRectEnabled;
	BOOL isRaised;
	NSTimer *raiseTimer;
	NSTimer *lowerTimer;	
	BOOL arrayViewShown;
	NSRect objViewRect;
	NSRect driveObjViewRect;
	NSRect arrayObjViewRect;
	
	/* Regions */
	NSPoint currentMouseLocation;
	NSRect arrayRect;
	
	/* View Controller */
	LCEntityViewController *driveViewController;
	LCEntityViewController *arrayViewController;
}

#pragma mark "Constructor"
- (id)initWithDrive:(LCObject *)initDrive slot:(int)index inFrame:(NSRect)frame;

#pragma mark "Accessors"
@property (assign) BOOL trackingRectEnabled;

#pragma mark "Raise/Lower Operations"
- (void) raise;
- (void) lower;

@end
