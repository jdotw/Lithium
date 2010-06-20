//
//  LCAuthenticator.m
//  Lithium Console
//
//  Created by James Wilson on 22/09/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCAuthenticator.h"
#import "LCConsoleController.h"

@implementation LCAuthenticator

#pragma mark "Class Methods"

static NSString *defaultUsername;
static NSString *defaultPassword;

+ (NSString *) defaultUsername
{ return defaultUsername; }
+ (void) setDefaultUsername:(NSString *)string
{ 
	[defaultUsername release];
	defaultUsername = [string retain];
}	

+ (NSString *) defaultPassword
{ return defaultPassword; }
+ (void) setDefaultPassword:(NSString *)string
{
	[defaultPassword release];
	defaultPassword = [string retain];
}

#pragma mark "Initialisation"

+ (LCAuthenticator *) authForCustomer:(id)initCustomer
{
	return [[[LCAuthenticator alloc] initForCustomer:initCustomer] autorelease];
}

- (LCAuthenticator *) initForCustomer:(id)initCustomer
{
	[self init];
	[self setCustomer:initCustomer];
	[self retrieveAuth];
	return self;
}

- (LCAuthenticator *) init
{
	[super init];
	nibloaded = NO;
	return self;
}

- (void) dealloc
{
	if (username) [username release];
	if (password) [password release];
	if (customer) [customer release];
	[super dealloc];
}

#pragma mark "Authentication Methods"

- (void) retrieveAuth
{
	/* Retrieve authentcation, prompt if necessary */
	
	/* Check for demo mode */
#ifdef DEMO_MODE
	username = @"admin";
	password = @"lcp22905";
	return;
#endif
	
	/* Check to see if customer has auth credentials set */
	if ([customer username] && [customer password])
	{
		/* Use customers cache credentials */
		username = [[customer username] retain];
		password = [[customer password] retain];
	}
	else
	{
		/* Check if keychain item exists */
		if ([self checkKeychainItemExists] == YES)
		{
			/* Password is in keychain */
			[self setCredentialsFromKeychain];
			
			/* Update customer */
			[customer setUsername:[self username]];
			[customer setPassword:[self password]];
		}
		else
		{
			/* Prompt user for auth */
			[self promptUser:NO];
		}
	}
}

- (BOOL) retryAuth
{
	/* Prompt for authentication retry */
	[self promptUser:YES];
	return retryreturn;
}

#pragma mark "Keychain Methods"

- (BOOL) checkKeychainItemExists
{
	BOOL found;
	
	NSURL *url = [NSURL URLWithString:[customer url]];
	OSStatus status = SecKeychainFindInternetPassword (NULL,[[url host] length],[[url host] cStringUsingEncoding:NSUTF8StringEncoding],0,NULL,0,NULL,[[url path] length],[[url path] cStringUsingEncoding:NSUTF8StringEncoding],(uint16) [[url port] unsignedIntValue],kSecProtocolTypeHTTP,kSecAuthenticationTypeDefault,NULL,NULL,NULL);
	if (status == 0)
	{ found = YES; }
	else
	{ found = NO; }
	
	return found;
}

- (BOOL) deleteKeychainItem
{
    SecKeychainItemRef item;
	BOOL deleted;
	
	NSURL *url = [NSURL URLWithString:[customer url]];
	OSStatus status = SecKeychainFindInternetPassword (NULL,[[url host] length],[[url host] cStringUsingEncoding:NSUTF8StringEncoding],0,NULL,0,NULL,[[url path] length],[[url path] cStringUsingEncoding:NSUTF8StringEncoding],(uint16) [[url port] unsignedIntValue],kSecProtocolTypeHTTP,kSecAuthenticationTypeDefault,NULL,NULL,&item);
	if (status == 0)
	{
		/* Item found */
		status = SecKeychainItemDelete (item);
		if (status == 0)
		{ deleted = YES; }
		else
		{ deleted = NO;	}
	}
	else
	{ deleted = NO; }
		
	return deleted;
}

- (BOOL) addKeychainItem
{
    OSStatus status;
	BOOL created;
	
	NSURL *url = [NSURL URLWithString:[customer url]];
	status = SecKeychainAddInternetPassword(NULL,[[url host] length],[[url host] cStringUsingEncoding:NSUTF8StringEncoding],0,NULL,[username length],[username cStringUsingEncoding:NSUTF8StringEncoding],[[url path] length],[[url path] cStringUsingEncoding:NSUTF8StringEncoding],(uint16) [[url port] unsignedIntValue],kSecProtocolTypeHTTP,kSecAuthenticationTypeDefault,[password length],[password cStringUsingEncoding:NSUTF8StringEncoding],NULL);
    if (status == 0) 
	{ 
		created = YES; 
	}
	else
	{
		created = NO;
	}
	
	return created;
}

- (void) setCredentialsFromKeychain
{
	/* Set username/password ivars from keychain item */
    SecKeychainItemRef item;
    OSStatus status;
	void *pwordbuf;
	UInt32 pwordbuf_len;
	char tempstr[1024];
	
	NSURL *url = [NSURL URLWithString:[customer url]];
	status = SecKeychainFindInternetPassword (NULL,[[url host] length],[[url host] cStringUsingEncoding:NSUTF8StringEncoding],0,NULL,0,NULL,[[url path] length],[[url path] cStringUsingEncoding:NSUTF8StringEncoding],(uint16) [[url port] unsignedIntValue],kSecProtocolTypeHTTP,kSecAuthenticationTypeDefault,&pwordbuf_len,&pwordbuf,&item);
	if (status == 0)
	{
		/* Item found */
		SecKeychainAttributeInfo info;
		SecKeychainAttributeList *list;
		UInt32 tag[1] = { kSecAccountItemAttr };
		UInt32 format[1] = { kSecFormatUnknown };
		
		/* Copy password */
		if (pwordbuf_len > 1023) 
		{ pwordbuf_len = 1023; }
		strncpy (tempstr, pwordbuf, pwordbuf_len);		
		tempstr[pwordbuf_len] = '\0';
		if (password) [password release];
		password = [[NSString stringWithCString:tempstr encoding:NSUTF8StringEncoding] retain];
		SecKeychainItemFreeContent(NULL, pwordbuf);
		
		/* Set attributes to copy */
		info.count = 1;
		info.tag = &tag[0];
		info.format = &format[0];
		
		/* Copy attributes */
		status = SecKeychainItemCopyAttributesAndData (item, &info, NULL, &list, NULL, NULL);
		if (status == 0)
		{
			if (list->attr[0].tag == kSecAccountItemAttr)
			{
				/* Set username ivar */
				strncpy (tempstr, list->attr[0].data, list->attr[0].length);
				tempstr[list->attr[0].length] = '\0';
				if (username) [username release];
				username = [[NSString stringWithCString:tempstr encoding:NSUTF8StringEncoding] retain];
			}
			SecKeychainItemFreeAttributesAndData(list, NULL);
		}
		
        CFRelease (item);
    }
	else
	{ NSLog (@"[LCAuthenticator setCredentialsFromKeychain] error %d from SecKeychainFindInternetPassword", (int) status); }
}

#pragma mark "User interface methods"

- (void) promptUser:(BOOL)retry
{
	[[LCConsoleController masterController] hideNotificationWindow];
	
	/* Load NIB */
	if (nibloaded == NO) 
	{ 
		nibloaded = [NSBundle loadNibNamed:@"AuthenticationRequest" owner:self];
	}
	
	/* Setup window */
	[backgroundView setImage:[NSImage imageNamed:@"slateback.png"]];
	if (retry == YES) 
	{ 
		if (username) [unamefield setStringValue:username];
		[infofield setStringValue:@"Authentication Failed - Try Again"];
		[infofield setTextColor:[NSColor redColor]];
		[window makeFirstResponder:pwordfield];
	}
	else
	{ 
		if ([LCAuthenticator defaultUsername]) [unamefield setStringValue:[LCAuthenticator defaultUsername]]; 
		else [unamefield setStringValue:@""]; 
		if ([LCAuthenticator defaultPassword]) [pwordfield setStringValue:[LCAuthenticator defaultPassword]]; 
		else [pwordfield setStringValue:@""]; 
		[infofield setStringValue:@"Authentication Request"];
		[infofield setTextColor:[NSColor whiteColor]];
		[window makeFirstResponder:unamefield];
	}
	if (password) [pwordfield setStringValue:@""];
	if (customer && [customer name]) [custfield setStringValue:[[customer name] uppercaseString]];
	[window setDefaultButtonCell:[okbutton cell]];
	[addToKeychainSwitch setState:NSOnState];
	[useAsDefaultSwitch setState:NSOnState];
	
	/* Begin modal session */
	[NSApp runModalForWindow:window];
}

- (IBAction) disableClicked:(id)sender
{
	/* Disable customer */
	[customer setDisabled:YES];
	
	/* Set retry return value */
	retryreturn = NO;
	
	/* End modal session */
	[NSApp stopModal];
	[window close];
}

- (IBAction) cancelClicked:(id)sender
{
	/* Cancel authentication clicked */	
	
	/* Set retry return value */
	retryreturn = NO;

	/* End modal session */
	[NSApp stopModal];
	[window close];
}

- (IBAction) okClicked:(id)sender
{
	/* OK authentication clicked */
	
	/* Delete existing item */
	if ([self checkKeychainItemExists])
	{ [self deleteKeychainItem]; }
	
	/* Get values from TextFields */
	username = [[unamefield stringValue] retain];
	password = [[pwordfield stringValue] retain];
	
	/* Update customer */
	[customer setUsername:username];
	[customer setPassword:password];
	
	/* Add new */
	if ([addToKeychainSwitch state] == NSOnState)
	{ [self addKeychainItem]; }

	/* Set default if asked */
	if ([useAsDefaultSwitch state] == NSOnState)
	{ 
		[LCAuthenticator setDefaultUsername:username];
		[LCAuthenticator setDefaultPassword:password];
	}
			
	/* Set retry return value */
	retryreturn = YES;
	
	/* End modal session */
	[NSApp stopModal];
	[window close];
}

#pragma mark "Accessor Methods"

- (NSString *) username 
{ return username; }
- (NSString *) password { return password; }

- (id) customer { return customer; }
- (void) setCustomer:(id)entity { customer = [entity retain]; }

@synthesize username;
@synthesize password;
@synthesize nibloaded;
@synthesize window;
@synthesize infofield;
@synthesize custfield;
@synthesize unamefield;
@synthesize pwordfield;
@synthesize okbutton;
@synthesize cancelbutton;
@synthesize disablebutton;
@synthesize addToKeychainSwitch;
@synthesize useAsDefaultSwitch;
@synthesize backgroundView;
@synthesize retryreturn;
@end
