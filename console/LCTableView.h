//
//  LCTableView.h
//  Lithium Console
//
//  Created by James Wilson on 4/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
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
	
	/* Settings */
	BOOL allowDeleteKey;
}

@property (retain) NSArrayController *tableContentController;
@property (retain) NSString *tableContentKey;
@property (retain) id doubleClickTarget;
@property (retain) NSString *doubleClickTargetKey;
@property (retain) NSString *doubleClickTargetSelectorName;
@property (retain) id doubleClickArgument;
@property (retain) NSString *doubleClickArgumentKey;
@property (nonatomic, assign) BOOL allowDeleteKey;

@end
