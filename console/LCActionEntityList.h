//
//  LCActionEntityList.h
//  Lithium Console
//
//  Created by James Wilson on 25/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXMLRequest.h"
#import "LCXMLObjectList.h"

@interface LCActionEntityList : LCXMLObjectList 
{
	/* Related objects */
	id action;
}

- (id) initWithAction:(id)initAction;

#pragma mark "Accessor Methods"
@property (assign) id action;

@end
