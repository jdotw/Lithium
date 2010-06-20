//
//  LCXSDriveView.h
//  Lithium Console
//
//  Created by James Wilson on 4/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCObject.h"

@interface LCXSDriveView : NSView 
{
	/* Drive */
	int slot;
	LCObject *driveObject;
	
	/* Image */
	NSImage *driveImage;	
}

#pragma mark "Constructors"
- (id) initWithDrive:(LCObject *)initDrive slot:(int)index inFrame:(NSRect)frame;
- (void) dealloc;

#pragma mark "View Management"
- (void) removeFromSuperview;

#pragma mark "Drawing"
- (void) drawRect:(NSRect)rect;

@end
