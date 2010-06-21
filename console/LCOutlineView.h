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

@property (nonatomic,retain) NSArrayController *outlineContentController;
@property (nonatomic,retain) NSString *outlineContentKey;
@property (nonatomic,retain) id doubleClickTarget;
@property (nonatomic,retain) NSString *doubleClickTargetKey;
@property (nonatomic,retain) NSString *doubleClickTargetSelectorName;
@property (nonatomic,retain) id doubleClickArgument;
@property (nonatomic,retain) NSString *doubleClickArgumentKey;

#pragma mark "Item Expansion"
@property (nonatomic,copy) NSString *expansionPreferencePrefix;
- (void) expandAllItemsUsingPreferences;
- (BOOL) expandByDefault;

@end
