//
//  LCProcessProfileList.h
//  Lithium Console
//
//  Created by James Wilson on 16/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXMLRequest.h"
#import "LCXMLObjectList.h"

@interface LCProcessProfileList : LCXMLObjectList 
{
}

#pragma mark "Constructors"
+ (LCProcessProfileList *) profileListForCustomer:(id)initCustomer;
- (LCProcessProfileList *) initForCustomer:(id)initCustomer;

@end
