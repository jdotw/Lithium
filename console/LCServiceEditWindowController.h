//
//  LCServiceEditWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 7/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCService.h"
#import "LCDevice.h"
#import "LCEntityArrayController.h"
#import "LCBrowserTableView.h"

@interface LCServiceEditWindowController : NSWindowController 
{
	/* Service */
	LCService *service;
	
	/* Related Objects */
	LCDevice *device;
	NSWindow *windowForSheet;
	id delegate;
	
	/* UI Elements */
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet NSButton *saveButton;
	IBOutlet LCTableView *configTableView;
	IBOutlet NSButton *backButton;
	IBOutlet NSButton *nextButton;
	IBOutlet NSTabView *assistantTabView;
	IBOutlet NSTabView *stepTabView;
	IBOutlet NSTabView *contentTabView;
	
	/* Indexes */
	int contentIndex;
	int stepIndex;
	NSString *assistantIdentifier;
}

#pragma mark "Constructors"
+ (LCServiceEditWindowController *) beginSheetForNewService:(LCDevice *)initDevice
											 windowForSheet:(NSWindow *)initWindow
												   delegate:(id)initDelegate;
+ (LCServiceEditWindowController *) beginSheetForNewHTTPService:(LCDevice *)initDevice
												 windowForSheet:(NSWindow *)initWindow
													   delegate:(id)initDelegate;
+ (LCServiceEditWindowController *) beginSheetForNewSMTPService:(LCDevice *)initDevice
												 windowForSheet:(NSWindow *)initWindow
													   delegate:(id)initDelegate;
+ (LCServiceEditWindowController *) beginSheetForNewPOPService:(LCDevice *)initDevice
												windowForSheet:(NSWindow *)initWindow
													  delegate:(id)initDelegate;
+ (LCServiceEditWindowController *) beginSheetForNewIMAPService:(LCDevice *)initDevice
												 windowForSheet:(NSWindow *)initWindow
													   delegate:(id)initDelegate;
+ (LCServiceEditWindowController *) beginSheetForNewDNSService:(LCDevice *)initDevice
												windowForSheet:(NSWindow *)initWindow
													  delegate:(id)initDelegate;
+ (LCServiceEditWindowController *) beginSheetForServiceToEdit:(LCObject *)initObject
												windowForSheet:(NSWindow *)initWindow
													  delegate:(id)initDelegate;
+ (LCServiceEditWindowController *) beginSheetForServiceToDelete:(LCObject *)initObject
												  windowForSheet:(NSWindow *)initindow;
- (id) initForNewServiceUsingScript:(NSString *)scriptName
						   atDevice:(LCDevice *)initDevice 
					 windowForSheet:(NSWindow *)initWindow
						   delegate:(id)initDelegate;
- (id) initWithServiceObject:(LCObject *)initServiceObject
			  windowForSheet:(NSWindow *)initWindow
					delegate:(id)initDelegate;
- (id) initWithDevice:(LCDevice *)initDevice
	   windowForSheet:(NSWindow *)initWindow
			 delegate:(id)initDelegate;
- (id) initWithServiceToDelete:(LCObject *)initServiceObject 
				windowForSheet:(NSWindow *)initWindow;

#pragma mark "UI Services"
- (IBAction) saveClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;

#pragma mark "Assistant Methods"
- (void) scriptListRefreshFinished:(id)sender;
- (IBAction) nextClicked:(id)sender;
- (IBAction) backClicked:(id)sender;
- (void) showStep0;
- (void) showStep1;
- (void) showStep2;

#pragma mark "Properties"
@property (nonatomic,retain) LCService *service;
@property (nonatomic,retain) LCDevice *device;
@property (nonatomic,retain) NSWindow *windowForSheet;
@property (nonatomic, assign) id delegate;
@property (nonatomic, assign) int contentIndex;
@property (nonatomic, assign) int stepIndex;
@property (nonatomic,copy) NSString *assistantIdentifier;

@end

