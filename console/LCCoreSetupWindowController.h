//
//  LCCoreSetupWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 29/03/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCLicense.h"
#import "LCDemoLicense.h"

@class LCDemoLicense, LCLicense, LCXMLRequest, LCCustomer, LCXMLRequest;

@interface LCCoreSetupWindowController : NSWindowController <LCLicenseDelegate, LCDemoLicenseDelegate>
{
	/* Customer */
	LCCustomer *customer;

	/* UI Elements */
	IBOutlet NSTabView *tabView;
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet NSComboBox *demoEmailComboBox;
	IBOutlet NSComboBox *licEmailComboBox;
	IBOutlet NSButton *nextButton;
	IBOutlet NSButton *backButton;
	IBOutlet NSButton *cancelButton;

	/* License */
	LCDemoLicense *demoLicense;
	LCLicense *existingLicense;

	/* Status */
	NSString *status;
	BOOL operationInProgress;
	BOOL canGoBack;
	BOOL canGoNext;
	
	/* License Tab */
	int licenseTypeIndex;
	NSString *demoName;
	NSString *demoEmail;
	NSString *demoCompany;
	NSString *licenseKey;
	
	/* License Activation */
	NSString *licName;
	NSString *licLastName;
	NSString *licEmail;
	NSString *licCompany;
	
	/* Licennse key ivar */
	NSString *signedLicenseKey;	// This is the key sent to Lithium Core
	
	/* Auth Tab */
	int authTypeIndex;
	NSString *authUsername;
	NSString *authPassword;
	NSString *authPasswordConfirm;
	
	/* XML Request */
	LCXMLRequest *coreSetupRequest;
}

- (id) initWithCustomer:(LCCustomer *)initCustomer;

- (IBAction) cancelClicked:(id)sender;
- (IBAction) backClicked:(id)sender;
- (IBAction) nextClicked:(id)sender;

@property (nonatomic,retain) NSString *status;
@property (nonatomic,assign) BOOL operationInProgress;

@property (nonatomic,assign) int licenseTypeIndex;
@property (nonatomic,retain) NSString *demoName;
@property (nonatomic,retain) NSString *demoEmail;
@property (nonatomic,retain) NSString *demoCompany;
@property (nonatomic,retain) NSString *licenseKey;

@property (nonatomic,retain) NSString *licName;
@property (nonatomic,retain) NSString *licEmail;
@property (nonatomic,retain) NSString *licCompany;

@property (nonatomic,assign) int authTypeIndex;
@property (nonatomic,retain) NSString *authUsername;
@property (nonatomic,retain) NSString *authPassword;
@property (nonatomic,retain) NSString *authPasswordConfirm;

@property (nonatomic,assign) BOOL canGoBack;
@property (nonatomic,assign) BOOL canGoNext;

@property (nonatomic,retain) LCCustomer *customer;

@end
