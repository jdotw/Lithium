//
//  LCBrowserSearchContentController.h
//  Lithium Console
//
//  Created by James Wilson on 24/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2ContentViewController.h"
#import "LCSearchSourceItem.h"
#import "LCInspectorController.h"

@interface LCBrowserSearchContentController : LCBrowser2ContentViewController 
{
	id browser;
	
	NSString *searchString;
	
	NSMutableArray *sources;
	LCSearchSourceItem *selectedSource;
	
	NSArray *selectedEntities;
	LCEntity *selectedEntity;
	
	IBOutlet LCInspectorController *inspectorController;
	IBOutlet NSArrayController *resultArrayController;
}

@property (assign) id browser;
@property (copy) NSString *searchString;

@property (readonly) NSMutableArray *sources;
- (void) insertObject:(LCSearchSourceItem *)item inSourcesAtIndex:(unsigned int)index;
- (void) removeObjectFromSourcesAtIndex:(unsigned int)index;
@property (retain) LCSearchSourceItem *selectedSource;

@property (copy) NSArray *selectedEntities;
@property (retain) LCEntity *selectedEntity;


@end
