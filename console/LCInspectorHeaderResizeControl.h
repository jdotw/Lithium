//
//  LCInspectorHeaderResizeControl.h
//  Lithium Console
//
//  Created by James Wilson on 14/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorItem.h"

@interface LCInspectorHeaderResizeControl : NSView 
{
	IBOutlet LCInspectorItem *item;
	NSPoint oldPoint;
	BOOL active;
}

@end
