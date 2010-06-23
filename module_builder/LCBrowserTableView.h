//
//  LCBrowserTableView.h
//  Lithium Console
//
//  Created by James Wilson on 14/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCTableView.h"

@interface LCBrowserTableView : LCTableView 
{
	NSColor *backgroundTintColor;
	BOOL roundedSelection;
}

- (NSColor *) backgroundTintColor;
- (void) setBackgroundTintColor:(NSColor *)value;

- (BOOL) roundedSelection;
- (void) setRoundedSelection:(BOOL)value;

#pragma mark "Menu Event Handling"
- (NSMenu *) menuForEvent:(NSEvent *)event;

@end
