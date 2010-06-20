//
//  LCBrowserTreeItemRoot.h
//  Lithium Console
//
//  Created by James Wilson on 21/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowserTreeItem.h"

@interface LCBrowserTreeItemRoot : LCBrowserTreeItem
{
	int preferredIndex;	
	BOOL enabled;
}

@property (assign) int preferredIndex;
@property (assign) BOOL enabled;

@end
