//
//  LCUserManagerController.h
//  Lithium Console
//
//  Created by James Wilson on 21/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCLithiumSetupWindowController.h"
#import "LCUser.h"

@interface LCUserManagerController : NSObject 
{
	/* Setup Controller */
	IBOutlet LCLithiumSetupWindowController *setupController;
	
	/* UI Elements */
	IBOutlet NSWindow *addUserSheet;
	IBOutlet NSWindow *removeUserSheet;
	IBOutlet NSWindow *editGlobalAdminSheet; 
	IBOutlet NSArrayController *userArrayController;
	IBOutlet NSButton *saveButton;
	IBOutlet NSTextField *usernameField;
	IBOutlet NSTextField *passwordField;
	
	/* Add Use */
	LCUser *editUser;
	
	/* XML Ops */
	LCXMLRequest *updateXmlReq;
	LCXMLRequest *removeXmlReq;
	BOOL xmlOperationInProgress;
	NSString *xmlElement;
	NSMutableString *xmlString;	
	NSString *resultString;
	NSImage *resultIcon;
}

#pragma mark "Add/Edit User"
- (IBAction) addUserClicked:(id)sender;
- (IBAction) addUserSheetCancelClicked:(id)sender;
- (IBAction) editUserClicked:(id)sender;
- (IBAction) editGlobalAdminCancelClicked:(id)sender;
- (IBAction) saveUserSheetAddClicked:(id)sender;

#pragma mark "Remove User"
- (IBAction) removeUserClicked:(id)sender;
- (IBAction) removeUserSheetRemoveClicked:(id)sender;
- (IBAction) removeUserSheetCancelClicked:(id)sender;

#pragma mark "Finished"
- (LCUser *) editUser;
- (void) setEditUser:(LCUser *)newUser;
- (NSString *) resultString;
- (void) setResultString:(NSString *)string;
- (NSImage *) resultIcon;
- (void) setResultIcon:(NSImage *)image;
- (BOOL) xmlOperationInProgress;
- (void) setXmlOperationInProgress:(BOOL)value;


@property (nonatomic,retain) LCLithiumSetupWindowController *setupController;
@property (nonatomic,retain) NSWindow *addUserSheet;
@property (nonatomic,retain) NSWindow *removeUserSheet;
@property (nonatomic,retain) NSWindow *editGlobalAdminSheet;
@property (nonatomic,retain) NSArrayController *userArrayController;
@property (nonatomic,retain) NSButton *saveButton;
@property (nonatomic,retain) NSTextField *usernameField;
@property (nonatomic,retain) NSTextField *passwordField;
@property (retain,getter=editUser,setter=setEditUser:) LCUser *editUser;
@property (nonatomic,retain) LCXMLRequest *updateXmlReq;
@property (nonatomic,retain) LCXMLRequest *removeXmlReq;
@property (getter=xmlOperationInProgress,setter=setXmlOperationInProgress:) BOOL xmlOperationInProgress;
@property (nonatomic,retain) NSString *xmlElement;
@property (nonatomic,retain) NSMutableString *xmlString;
@property (retain,getter=resultString,setter=setResultString:) NSString *resultString;
@property (retain,getter=resultIcon,setter=setResultIcon:) NSImage *resultIcon;
@end
