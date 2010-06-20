//
//  LCObjectTreeHeaderItem.h
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCObjectTreeItem.h"

@interface LCObjectTreeHeaderItem : LCObjectTreeItem 
{
	
}

- (id) initWithObject:(LCObject *)initObject showAllMetrics:(BOOL)showAll;

@end
