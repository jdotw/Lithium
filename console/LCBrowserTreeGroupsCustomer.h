//
//  LCBrowserTreeGroupsCustomer.h
//  Lithium Console
//
//  Created by James Wilson on 9/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowserTreeItem.h"
#import "LCCustomer.h"

@interface LCBrowserTreeGroupsCustomer : LCBrowserTreeItem 
{
	LCCustomer *customer;
}

@property (nonatomic,retain) LCCustomer *customer;

@end
