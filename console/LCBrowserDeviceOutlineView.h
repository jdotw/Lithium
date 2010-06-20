//
//  LCBrowserDeviceOutlineView.h
//  Lithium Console
//
//  Created by James Wilson on 20/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCTransparentOutlineView.h"

@interface LCBrowserDeviceOutlineView : LCTransparentOutlineView 
{
	NSTableColumn *viewColumn;
	NSMutableDictionary *viewControllerDictionary;	
}

@property (retain) NSTableColumn *viewColumn;
@property (retain) NSMutableDictionary *viewControllerDictionary;
@end
