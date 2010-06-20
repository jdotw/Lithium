//
//  LCBrowserTreeCoreProperty.h
//  Lithium Console
//
//  Created by James Wilson on 3/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowserTreeItem.h"
#import "LCCustomer.h"

@interface LCBrowserTreeCoreProperty : LCBrowserTreeItem 
{
	LCCustomer *customer;
	Class contentControllerClass;
}

- (LCBrowserTreeCoreProperty *) initWithCustomer:(LCCustomer *)initCustomer;
@property (retain) LCCustomer *customer;
@property (assign) Class contentControllerClass;

@end
