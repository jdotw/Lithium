//
//  LCBrowserTreeCoreCustomer.h
//  Lithium Console
//
//  Created by James Wilson on 3/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowserTreeItem.h"
#import "LCCustomer.h"

@interface LCBrowserTreeCoreCustomer : LCBrowserTreeItem 
{
	LCCustomer *customer;
}

- (LCBrowserTreeCoreCustomer *) initWithCustomer:(LCCustomer *)initCustomer;
@property (retain) LCCustomer *customer;

@end
