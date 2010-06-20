//
//  LCSetupController.h
//  LCAdminTools
//
//  Created by James Wilson on 31/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCConfigController.h"
#import "LCStatusController.h"

@interface LCSetupController : NSObject 
{
	IBOutlet NSTabView *tabView;
	IBOutlet NSWindow *sheet;
	IBOutlet NSWindow *window;
	IBOutlet NSButton *backButton;
	IBOutlet NSButton *nextButton;
	IBOutlet NSButton *cancelButton;
	IBOutlet LCConfigController *configController;
	IBOutlet LCStatusController *statusController;
	
	IBOutlet NSResponder *step1FR;
	IBOutlet NSResponder *step2FR;
	IBOutlet NSResponder *step3FR;
	IBOutlet NSResponder *step4FR;
	
	BOOL setupInProgress;
	NSString *status;
	NSImage *statusIcon;
	
	NSString *configStatus;
	NSString *dbStatus;
	BOOL dbUseDefault;
	
	NSString *customerName;
	NSString *customerDesc;
	BOOL editName;
	
	NSString *emailFrom;
	NSString *emailServer;
	NSString *emailTo;
	BOOL pushEnabled;
	
	NSAlert *alert;
}

#pragma mark "Perform Setup"
- (void) performSetup;

#pragma mark "Validation"
- (BOOL) validateInputForStage:(int)index;

#pragma mark "UI Actions"
- (IBAction) runSetupWizard:(id)sender;
- (IBAction) launchConsole:(id)sender;
- (IBAction) nextClicked:(id)sender;
- (IBAction) backClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;
- (BOOL) setupInProgress;
- (void) setSetupInProgress:(BOOL)flag;
- (NSString *) customerName;
- (void) setCustomerName:(NSString *)string;
- (NSString *) customerDesc;
- (void) setCustomerDesc:(NSString *)string;
- (BOOL) editName;
- (void) setEditName:(BOOL)flag;
- (NSString *) status;
- (void) setStatus:(NSString *)string;
- (NSImage *) statusIcon;
- (void) setStatusIcon:(NSImage *)image;
- (NSString *) configStatus;
- (void) setConfigStatus:(NSString *)string;
- (NSString *) dbStatus;
- (void) setDbStatus:(NSString *)string;
- (BOOL) dbUseDefault;
- (void) setDbUseDefault:(BOOL)flag;
@property (copy) NSString *emailFrom;
@property (copy) NSString *emailServer;
@property (copy) NSString *emailTo;
@property (assign) BOOL pushEnabled;


@end
