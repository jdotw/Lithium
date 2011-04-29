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
	NSString *accessLevelString;
	int accessLevel;
	BOOL isGlobalAdmin;
	
	id customer;
}

#pragma mark "Properties Accessors"
@property (nonatomic,copy) NSString *username;
@property (nonatomic,copy) NSString *fullname;
@property (nonatomic,copy) NSString *password;
@property (nonatomic,copy) NSString *confirmPassword;
@property (nonatomic,copy) NSString *accessLevelString;
@property (nonatomic, assign) int accessLevel;
@property (nonatomic, assign) BOOL isGlobalAdmin;
@property (nonatomic, assign) id customer;

@end
