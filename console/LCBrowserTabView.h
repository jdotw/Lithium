//
//  LCBrowserTabView.h
//  Lithium Console
//
//  Created by James Wilson on 20/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCBrowserTabView : NSTabView 
{
	NSTabViewItem *selectedTabViewItem;
}

@property (nonatomic,retain,getter=selectedTabViewItem) NSTabViewItem *selectedTabViewItem;
@end
