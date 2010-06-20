//
//  LCXSCPUView.h
//  Lithium Console
//
//  Created by James Wilson on 4/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCObject.h"

@interface LCXSCPUView : NSView 
{
	/* Object */
	LCObject *cpuObject;
	
	/* Images */
	NSImage *cpuImage;
}

#pragma mark "Constructors"
- (id) initWithCPU:(LCObject *)initCPU inFrame:(NSRect)frame ;
- (void) dealloc;

#pragma mark "View Management"
- (void) removeFromSuperview;

#pragma mark "Drawing"
- (void) drawRect:(NSRect)rect;
- (void) fillGaugeRect:(NSRect)rect amount:(float)amount;

@end
