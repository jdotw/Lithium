//
//  LCServiceList.h
//  Lithium Console
//
//  Created by James Wilson on 9/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXMLObjectList.h"

@interface LCServiceList : LCXMLObjectList 
{
	/* Customer */
	id customer;	
}

#pragma mark "Constructors"
+ (LCServiceList *) serviceListForCustomer:(id)initCustomer;
- (id) initWithCustomer:(id)initCustomer;
	
#pragma mark "Properties"
@property (nonatomic,retain) id customer;

@end
