//
//  LCActionList.h
//  Lithium Console
//
//  Created by James Wilson on 1/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCXMLRequest.h"
#import "LCAction.h"
#import "LCXMLObjectList.h"

@interface LCActionList : LCXMLObjectList 
{
	/* Related Objects */
	id customer;
}

#pragma mark "Constructors"
+ (LCActionList *) actionListForCustomer:(LCCustomer *)initCustomer;
- (LCActionList *) initWithCustomer:(LCCustomer *)initCustomer;

#pragma mark "Accessor Methods"
@property (nonatomic,retain) id customer;

@end
