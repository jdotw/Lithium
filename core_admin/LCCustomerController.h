//
//  LCCustomerController.h
//  LCAdminTools
//
//  Created by James Wilson on 22/09/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCConfigController.h"
#import "LCStatusController.h"
#import "LCCustomer.h"

@interface LCCustomerController : NSObject
{
	/* UI Elements */
	IBOutlet NSWindow *window;
	IBOutlet NSArrayController *customerArrayController;
	IBOutlet LCConfigController *configController;
	IBOutlet LCStatusController *statusController;
	
	/* Customers */
	NSMutableArray *customers;
	NSMutableDictionary *customerDict;
	NSArray *customerSortDescriptors;
	
	/* Customer Add */
	IBOutlet NSWindow *addCustomerSheet;
	NSString *addCustomerDesc;
	NSString *addCustomerName;
	BOOL addCustomerSetNameAutomatically;
	NSString *addCustomerURL;
	NSString *addCustomerMailServer;
	NSString *addCustomerMailFrom;
	NSString *addCustomerMailTo;
	BOOL addCustomerEnablePush;
	
	/* Customer Edit */
	LCCustomer *editCustomer;
	IBOutlet NSWindow *editCustomerSheet;
	IBOutlet NSTextField *editCustomerDescField;
}

#pragma mark "UI Actions"
- (IBAction) addNewCustomerClicked:(id)sender;
- (IBAction) deleteSelectedClicked:(id)sender;

#pragma mark "Add New Customer"
- (IBAction) addCustomerAddClicked:(id)sender;
- (IBAction) addCustomerCancelClicked:(id)sender;

#pragma mark "Edit Customer"
- (IBAction) editSelectedClicked:(id)sender;
- (IBAction) editCustomerCancelClicked:(id)sender;
- (IBAction) editCustomerSaveClicked:(id)sender;

#pragma mark "Delete Selected"
- (void) deleteAlertDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo;

#pragma mark "Customer List"
@property (nonatomic,readonly) NSMutableArray *customers;
@property (nonatomic,readonly) NSMutableDictionary *customerDict;
- (void) insertObject:(id)obj inCustomersAtIndex:(unsigned int)index;
- (void) removeObjectFromCustomersAtIndex:(unsigned int)index;
- (void) refreshCustomerList;

#pragma mark "Properties"
@property (nonatomic,copy) NSString *addCustomerDesc;
@property (nonatomic,copy) NSString *addCustomerName;
@property (nonatomic,copy) NSString *addCustomerURL;
@property (nonatomic,assign) BOOL addCustomerSetNameAutomatically;
@property (nonatomic,copy) NSString *addCustomerMailServer;
@property (nonatomic,copy) NSString *addCustomerMailFrom;
@property (nonatomic,copy) NSString *addCustomerMailTo;
@property (nonatomic,assign) BOOL addCustomerEnablePush;
@property (nonatomic,copy) NSArray *customerSortDescriptors;

@end
