//
//  LCGraphStadiumController.h
//  Lithium Console
//
//  Created by James Wilson on 27/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>

#import "LCEntity.h"
#import "LCBrowserHorizontalScroller.h"
#import "LCGraphFlowController.h"

@interface LCGraphStadiumController : NSObject 
{
	/* Entities */
	LCEntity *target;
	NSArray *targetArray;
	LCEntity *selectedEntity;

	/* Properties */
	NSMutableDictionary *graphableObjects;
	NSMutableArray *graphControllers;
	NSMutableArray *slices;
	NSString *title;

	/* UI Elements */
	IBOutlet NSView *stadiumView;
	IBOutlet LCBrowserHorizontalScroller *flowScroller;
	
	/* Root Layers */
	CALayer *rootLayer;
	CAScrollLayer *bodyLayer;
	CATextLayer *desktopImageCountLayer;
	CGImageRef shadowImage;
	NSDictionary *textStyle;
		
	/* Graph Flow */
	LCGraphFlowController *flowController;
}

#pragma mark "Slice Management"
- (void) updateControllersAndLayers;

#pragma mark "Properties"

@property (retain) LCEntity *target;
@property (retain) LCEntity *selectedEntity;
@property (copy) NSArray *targetArray;
@property (copy) NSString *title;

@property (readonly) NSMutableDictionary *graphableObjects;
@property (readonly) NSMutableArray *graphControllers;
@property (readonly) NSMutableArray *slices;

@property (assign) NSView *stadiumView;

@property (readonly) LCGraphFlowController *flowController;
@property (readonly) CAScrollLayer *bodyLayer;

@end
