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

@property (nonatomic,retain) NSTableColumn *viewColumn;
@property (nonatomic,retain) NSMutableDictionary *viewControllerDictionary;
@property (nonatomic,retain) LCEntityArrayController *containersArrayController;
@property (nonatomic,retain) NSMenuItem *serviceMenuItem;
@property (nonatomic,retain) id browserController;
@property (nonatomic,retain) NSMutableDictionary *rowHeightDict;
@end
