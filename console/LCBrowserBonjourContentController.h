//
//  LCBrowserBonjourContentController.h
//  Lithium Console
//
//  Created by James Wilson on 9/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2ContentViewController.h"
#import "LCBonjourBrowserList.h"
#import "LCBonjourService.h"
#import "LCBrowser2Controller.h"

@interface LCBrowserBonjourContentController : LCBrowser2ContentViewController 
{
	/* Objects */
	LCBrowser2Controller *browser;
	NSMutableArray *browserLists;
	
	/* Selection */
	LCCustomer *selectedCustomer;
	LCSite *selectedSite;
	LCBonjourService *selectedService;
	NSArray *selectedServices;
	
	/* UI Elements */
	IBOutlet NSTreeController *serviceTreeController;
	IBOutlet LCTransparentOutlineView *outlineView;
}

#pragma mark "Constructor"
- (id) initInBrowser:(LCBrowser2Controller *)initBrowser;

#pragma mark "Selection"
@property (retain) LCCustomer *selectedCustomer;
@property (retain) LCSite *selectedSite;
@property (retain) LCBonjourService *selectedService;
@property (retain) NSArray *selectedServices;

#pragma mark "Properties"
@property (assign) LCBrowser2Controller *browser;

#pragma mark "UI Actions"
- (IBAction) monitorDevicesClicked:(id)sender;


@end
