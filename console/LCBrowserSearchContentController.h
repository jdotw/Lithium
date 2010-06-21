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

@property (nonatomic, assign) id browser;
@property (nonatomic, copy) NSString *searchString;

@property (nonatomic,readonly) NSMutableArray *sources;
- (void) insertObject:(LCSearchSourceItem *)item inSourcesAtIndex:(unsigned int)index;
- (void) removeObjectFromSourcesAtIndex:(unsigned int)index;
@property (nonatomic,retain) LCSearchSourceItem *selectedSource;

@property (nonatomic,copy) NSArray *selectedEntities;
@property (nonatomic,retain) LCEntity *selectedEntity;


@end
