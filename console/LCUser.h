//
//  LCUser.h
//  Lithium Console
//
//  Created by James Wilson on 7/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCXMLObject.h"

@interface LCUser : LCXMLObject 
{
	NSString *username;
	NSString *fullname;
	NSString *password;
	NSString *confirmPassword;
	NSString *title;
	NSString *email;
	NSString *accessLevelString;
	int accessLevel;
	BOOL isGlobalAdmin;
	
	id customer;
}

#pragma mark "Properties Accessors"
@property (copy) NSString *username;
@property (copy) NSString *fullname;
@property (copy) NSString *password;
@property (copy) NSString *confirmPassword;
@property (copy) NSString *title;
@property (copy) NSString *email;
@property (copy) NSString *accessLevelString;
@property (assign) int accessLevel;
@property (assign) BOOL isGlobalAdmin;
@property (assign) id customer;

@end
