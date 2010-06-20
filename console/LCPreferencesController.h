//
//  LCPreferencesController.h
//  Lithium Console
//
//  Created by James Wilson on 3/01/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomerList.h"
#import "LCBackgroundView.h"

#import "LCOutlineView.h"

@interface LCPreferencesController : NSWindowController 
{
	/* UI Elements */
	IBOutlet NSTabView *tabView;
	IBOutlet NSWindow *coreAddSheet;
	IBOutlet NSTextField *addCoreAddressField;
	IBOutlet NSTextField *addCorePortField;
	IBOutlet NSButton *addCoreSSLButton;
	IBOutlet NSButton *addCoreOKButton;
	IBOutlet LCOutlineView *coreOutlineView;
	IBOutlet NSTreeController *coreTreeController;
	IBOutlet LCBackgroundView *backView;
	BOOL addCoreUseSSL;
	
	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarSelectableItems;
	NSMutableArray *toolbarDefaultItems;		
	
	/* Deployments */
	NSMutableArray *deployments;
	LCCoreDeployment *editDeployment;
	
	/* Misc */
	BOOL demoFlag;
}

#define PREF_CUSTLIST @"CustomerList"
#define PREF_CUSTLIST_URLCONF_BOOL @"CustomerList_URLConf_Bool"
#define PREF_CUSTLIST_URLCONF_URL @"CustomerList_URLConf_URL"
#define PREF_CUSTLIST_DISCOVERY_BOOL @"CustomerList_Discovery_Bool"
#define PREF_CUSTLIST_ALERTERROR_BOOL @"CustomerList_Alert_Error_Bool"
#define OLD_PREF_CUSTLIST_STATIC_DEPS @"CustomerList_Static_Deployments"
#define PREF_CUSTLIST_STATIC_DEPS @"CustomerList_Static_Deployment_Object_Data"

#pragma mark "Initialisation"
+ (LCPreferencesController *) preferencesController;
- (LCPreferencesController *) init;
- (void) dealloc;

#pragma mark "Register defaults"
+ (void) registerDefaults;

#pragma mark "Toolbar Methods"
- (void) buildToolbar;

#pragma mark "Toolbar Targets"
- (void) coreClicked:(id)sender;
- (void) customersClicked:(id)sender;
- (void) autoRefreshClicked:(id)sender;
- (void) connectivityClicked:(id)sender;

#pragma mark "UI Actions"
- (IBAction) addCoreClicked:(id)sender;
- (IBAction) removeCoreClicked:(id)sender;
- (IBAction) addCoreAddClicked:(id)sender;
- (IBAction) addCoreCancelClicked:(id)sender;
@property (nonatomic, assign) BOOL addCoreUseSSL;

#pragma mark "Window Delegate Methods"
- (void) windowWillClose:(NSNotification *)notification;

#pragma mark "Accessor Methods"
- (LCCustomerList *) customerList;
- (BOOL) useDiscovery;
- (void) setUseDiscovery:(BOOL)flag;
- (BOOL) demoFlag;

@end
