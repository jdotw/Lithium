//
//  LCInspectorItem.h
//  Lithium Console
//
//  Created by James Wilson on 2/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCInspectorItem : NSViewController 
{
	/* Target */
	id target;
	
	/* Controller */
	id controller;
	
	/* UI Elements */
	IBOutlet NSObjectController *controllerAlias;

	/* View Controllers */
	NSMutableArray *viewControllers;
	
	/* Display String */
	NSString *displayString;
	
	/* Misc */
	BOOL expandByDefault;
}

#pragma mark "Constructors"
+ (LCInspectorItem *) itemWithTarget:(id)initTarget forController:(id)initController;
+ (LCInspectorItem *) itemWithTarget:(id)initTarget;
- (LCInspectorItem *) initWithTarget:(id)initTarget forController:(id)initController;
- (LCInspectorItem *) initWithTarget:(id)initTarget;
- (void) dealloc;
	
#pragma mark "View Controllers"
@property (readonly) NSMutableArray *viewControllers;
- (void) insertObject:(id)viewController inViewControllersAtIndex:(unsigned int)index;
- (void) removeObjectFromViewControllersAtIndex:(unsigned int)index;
- (BOOL) leafNode;

#pragma mark "Target Methods"
@property (retain) id target;

#pragma mark "Controller Methods"
@property (assign) id controller;

#pragma mark "View Management"
- (void) removedFromInspector;

#pragma mark "Display String"
@property (copy) NSString *displayString;

#pragma mark "View Height Methods"
- (void) adjustViewHeightsBy:(float)delta;
- (void) adjustViewHeightsToDefault;

#pragma mark "Accessors"
@property (readonly) NSString *nibName;
- (float) rowHeight;
@property (assign) BOOL expandByDefault;
- (BOOL) allowsResize;

@end
