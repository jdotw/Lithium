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

@property (nonatomic,retain) id browserController;
@property (nonatomic,retain) NSMenuItem *connectToMenu;
@property (nonatomic,retain) NSMenuItem *addSiteMenu;
@property (nonatomic,retain) NSMenuItem *editSiteMenu;
@property (nonatomic,retain) NSMenuItem *removeSiteMenu;
@property (nonatomic,retain) NSMenuItem *addDeviceMenu;
@property (nonatomic,retain) NSMenuItem *editDeviceMenu;
@property (nonatomic,retain) NSMenuItem *removeDeviceMenu;
@property (nonatomic,retain) NSMenuItem *addServiceMenu;
@property (nonatomic,retain) NSMenuItem *manageServiceScriptsMenu;
@end
