//
//  LCOutlineView.h
//  Lithium Console
//
//  Created by James Wilson on 26/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCOutlineView : NSOutlineView 
{
	/* outlineView binding */
	NSArrayController *outlineContentController;
	NSString *outlineContentKey;
	
	/* doubleClickTarget binding */
	id doubleClickTarget;
	NSString *doubleClickTargetKey;
	NSString *doubleClickTargetSelectorName;
	
	/* doubleClickArgument binding */
	id doubleClickArgument;
	NSString *doubleClickArgumentKey;	
	
	/* Expansion */
	NSString *expansionPreferencePrefix;
}

@property (retain) NSArrayController *outlineContentController;
@property (retain) NSString *outlineContentKey;
@property (retain) id doubleClickTarget;
@property (retain) NSString *doubleClickTargetKey;
@property (retain) NSString *doubleClickTargetSelectorName;
@property (retain) id doubleClickArgument;
@property (retain) NSString *doubleClickArgumentKey;

#pragma mark "Item Expansion"
@property (copy) NSString *expansionPreferencePrefix;
- (void) expandAllItemsUsingPreferences;
- (BOOL) expandByDefault;

@end
