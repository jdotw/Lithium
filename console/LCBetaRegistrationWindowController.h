//
//  LCBetaRegistrationWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 19/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LCBetaRegistrationWindowController : NSWindowController 
{
	/* User-Entered Fields */
	NSString *firstname;
	NSString *lastname;
	NSString *company;
	NSString *email;
	NSString *confirmEmail;
	BOOL hasiPhone;
	NSString *iphoneid;
	BOOL hasLicense;	
	NSString *status;
	
	/* UI Control Properties */
	NSString *topTabTag;
	BOOL shouldShow;
	
	/* UI Elements */
	IBOutlet NSTabView *topTabView;
	IBOutlet NSButton *okButton;
	IBOutlet NSTextField *firstnameTextField;
	IBOutlet NSTextField *lastnameTextField;
	IBOutlet NSTextField *companyTextField;
	IBOutlet NSTextField *emailTextField;
	IBOutlet NSTextField *confirmEmailTextField;
	IBOutlet NSTextField *iphoneidTextField;
	IBOutlet NSObjectController *controllerAlias;
	
	/* URL Request */
	NSURLConnection *urlConn;
	NSMutableData *receivedData;	
}

#pragma mark "Initialisation"
- (id) initForBrowserWindow:(NSWindow *)windowForSheet;

#pragma mark "UI Actions"
- (IBAction) registerClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;

#pragma mark "Send Registration"
- (void) sendRegistration;

#pragma mark "Accessors"
@property (nonatomic,copy) NSString *firstname;
@property (nonatomic,copy) NSString *lastname;
@property (nonatomic,copy) NSString *company;
@property (nonatomic,copy) NSString *email;
@property (nonatomic,copy) NSString *confirmEmail;
@property (nonatomic, assign) BOOL hasiPhone;
@property (nonatomic,copy) NSString *iphoneid;
@property (nonatomic, assign) BOOL hasLicense;
@property (nonatomic,copy) NSString *status;
@property (nonatomic,copy) NSString *topTabTag;
@property (nonatomic, assign) BOOL shouldShow;

@end
