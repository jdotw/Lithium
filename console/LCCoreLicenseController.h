//
//  LCCoreLicenseWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 6/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"
#import "LCCoreLicenseKeyList.h"
#import "LCCoreLicenseEntitlement.h"
#import "LCLithiumSetupWindowController.h"

@interface LCCoreLicenseController : NSObject 
{
	/* Related objects */
	IBOutlet LCLithiumSetupWindowController *setupController;
	
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
	IBOutlet NSWindow *addKeySheet;
	IBOutlet NSWindow *removeKeySheet;
	IBOutlet NSArrayController *keyArrayController;
	IBOutlet NSTabView *tabView;
	IBOutlet NSTextField *firstnameTextField;
	IBOutlet NSTextField *lastnameTextField;
	IBOutlet NSTextField *companyTextField;
	IBOutlet NSTextField *emailTextField;
	
	/* XML Ops */
	LCXMLRequest *addXMLRequest;
	LCXMLRequest *removeXMLRequest;
	NSString *xmlElement;
	NSMutableString *xmlString;
	BOOL xmlOperationInProgress;
	NSString *progressString;
	NSString *errorMessage;
	NSString *addResult;
	LCActivity *activity;
	NSURLConnection *urlConn;
	NSMutableData *receivedData;
}

#pragma mark "Add License Key"
- (IBAction) addLicenseKeyClicked:(id)sender;
- (IBAction) closeAddLicenseKeySheet:(id)sender;
- (IBAction) addLicenseKeySheetClicked:(id)sender;
- (IBAction) activateLicenseKeySheetClicked:(id)sender;
- (void) activateLicenseKey;
- (void) uploadSignedKeyToCore;

#pragma mark "Remove License Key"
- (IBAction) removeLicenseKeyClicked:(id)sender;
- (IBAction) removeSheetRemoveClicked:(id)sender;
- (IBAction) removeSheetCancelClicked:(id)sender;
- (IBAction) privacyClicked:(id)sender;

#pragma mark "Purchase Online"
- (IBAction) purchaseOnlineClicked:(id)sender;

#pragma mark "Get Demo License Clicked"
- (IBAction) getDemoLicenseClicked:(id)sender;

#pragma mark "Accessors"
@property (nonatomic,retain) LCCoreLicenseKeyList *keyList;
@property (nonatomic,retain) LCCoreLicenseEntitlement *entitlement;
@property (nonatomic,copy) NSString *licenseKeyString;
@property (nonatomic,copy) NSString *signedKeyString;
@property (nonatomic,copy) NSString *firstname;
@property (nonatomic,copy) NSString *lastname;
@property (nonatomic,copy) NSString *company;
@property (nonatomic,copy) NSString *email;
@property (nonatomic, assign) BOOL xmlOperationInProgress;
@property (nonatomic,copy) NSString *progressString;
@property (nonatomic,copy) NSString *errorMessage;
@property (nonatomic,copy) NSString *addResult;

@end
