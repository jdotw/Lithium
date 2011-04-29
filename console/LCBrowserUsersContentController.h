//
//  LCBrowserUsersContentController.h
//  Lithium Console
//
//  Created by James Wilson on 5/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2Controller.h"
#import "LCCustomer.h"
#import "LCUser.h"

@interface LCBrowserUsersContentController : LCBrowser2ContentViewController 
{
	/* Objects */
	LCCustomer *customer;
	LCBrowser2Controller *browser;
	
	/* Add/Edit/Delete */
	LCUser *editUser;
	BOOL editUserIsNew;
	NSString *editSheetStatus;
	BOOL xmlOperationInProgress;
	
	/* UI Elements */
	IBOutlet NSArrayController *userArrayController;
	IBOutlet NSWindow *editSheet;
	IBOutlet NSTextField *editSheetLabel;
	IBOutlet NSButton *editSheetOKButton;
	IBOutlet NSTextField *editSheetUsername;
	IBOutlet NSTextField *editSheetPassword;
	IBOutlet NSTextField *editSheetConfirmPassword;
	IBOutlet NSTextField *editSheetFullname;
	
	/* Selection */
	LCUser *selectedUser;
	NSArray *selectedUsers;
}

#pragma mark "Constructors"
- (id) initWithCustomer:(LCCustomer *)initCustomer inBrowser:(LCBrowser2Controller *)initBrowser;

#pragma mark "Selection (KVO Observable)"
@property (nonatomic, assign) LCUser *selectedUser;
@property (nonatomic,copy) NSArray *selectedUsers;

#pragma mark "UI Actions"
- (IBAction) addNewUserClicked:(id)sender;
- (IBAction) editSelectedUserClicked:(id)sender;
- (IBAction) editSheetOKClicked:(id)sender;
- (IBAction) editSheetCancelClicked:(id)sender;
- (IBAction) deleteSelectedUserClicked:(id)sender;
- (IBAction) userTableDoubleClicked:(NSArray *)tableSelectedObjects;
- (IBAction) refreshUserListClicked:(id)sender;

#pragma mark "Properties"
@property (nonatomic, assign) LCBrowser2Controller *browser;
@property (nonatomic,retain) LCCustomer *customer;
@property (nonatomic,retain) LCUser *editUser;
@property (nonatomic, assign) BOOL editUserIsNew;
@property (nonatomic,copy) NSString *editSheetStatus;
@property (nonatomic, assign) BOOL xmlOperationInProgress;

@end
