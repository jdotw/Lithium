//
//  LCBrowser2ObjectOutlineView.h
//  Lithium Console
//
//  Created by James Wilson on 16/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCOutlineView.h"

@interface LCBrowser2ObjectOutlineView : LCOutlineView 
{
	NSColor *backgroundTintColor;
}

@property (retain,getter=backgroundTintColor,setter=setBackgroundTintColor:) NSColor *backgroundTintColor;
@end
