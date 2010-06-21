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

@property (nonatomic,retain,getter=customer,setter=setCustomer:) LCCustomer *customer;
@property (nonatomic,retain,getter=toolbar) NSToolbar *toolbar;
@property (nonatomic,retain) NSMutableDictionary *toolbarItems;
@property (nonatomic,retain) NSMutableArray *toolbarSelectableItems;
@property (nonatomic,retain) NSMutableArray *toolbarDefaultItems;
@property (nonatomic,retain) LCBackgroundView *backView;
@property (nonatomic,retain) NSObjectController *controllerAlias;
@property (nonatomic,retain) NSObjectController *coreLicenseControllerAlias;
@property (nonatomic,retain) NSObjectController *actionControllerAlias;
@property (nonatomic,retain) NSObjectController *serviceControllerAlias;
@property (nonatomic,retain) NSObjectController *userControlerAlias;
@property (nonatomic,retain) NSObjectController *coreInfoControlerAlias;
@property (nonatomic,retain,getter=tabView) NSTabView *tabView;
@property (nonatomic,retain) id coreInfoController;
@property (nonatomic,retain) NSTimer *coreInfoRefreshTimer;
@end
