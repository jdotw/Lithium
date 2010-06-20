//
//  LCBrowserLicenseContentController.h
//  Lithium Console
//
//  Created by James Wilson on 8/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2Controller.h"
#import "LCBrowser2ContentViewController.h"
#import "LCCustomer.h"
#import "LCCoreLicenseKey.h"
#import "LCCoreLicenseKeyList.h"
#import "LCCoreLicenseEntitlement.h"

@interface LCBrowserLicenseContentController : LCBrowser2ContentViewController 
{
	/* Objects */
	LCCustomer *customer;
	LCBrowser2Controller *browser;
	
	/* Licenses */
	LCCoreLicenseKeyList *keyList;
	LCCoreLicenseEntitlement *entitlement;	
	
	/* Key String */
	NSString *licenseKeyString;
	NSString *signedKeyString;
	
	/* Registration Info */
	NSString *firstname;
	NSString *lastname;
	NSString *company;
	NSString *email;	
	
	/* UI Elements */
	IBOutlet NSArrayController *keyArrayController;
	IBOutlet NSWindow *addKeySheet;
	IBOutlet NSTabView *tabView;
	IBOutlet NSTextField *firstnameTextField;
	IBOutlet NSTextField *lastnameTextField;
	IBOutlet NSTextField *companyTextField;
	IBOutlet NSTextField *emailTextField;
	IBOutlet NSScrollView *keyScrollView;
	
	/* Selection */
	LCCoreLicenseKey *selectedKey;
	NSArray *selectedKeys;
	
	/* XML Operation Properties */
	LCXMLRequest *addXMLRequest;
	LCXMLRequest *removeXMLRequest;
	BOOL xmlOperationInProgress;
	NSString *progressString;
	NSString *errorMessage;
	NSString *addResult;
	LCActivity *activity;
	NSURLConnection *urlConn;
	NSMutableData *receivedData;
	NSMutableString *xmlString;
}

#pragma mark "Constructors"
- (id) initWithCustomer:(LCCustomer *)initCustomer inBrowser:(LCBrowser2Controller *)initBrowser;

#pragma mark "Selection (KVO Observable)"
@property (assign) LCCoreLicenseKey *selectedKey;
@property (copy) NSArray *selectedKeys;

#pragma mark "Misc UI Actions"
- (IBAction) refreshClicked:(id)sender;

#pragma mark "Add License Key"
- (IBAction) addLicenseKeyClicked:(id)sender;
- (IBAction) closeAddLicenseKeySheet:(id)sender;
- (IBAction) addLicenseKeySheetClicked:(id)sender;
- (IBAction) activateLicenseKeySheetClicked:(id)sender;
- (void) activateLicenseKey;
- (void) uploadSignedKeyToCore;

#pragma mark "Remove License Key"
- (IBAction) removeLicenseKeyClicked:(id)sender;
- (IBAction) privacyClicked:(id)sender;

#pragma mark "Purchase Online"
- (IBAction) purchaseOnlineClicked:(id)sender;

#pragma mark "Get Demo License Clicked"
- (IBAction) getDemoLicenseClicked:(id)sender;

#pragma mark "Properties"
@property (assign) LCBrowser2Controller *browser;
@property (retain) LCCustomer *customer;
@property (retain) LCCoreLicenseKeyList *keyList;
@property (retain) LCCoreLicenseEntitlement *entitlement;
@property (copy) NSString *licenseKeyString;
@property (copy) NSString *signedKeyString;
@property (copy) NSString *firstname;
@property (copy) NSString *lastname;
@property (copy) NSString *company;
@property (copy) NSString *email;
@property (assign) BOOL xmlOperationInProgress;
@property (copy) NSString *progressString;
@property (copy) NSString *errorMessage;
@property (copy) NSString *addResult;


@end
