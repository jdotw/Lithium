//
//  LCActionHistoryList.h
//  Lithium Console
//
//  Created by James Wilson on 3/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCXMLRequest.h"
#import "LCActionLogEntry.h"
#import "LCXMLObjectList.h"

@interface LCActionHistoryList : LCXMLObjectList 
{
	/* Related objects */
	id action;
	
	/* XML Variables */
	BOOL hasBeenRefreshed;
}

+ (LCActionHistoryList *) actionHistoryListForAction:(id)initAction;
- (id) initWithAction:(id)initAction;

#pragma mark "Accessor Methods"
@property (assign) id action;
@property (assign) BOOL hasBeenRefreshed;

@end
