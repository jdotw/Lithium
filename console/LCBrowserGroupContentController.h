//
//  LCBrowserGroupContentController.h
//  Lithium Console
//
//  Created by James Wilson on 30/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2ContentViewController.h"
#import "LCBrowser2Controller.h"
#import "LCBrowser2ContentViewController.h"
#import "LCInspectorController.h"
#import "LCGroup.h"
#import "LCGroupTreeOutlineView.h"
#import "LCGraphStadiumController.h"

@interface LCBrowserGroupContentController : LCBrowser2ContentViewController 
{
	/* Objects */
	LCGroup *group;
	LCBrowser2Controller *browser;

	/* Outline */
	NSMutableArray *outlineItems;
	
	/* UI Elements */
	IBOutlet NSTreeController *groupTreeController;
	IBOutlet LCGroupTreeOutlineView *groupOutlineView;
	IBOutlet LCInspectorController *inspectorController;
	IBOutlet LCGraphStadiumController *stadiumController;
	
	/* Selection */
	LCEntity *selectedEntity;
	NSArray *selectedEntities;	
	id inspectorTarget;	
}

#pragma mark "Constructors"
- (id) initWithGroup:(LCGroup *)initGroup inBrowser:(LCBrowser2Controller *)initBrowser;

#pragma mark "Selection (KVO Observable)"
@property (nonatomic, assign) LCEntity *selectedEntity;
@property (nonatomic,copy) NSArray *selectedEntities;
@property (nonatomic, assign) id inspectorTarget;

#pragma mark "Outline Item Methods"
- (void) rebuildOutlineItemArray;

#pragma mark "Properties"
@property (nonatomic,retain) LCGroup *group;
@property (readonly) NSMutableArray *outlineItems;
- (void) insertObject:(id)item inOutlineItemsAtIndex:(unsigned int)index;
- (void) removeObjectFromOutlineItemsAtIndex:(unsigned int)index;
@property (readonly) NSWindow *window;
@property (nonatomic, assign) LCBrowser2Controller *browser;

@end
