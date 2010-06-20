//
//  LCBrowser2ContentViewController.h
//  Lithium Console
//
//  Created by James Wilson on 26/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCIncident.h"

#define RESIZE_BOTTOM 0
#define RESIZE_TOP 1
#define RESIZE_BOTH 2

@interface LCBrowser2ContentViewController : NSViewController 
{
	IBOutlet NSView *bottomView;
	IBOutlet NSView *inspectorView;
	IBOutlet NSView *statusBarView;
	IBOutlet NSObjectController *controllerAlias;
	
	int resizeMode;
	
	NSArray *sortStringDescriptors;
}

- (void) removedFromBrowserWindow;
- (void) selectEntity:(LCEntity *)entity;
- (void) selectIncident:(LCIncident *)incident;
- (BOOL) treeSelectionCanChangeToRepresent:(id)obj;

@property (assign) NSView *bottomView;
@property (assign) NSView *inspectorView;
@property (assign) NSView *statusBarView;
@property (readonly) CGFloat preferredFixedComponentHeight;
@property (readonly) CGFloat preferredInspectorWidth;
@property (assign) int resizeMode;
@property (copy) NSArray *sortStringDescriptors;

@end
