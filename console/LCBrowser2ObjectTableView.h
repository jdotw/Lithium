//
//  LCBrowser2ObjectTableView.h
//  Lithium Console
//
//  Created by James Wilson on 14/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCBrowserTableView.h"
#import "LCEntityArrayController.h"

@interface LCBrowser2ObjectTableView : LCBrowserTableView
{
	NSTableColumn *viewColumn;
	NSMutableDictionary *viewControllerDictionary;
	IBOutlet LCEntityArrayController *containersArrayController;
	IBOutlet NSMenuItem *serviceMenuItem;
	IBOutlet id browserController;
	NSMutableDictionary *rowHeightDict;
}

@property (retain) NSTableColumn *viewColumn;
@property (retain) NSMutableDictionary *viewControllerDictionary;
@property (retain) LCEntityArrayController *containersArrayController;
@property (retain) NSMenuItem *serviceMenuItem;
@property (retain) id browserController;
@property (retain) NSMutableDictionary *rowHeightDict;
@end
