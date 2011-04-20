//
//  LCAdminController.h
//  LCAdminTools
//
//  Created by James Wilson on 3/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCCrashReporter.h"
#import "LCUpdateController.h"
#import "LCCustomerController.h"
#import "LCLicenseController.h"
#import "LCModulesController.h"

@interface LCAdminController : NSObject 
{
	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarSelectableItems;
	NSMutableArray *toolbarDefaultItems;	
	
	/* UI Elements */
	IBOutlet NSWindow *window;
	IBOutlet NSTabView *tabView;
	IBOutlet NSWindow *setupWizard;
	IBOutlet LCCrashReporter *crashReporter;
	IBOutlet LCUpdateController *updateController;
	IBOutlet LCCustomerController *customerController;
	IBOutlet LCLicenseController *licenseController;
	IBOutlet LCModulesController *modulesController;
}

- (void) buildToolbar;
- (void) statusClicked:(id)sender;
- (void) performanceClicked:(id)sender;
- (void) configureClicked:(id)sender;
- (void) updateClicked:(id)sender;
- (void) backupClicked:(id)sender;
- (void) restoreClicked:(id)sender;
- (void) diagnosticClicked:(id)sender;


@end
