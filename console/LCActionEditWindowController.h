//
//  LCActionEditWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 1/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCAction.h"
#import "LCActionScriptList.h"
#import "LCCustomer.h"
#import "LCEntityArrayController.h"

@interface LCActionEditWindowController : NSWindowController 
{
	/* Action */
	LCAction *action;
	
	/* Related Objects */
	LCCustomer *customer;
	NSWindow *windowForSheet;
	id delegate;
	
	/* UI Elements */
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet NSView *uploadProgressSheet;
	IBOutlet NSView *updateProgressSheet;
	IBOutlet LCEntityArrayController *entityArrayController;
	IBOutlet NSButton *saveButton;
	IBOutlet LCBackgroundView *backgroundView;
	IBOutlet NSButton *nextButton;
	IBOutlet NSButton *backButton;
	IBOutlet NSTabView *contentTabView;
	IBOutlet NSTabView *stepTabView;
	IBOutlet NSTabView *assistantTabView;
	IBOutlet NSTextField *listDownloadStatus;
	IBOutlet NSButton *downloadScriptButton;
	IBOutlet LCEntityArrayController *emailEntityArrayController;
	
	/* Indexes */
	int contentIndex;
	int assistantIndex;
	int stepIndex;
}

#pragma mark "Constructors"
+ (LCActionEditWindowController *) beginSheetForNewAction:(LCCustomer *)initCustomer
										   windowForSheet:(NSWindow *)initWindow
												 delegate:(id)initDelegate;
+ (LCActionEditWindowController *) beginSheetForNewEmailAction:(LCCustomer *)initCustomer
												windowForSheet:(NSWindow *)initWindow
													  delegate:(id)initDelegate;
+ (LCActionEditWindowController *) beginSheetForAction:(LCAction *)initAction
										windowForSheet:(NSWindow *)initWindow
											  delegate:(id)initDelegate;
+ (LCActionEditWindowController *) beginSheetToDeleteAction:(LCAction *)initAction
											 windowForSheet:(NSWindow *)initWindow
												   delegate:(id)initDelegate;
- (id) initForNewAction:(LCCustomer *)initCustomer
		 windowForSheet:(NSWindow *)initWindow
			   delegate:(id)initDelegate;
- (id) initForNewEmailAction:(LCCustomer *)initCustomer
			  windowForSheet:(NSWindow *)initWindow
					delegate:(id)initDelegate;
- (id) initForAction:(LCAction *)initAction
	  windowForSheet:(NSWindow *)initWindow
			delegate:(id)initDelegate;
- (id) initWithActionToDelete:(LCAction *)initAction
			   windowForSheet:(NSWindow *)initWindow
					 delegate:(id)initDelegate;
- (id) initWithCustomer:(LCCustomer *)initCustomer
		 windowForSheet:(NSWindow *)initWindow
			   delegate:(id)initDelegate;

#pragma mark "UI Actions"
- (IBAction) saveClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;
- (IBAction) deleteSelectedEntityClicked:(id)sender;

#pragma mark "Assistant Methods"
- (void) scriptListRefreshFinished:(id)sender;
- (IBAction) nextClicked:(id)sender;
- (IBAction) backClicked:(id)sender;
- (void) showStep0;
- (void) showStep1;
- (void) showStep2;
@property (assign) int contentIndex;
@property (assign) int assistantIndex;
@property (assign) int stepIndex;

#pragma mark "Accessors"
@property (retain) LCAction *action;
@property (retain) LCCustomer *customer;
@property (retain) NSWindow *windowForSheet;
@property (assign) id delegate;

@end
