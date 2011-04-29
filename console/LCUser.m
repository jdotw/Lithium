//
//  LCUser.m
//  Lithium Console
//
//  Created by James Wilson on 7/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCUser.h"


@implementation LCUser

#pragma mark "Constructors"

- (LCUser *) init
{
	[super init];
	
	/* Setup XML Translation */
	if (!self.xmlTranslation)
	{ self.xmlTranslation = [NSMutableDictionary dictionary]; }
	[self.xmlTranslation setObject:@"username" forKey:@"username"];
	[self.xmlTranslation setObject:@"password" forKey:@"password"];
	[self.xmlTranslation setObject:@"confirmPassword" forKey:@"confirm_password"];
	[self.xmlTranslation setObject:@"fullname" forKey:@"fullname"];
	[self.xmlTranslation setObject:@"accessLevel" forKey:@"level_num"];
	[self.xmlTranslation setObject:@"isGlobalAdmin" forKey:@"global_admin"];
	
	return self;
}

- (void) dealloc
{
	[username release];
	[fullname release];
	[super dealloc];
}

#pragma mark "XML Methods"

- (NSString *) xmlRootElement
{ return @"user"; }

#pragma mark "Properties Accessors"

@synthesize username;
@synthesize password;
@synthesize confirmPassword;
@synthesize fullname;
@synthesize accessLevel;
- (void) setAccessLevel:(int)value
{
	accessLevel = value;
	switch (accessLevel)
	{
		case 20:
			self.accessLevelString = @"Read-Only";
			break;
		case 30:
			self.accessLevelString = @"Normal User";
			break;
		case 40:
			self.accessLevelString = @"Administrator";
			break;
		case 1024:
			self.accessLevelString = @"Global Admin";
			break;
		defualt:
			self.accessLevelString = @"Other";
	}
}
@synthesize accessLevelString;
@synthesize isGlobalAdmin;
@synthesize customer;

@end
