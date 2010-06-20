//
//  LCBrowserScriptsContentController.h
//  Lithium Console
//
//  Created by James Wilson on 10/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2Controller.h"
#import "LCBrowser2ContentViewController.h"
#import "LCCustomer.h"
#import "LCScript.h"
#import "LCScriptList.h"
#import "LCScriptUploadRequest.h"
#import "LCScriptDownloadRequest.h"

@interface LCBrowserScriptsContentController : LCBrowser2ContentViewController 
{
	/* Objects */
	LCCustomer *customer;
	LCBrowser2Controller *browser;
	
	/* Controller Config */
	NSString *type;			/* action / service */

	/* UI Elements */
	IBOutlet NSArrayController *scriptArrayController;
	IBOutlet NSWindow *uploadSheet;
	IBOutlet NSWindow *downloadSheet;
	IBOutlet NSObjectController *commonControllerAlias;
	
	/* Request Objects */
	LCScriptUploadRequest *uploadRequest;
	LCScriptDownloadRequest *downloadRequest;
	
	/* Selection */
	LCScript *selectedScript;
	NSArray *selectedScripts;
}

#pragma mark "Constructors"
- (id) initWithCustomer:(LCCustomer *)initCustomer 
			  inBrowser:(LCBrowser2Controller *)initBrowser 
			   usingNib:(NSString *)nibName
		  forScriptType:(NSString *)initType;

#pragma mark "Selection (KVO Observable)"
@property (assign) LCScript *selectedScript;
@property (copy) NSArray *selectedScripts;

#pragma mark "UI Actions"
- (IBAction) uploadScriptClicked:(id)sender;
- (IBAction) uploadCancelClicked:(id)sender;
- (IBAction) downloadSelectedScriptClicked:(id)sender;
- (IBAction) downloadCancelClicked:(id)sender;
- (IBAction) deleteSelectedScriptClicked:(id)sender;
- (IBAction) refreshScriptListClicked:(id)sender;

#pragma mark "Properties"
@property (assign) LCBrowser2Controller *browser;
@property (retain) LCCustomer *customer;
@property (copy) NSString *type;

@end
