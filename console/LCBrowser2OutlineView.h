//
//  LCBrowser2OutlineView.h
//  Lithium Console
//
//  Created by James Wilson on 16/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCBrowser2OutlineView : NSOutlineView 
{
	IBOutlet id browserController;
	IBOutlet NSMenuItem *connectToMenu;
	IBOutlet NSMenuItem *addSiteMenu;
	IBOutlet NSMenuItem *editSiteMenu;
	IBOutlet NSMenuItem *removeSiteMenu;
	IBOutlet NSMenuItem *addDeviceMenu;
	IBOutlet NSMenuItem *editDeviceMenu;
	IBOutlet NSMenuItem *removeDeviceMenu;
	IBOutlet NSMenuItem *addServiceMenu;
	IBOutlet NSMenuItem *manageServiceScriptsMenu;
}

@property (retain) id browserController;
@property (retain) NSMenuItem *connectToMenu;
@property (retain) NSMenuItem *addSiteMenu;
@property (retain) NSMenuItem *editSiteMenu;
@property (retain) NSMenuItem *removeSiteMenu;
@property (retain) NSMenuItem *addDeviceMenu;
@property (retain) NSMenuItem *editDeviceMenu;
@property (retain) NSMenuItem *removeDeviceMenu;
@property (retain) NSMenuItem *addServiceMenu;
@property (retain) NSMenuItem *manageServiceScriptsMenu;
@end
