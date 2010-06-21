//
//  LCDeviceTree.h
//  Lithium Console
//
//  Created by James Wilson on 4/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"
#import "LCSite.h"
#import "LCDevice.h"

@interface LCDeviceTree : NSObject 
{
	/* Target */
	LCSite *site;
	LCCustomer *customer;
	
	/* Items */
	NSMutableArray *items;
}

- (id) initWithSite:(LCSite *)initSite;
- (id) initWithCustomer:(LCCustomer *)initCustomer;

@property (nonatomic,retain) LCSite *site;
@property (nonatomic,retain) LCCustomer *customer;
@property (readonly) NSMutableArray *items;
- (void) insertObject:(id)item inItemsAtIndex:(unsigned int)index;
- (void) removeObjectFromItemsAtIndex:(unsigned int)index;

@end
