//
//  LCInspXRaidMiniView.h
//  Lithium Console
//
//  Created by James Wilson on 17/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXRDeviceView.h"
#import "LCInspectorController.h"

@interface LCInspXRaidMiniView : NSView 
{
	/* Display mode */
	int numUnits;
	
	/* RAID 1 */
	NSImage *raid1Image;		/* The rendered image of the unit */
	NSArray *raid1Arrays;		/* Arrays to highlight */
	LCXRDeviceView *raid1View;	/* THe full-size "real" view */
	
	/* RAID 2 */
	NSImage *raid2Image;
	NSArray *raid2Arrays;
	LCXRDeviceView *raid2View;
	
	/* Metadata Arrays */
	NSArray *metadataArrays;
	
	/* Inspector Controller */
	LCInspectorController *controller;
}

#pragma mark "Drawing"
- (void) drawRect:(NSRect)rect;
- (void) drawRaidInRect:(NSRect)rect device:(LCEntity *)device arrays:(NSArray *)arrays image:(NSImage *)raidImage scale:(float)scaleRatio;

#pragma mark "Image Update"
- (void) updateImages;

#pragma mark "Array Management"
@property (nonatomic, assign) int numUnits;
@property (nonatomic, retain) NSArray *raid1Arrays;
@property (nonatomic, retain) NSArray *raid2Arrays;
@property (nonatomic,retain) NSArray *metadataArrays;
@property (nonatomic, assign) LCInspectorController *controller;

@end
