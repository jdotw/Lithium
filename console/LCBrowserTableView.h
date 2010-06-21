//
//  LCBrowserTableView.h
//  Lithium Console
//
//  Created by James Wilson on 14/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCTableView.h"

@interface LCBrowserTableView : LCTableView 
{
	NSColor *backgroundTintColor;
	BOOL roundedSelection;
	BOOL denySelection;
}

- (NSColor *) backgroundTintColor;
- (void) setBackgroundTintColor:(NSColor *)value;

- (NSBezierPath *) outlinePath:(NSRect)frame;
- (BOOL) roundedSelection;
- (void) setRoundedSelection:(BOOL)value;

#pragma mark "Menu Event Handling"
- (NSMenu *) menuForEvent:(NSEvent *)event;

@property (retain,getter=backgroundTintColor,setter=setBackgroundTintColor:) NSColor *backgroundTintColor;
@property (getter=roundedSelection,setter=setRoundedSelection:) BOOL roundedSelection;
@property (nonatomic, assign) BOOL denySelection;
@end
