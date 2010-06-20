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

@property (retain) LCBrowserEntityCell *entityCell;
@property (retain) LCTextFieldCell *textCell;
@property (retain) NSMenu *devicesMenu;
@property (retain) NSMenu *servicesMenu;
@property (retain) NSMenuItem *connectToMenu;
@property (retain) NSMenuItem *addSiteMenu;
@property (retain) NSMenuItem *editSiteMenu;
@property (retain) NSMenuItem *removeSiteMenu;
@property (retain) NSMenuItem *addDeviceMenu;
@property (retain) NSMenuItem *editDeviceMenu;
@property (retain) NSMenuItem *removeDeviceMenu;
@property (retain) NSMenuItem *addServiceMenu;
@property (retain) NSMenuItem *manageServiceScriptsMenu;
@property (assign) id browser;

@end
