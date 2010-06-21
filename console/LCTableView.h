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

@property (nonatomic,retain) NSArrayController *tableContentController;
@property (nonatomic,retain) NSString *tableContentKey;
@property (nonatomic,retain) id doubleClickTarget;
@property (nonatomic,retain) NSString *doubleClickTargetKey;
@property (nonatomic,retain) NSString *doubleClickTargetSelectorName;
@property (nonatomic,retain) id doubleClickArgument;
@property (nonatomic,retain) NSString *doubleClickArgumentKey;
@property (nonatomic, assign) BOOL allowDeleteKey;

@end
