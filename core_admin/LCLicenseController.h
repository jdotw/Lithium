//
//  LCLicenseController.h
//  LCAdminTools
//
//  Created by James Wilson on 25/09/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCConfigController.h"
#import "LCCustomer.h"
#import "LCCustomerController.h"

@interface LCLicenseController : NSObject 
{
	/* UI Elements */
	IBOutlet NSWindow *window;
	IBOutlet NSArrayController *keyArrayController;
	IBOutlet NSArrayController *licenseArrayController;
	IBOutlet LCConfigController *configController;
	IBOutlet LCCustomerController *customerController;

	/* Licenses */
	NSMutableArray *licenses;
	NSMutableDictionary *licenseDict;
	NSArray *licenseSortDescriptor;
	
	/* License Add */
	IBOutlet NSWindow *addKeySheet;
	IBOutlet NSTabView *tabView;
	IBOutlet NSTextField *firstnameTextField;
	IBOutlet NSTextField *lastnameTextField;
	IBOutlet NSTextField *companyTextField;
	IBOutlet NSTextField *emailTextField;
	IBOutlet NSScrollView *keyScrollView;
	NSString *licenseKeyString;
	NSString *signedKeyString;
	
	/* Activation */
	NSString *firstname;
	NSString *lastname;
	NSString *company;
	NSString *email;
	LCCustomer *customer;
	NSURLConnection *urlConn;
	NSMutableData *receivedData;
	NSMutableString *xmlString;
	BOOL keyAlreadyActivated;
	BOOL keyIsBoundToCustomer;
	BOOL xmlOperationInProgress;	
	NSString *progressString;
	NSString *errorMessage;
	NSString *licenseType;
	int result;
}

- (void) refreshLicenseKeyList;

- (IBAction) addNewKeyClicked:(id)sender;
- (IBAction) addSheetAddKeyClicked:(id)sender;
- (void) addSheetAddCustomerClicked:(id)sender;
- (void) addSheetActivateClicked:(id)sender;
- (IBAction) cancelAddClicked:(id)sender;
- (IBAction) deleteSelectedKeyClicked:(id)sender;

@property (readonly) NSMutableArray *licenses;
- (void) insertObject:(id)obj inLicensesAtIndex:(unsigned int)index;
- (void) removeObjectFromLicensesAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *licenseDict;
@property (copy) NSString *firstname;
@property (copy) NSString *lastname;
@property (copy) NSString *company;
@property (copy) NSString *email;
@property (assign) BOOL keyAlreadyActivated;
@property (copy) NSString *licenseKeyString;
@property (copy) NSString *signedKeyString;	
@property (assign) BOOL xmlOperationInProgress;	
@property (copy) NSString *progressString;
@property (copy) NSString *errorMessage;
@property (assign) int result;
@property (copy) NSString *licenseType;
@property (assign) BOOL keyIsBoundToCustomer;
@property (retain) LCCustomer *customer;

@end
