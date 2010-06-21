//
//  LCAuthenticator.h
//  Lithium Console
//
//  Created by James Wilson on 22/09/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import <Security/Security.h>
#import <CoreFoundation/CoreFoundation.h>
#import "LCBackgroundView.h"

@interface LCAuthenticator : NSObject 
{
	/* Authentication vars */
	NSString *username;
	NSString *password;
	id customer;

	/* User Interface */
	BOOL nibloaded;
	IBOutlet NSWindow *window;
	IBOutlet NSTextField *infofield;
	IBOutlet NSTextField *custfield;
	IBOutlet NSTextField *unamefield;
	IBOutlet NSTextField *pwordfield;
	IBOutlet NSButton *okbutton;
	IBOutlet NSButton *cancelbutton;
	IBOutlet NSButton *disablebutton;
	IBOutlet NSButton *addToKeychainSwitch;
	IBOutlet NSButton *useAsDefaultSwitch;
	IBOutlet LCBackgroundView *backgroundView;
	
	/* Op variables */
	BOOL retryreturn;
}

#pragma mark "Class Methods"
+ (NSString *) defaultUsername;
+ (void) setDefaultUsername:(NSString *)string;
+ (NSString *) defaultPassword;
+ (void) setDefaultPassword:(NSString *)string;

#pragma mark "Initialisation"
+ (LCAuthenticator *) authForCustomer:(id)initCustomer;
- (LCAuthenticator *) initForCustomer:(id)initCustomer;
- (LCAuthenticator *) init;

#pragma mark "Authentication Methods"
- (void) retrieveAuth; 
- (BOOL) retryAuth;

#pragma mark "Keychain Methods"
- (BOOL) checkKeychainItemExists;
- (BOOL) deleteKeychainItem;
- (BOOL) addKeychainItem;
- (void) setCredentialsFromKeychain;

#pragma mark "User interface methods"
- (void) promptUser:(BOOL)retry;
- (IBAction) disableClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;
- (IBAction) okClicked:(id)sender;

#pragma mark "Accessor Methods"
- (NSString *) username;
- (NSString *) password;
- (id) customer;
- (void) setCustomer:(id)entity;

@property (nonatomic,retain) NSString *username;
@property (nonatomic,retain) NSString *password;
@property BOOL nibloaded;
@property (nonatomic,retain) NSWindow *window;
@property (nonatomic,retain) NSTextField *infofield;
@property (nonatomic,retain) NSTextField *custfield;
@property (nonatomic,retain) NSTextField *unamefield;
@property (nonatomic,retain) NSTextField *pwordfield;
@property (nonatomic,retain) NSButton *okbutton;
@property (nonatomic,retain) NSButton *cancelbutton;
@property (nonatomic,retain) NSButton *disablebutton;
@property (nonatomic,retain) NSButton *addToKeychainSwitch;
@property (nonatomic,retain) NSButton *useAsDefaultSwitch;
@property (nonatomic,retain) LCBackgroundView *backgroundView;
@property BOOL retryreturn;
@end
