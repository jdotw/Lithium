//
//  LCBrowserOutlineView.h
//  Lithium Console
//
//  Created by James Wilson on 8/12/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCBrowserOutlineView : NSOutlineView 
{
	IBOutlet NSTreeController *treeController;
	IBOutlet NSTableColumn *viewColumn;
	
	NSMutableDictionary *entityViewControllers;	
}

@property (nonatomic,retain) NSTreeController *treeController;
@property (nonatomic,retain) NSTableColumn *viewColumn;
@property (nonatomic,retain) NSMutableDictionary *entityViewControllers;
@end
