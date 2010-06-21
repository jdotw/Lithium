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

@property (nonatomic,readonly) NSMutableArray *licenses;
- (void) insertObject:(id)obj inLicensesAtIndex:(unsigned int)index;
- (void) removeObjectFromLicensesAtIndex:(unsigned int)index;
@property (nonatomic,readonly) NSMutableDictionary *licenseDict;
@property (nonatomic,copy) NSString *firstname;
@property (nonatomic,copy) NSString *lastname;
@property (nonatomic,copy) NSString *company;
@property (nonatomic,copy) NSString *email;
@property (nonatomic,assign) BOOL keyAlreadyActivated;
@property (nonatomic,copy) NSString *licenseKeyString;
@property (nonatomic,copy) NSString *signedKeyString;	
@property (nonatomic,assign) BOOL xmlOperationInProgress;	
@property (nonatomic,copy) NSString *progressString;
@property (nonatomic,copy) NSString *errorMessage;
@property (nonatomic,assign) int result;
@property (nonatomic,copy) NSString *licenseType;
@property (nonatomic,assign) BOOL keyIsBoundToCustomer;
@property (nonatomic,retain) LCCustomer *customer;

@end
