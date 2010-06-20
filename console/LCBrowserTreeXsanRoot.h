//
//  LCBrowserTreeXsanRoot.h
//  Lithium Console
//
//  Created by James Wilson on 30/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCBrowserTreeItemRoot.h"

#import "LCCustomer.h"

@interface LCBrowserTreeXsanRoot : LCBrowserTreeItemRoot 
{
	/* Items */
	NSMutableArray *items;
}

#pragma mark "Constructors"
- (LCBrowserTreeXsanRoot *) init;
- (void) dealloc;

#pragma mark "Accessors"
@property (readonly) NSMutableArray *items;
- (void) insertObject:(id)obj inItemsAtIndex:(unsigned int)index;
- (void) removeObjectFromItemsAtIndex:(unsigned int)index;

@end
