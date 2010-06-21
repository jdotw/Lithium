//
//  LCUserList.h
//  Lithium Console
//
//  Created by James Wilson on 7/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCXMLRequest.h"

@interface LCUserList : NSObject 
{
	/* Related Objects */
	id customer;

	/* Users */
	NSMutableArray *users;
	NSMutableDictionary *userDictionary;
	
	/* Refresh variables */
	LCXMLRequest *refreshXMLRequest;
	NSMutableDictionary *curXMLDictionary;
	NSMutableString *curXMLString;
	NSMutableDictionary *xmlUserDictionary;
	BOOL refreshInProgress;	
}

#pragma mark Constructors
+ (LCUserList *) userListWithCustomer:(id)initCustomer;
- (id) initWithCustomer:(id)initCustomer;

#pragma mark "Refresh methods"
- (void) refreshWithPriority:(int)priority;

#pragma mark "User Methods"
@property (readonly) NSMutableArray *users;
- (void) insertObject:(id)user inUsersAtIndex:(unsigned int)index;
- (void) removeObjectFromUsersAtIndex:(unsigned int) index;
@property (readonly) NSMutableDictionary *userDictionary;

#pragma mark "General Properties"
@property (nonatomic, assign) BOOL refreshInProgress;
@property (nonatomic,retain) id customer;

@end
