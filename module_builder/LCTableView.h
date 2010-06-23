//
//  LCTableView.h
//  Lithium Console
//
//  Created by James Wilson on 4/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LCTableView : NSTableView 
{
	/* tableContent binding */
	NSArrayController *tableContentController;
	NSString *tableContentKey;
	
	/* doubleClickTarget binding */
	id doubleClickTarget;
	NSString *doubleClickTargetKey;
	NSString *doubleClickTargetSelectorName;
	
	/* doubleClickArgument binding */
	id doubleClickArgument;
	NSString *doubleClickArgumentKey;
}

@end
