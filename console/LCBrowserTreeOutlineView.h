//
//  LCBrowserTreeOutlineView.h
//  Lithium Console
//
//  Created by James Wilson on 8/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCOutlineView.h"
#import "LCBrowserEntityCell.h"
#import "LCTextFieldCell.h"
#import "LCSSceneDocument.h"
#import "LCVRackDocument.h"

@interface LCBrowserTreeOutlineView : LCOutlineView 
{
	/* Cells */
	LCBrowserEntityCell *entityCell;
	LCTextFieldCell *textCell;
	
	/* Menus */
	IBOutlet NSMenu *devicesMenu;
	IBOutlet NSMenu *servicesMenu;
	IBOutlet NSMenu *scenesMenu;
	IBOutlet NSMenuItem *scenesMenuAddItem;
	IBOutlet NSMenu *vracksMenu;
	IBOutlet NSMenuItem *vracksMenuAddItem;
	IBOutlet NSMenu *groupsMenu;
	IBOutlet NSMenuItem *groupsMenuRemoveEntityItem;
	IBOutlet NSMenu *faultsMenu;
	IBOutlet NSMenu *applicationsMenu;
	IBOutlet NSMenu *rootItemsMenu;
	IBOutlet NSMenu *deploymentsMenu;
	IBOutlet NSMenuItem *deploymentsMenuRefreshItem;
	
	/* Group Entity-Specific menus */
	IBOutlet NSMenuItem *groupDeviceMenuItem;
	IBOutlet NSMenuItem *groupLocationMenuItem;
	
	/* Selective Menu Items */
	IBOutlet NSMenuItem *connectToMenu;
	IBOutlet NSMenuItem *addSiteMenu;
	IBOutlet NSMenuItem *editSiteMenu;
	IBOutlet NSMenuItem *removeSiteMenu;
	IBOutlet NSMenuItem *addDeviceMenu;
	IBOutlet NSMenuItem *editDeviceMenu;
	IBOutlet NSMenuItem *removeDeviceMenu;
	IBOutlet NSMenuItem *addServiceMenu;
	IBOutlet NSMenuItem *manageServiceScriptsMenu;
	
	/* Controller */
	IBOutlet id browser;	
}

- (void) selectEntity:(LCEntity *)entity;
- (NSMenu *) devicesMenuForEvent:(NSEvent *)event;
- (void) selectScenesRoot;
- (void) selectVRacksRoot;
- (void) selectSceneDocument:(LCSSceneDocument *)document;
- (void) selectVRackDocument:(LCVRackDocument *)document;
- (void) selectIncidentsItem;

@property (nonatomic,retain) LCBrowserEntityCell *entityCell;
@property (nonatomic,retain) LCTextFieldCell *textCell;
@property (nonatomic,retain) NSMenu *devicesMenu;
@property (nonatomic,retain) NSMenu *servicesMenu;
@property (nonatomic,retain) NSMenuItem *connectToMenu;
@property (nonatomic,retain) NSMenuItem *addSiteMenu;
@property (nonatomic,retain) NSMenuItem *editSiteMenu;
@property (nonatomic,retain) NSMenuItem *removeSiteMenu;
@property (nonatomic,retain) NSMenuItem *addDeviceMenu;
@property (nonatomic,retain) NSMenuItem *editDeviceMenu;
@property (nonatomic,retain) NSMenuItem *removeDeviceMenu;
@property (nonatomic,retain) NSMenuItem *addServiceMenu;
@property (nonatomic,retain) NSMenuItem *manageServiceScriptsMenu;
@property (nonatomic, assign) id browser;

@end
