//
//  LCLithiumSetupWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 11/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"
#import "LCBackgroundView.h"

@interface LCLithiumSetupWindowController : NSWindowController 
{
	
	LCCustomer *customer;
	
	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarSelectableItems;
	NSMutableArray *toolbarDefaultItems;			
	
	/* UI Elements */
	IBOutlet LCBackgroundView *backView;
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet NSObjectController *coreLicenseControllerAlias;
	IBOutlet NSObjectController *actionControllerAlias;
	IBOutlet NSObjectController *serviceControllerAlias;
	IBOutlet NSObjectController *userControlerAlias;
	IBOutlet NSObjectController *coreInfoControlerAlias;
	IBOutlet NSTabView *tabView;
	IBOutlet id coreInfoController;
	
	/* Refresh Timers */
	NSTimer *coreInfoRefreshTimer;
}

#pragma mark "Constructors"
+ (LCLithiumSetupWindowController *) setupControllerForCustomer:(LCCustomer *)initCustomer;
+ (LCLithiumSetupWindowController *) actionScriptSetupControllerForCustomer:(LCCustomer *)initCustomer;
+ (LCLithiumSetupWindowController *) serviceScriptSetupControllerForCustomer:(LCCustomer *)initCustomer;
+ (LCLithiumSetupWindowController *) licenseSetupControllerForCustomer:(LCCustomer *)initCustomer;
- (LCLithiumSetupWindowController *) initForCustomer:(LCCustomer *)initCustomer showingTab:(NSString *)tabIdentifier;
- (void) dealloc;

#pragma mark "Window Delegate Methods"
- (void) windowWillClose:(NSNotification *)notification;

#pragma mark "Toolbar Methods"
- (void) buildToolbar;

#pragma mark "Accessors"
- (LCCustomer *) customer;
- (void) setCustomer:(LCCustomer *)newCustomer;
- (NSTabView *) tabView;
- (NSToolbar *) toolbar;

@property (retain,getter=customer,setter=setCustomer:) LCCustomer *customer;
@property (retain,getter=toolbar) NSToolbar *toolbar;
@property (retain) NSMutableDictionary *toolbarItems;
@property (retain) NSMutableArray *toolbarSelectableItems;
@property (retain) NSMutableArray *toolbarDefaultItems;
@property (retain) LCBackgroundView *backView;
@property (retain) NSObjectController *controllerAlias;
@property (retain) NSObjectController *coreLicenseControllerAlias;
@property (retain) NSObjectController *actionControllerAlias;
@property (retain) NSObjectController *serviceControllerAlias;
@property (retain) NSObjectController *userControlerAlias;
@property (retain) NSObjectController *coreInfoControlerAlias;
@property (retain,getter=tabView) NSTabView *tabView;
@property (retain) id coreInfoController;
@property (retain) NSTimer *coreInfoRefreshTimer;
@end
