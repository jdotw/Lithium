//
//  LCVRackDeviceBackView.h
//  Lithium Console
//
//  Created by James Wilson on 12/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCVRackView.h"
#import "LCVRackDevice.h"
#import "LCVRackInterface.h"
#import "LCEntity.h"
#import "LCBrowserVRackContentController.h"
#import "LCVRackDeviceView.h"
#import "LCPopupView.h"

@interface LCVRackDeviceBackView : LCVRackDeviceView 
{
	/* Racks properties */
	NSMutableArray *interfaces;

	/* Mouse tracking */
	BOOL cableHighlightDeviceSet;
	BOOL mouseDown;

	/* Hovering */
	LCVRackInterface *hoverInterface;
	LCPopupView *popupView;
	
	/* Selection */
	LCEntity *selectedInterfaceEntity;
}

#pragma mark Drawing
- (void)drawRect:(NSRect)rect;

#pragma mark "Popup Methods"
- (NSRect) popupRectForInterface:(LCVRackInterface *)interface;
- (LCPopupView *) popupViewForInterface:(LCVRackInterface *)interface;

#pragma mark Interface Methods
- (NSRect) rectForInterface:(LCEntity *)entity;

#pragma mark Properties
@property (nonatomic,retain) LCVRackInterface *hoverInterface;
@property (nonatomic,retain) LCPopupView *popupView;
@property (nonatomic,retain) LCEntity *selectedInterfaceEntity;

@end
