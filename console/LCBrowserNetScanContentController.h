//
//  LCBrowserNetScanContentController.h
//  Lithium Console
//
//  Created by James Wilson on 10/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2Controller.h"
#import "LCBrowser2ContentViewController.h"
#import "LCIPRegistryNetworkList.h"
#import "LCIPRegistryNetwork.h"
#import "LCIPRegistryEntry.h"

@interface LCBrowserNetScanContentController : LCBrowser2ContentViewController 
{
	/* Objects */
	LCBrowser2Controller *browser;
	NSMutableArray *networkLists;
	
	/* Selection */
	LCCustomer *selectedCustomer;
	LCSite *selectedSite;
	LCIPRegistryNetworkList *selectedList;
	LCIPRegistryEntry *selectedEntry;
	NSArray *selectedEntries;
	
	/* UI Elements */
	IBOutlet NSTreeController *entryTreeController;
	IBOutlet NSWindow *scanNetworkSheet;
	IBOutlet NSTextField *scanNetwork;
	IBOutlet NSTextField *scanMask;
	IBOutlet LCTransparentOutlineView *outlineView;
}

#pragma mark "Constructor"
- (id) initInBrowser:(LCBrowser2Controller *)initBrowser;

#pragma mark "Selection"
@property (nonatomic,retain) LCCustomer *selectedCustomer;
@property (nonatomic,retain) LCSite *selectedSite;
@property (nonatomic,retain) LCIPRegistryNetworkList *selectedList;
@property (nonatomic,retain) LCIPRegistryEntry *selectedEntry;
@property (nonatomic,retain) NSArray *selectedEntries;

#pragma mark "Properties"
@property (nonatomic, assign) LCBrowser2Controller *browser;

#pragma mark "UI Actions"
- (IBAction) monitorDevicesClicked:(id)sender;
- (IBAction) scanNetworkClicked:(id)sender;
- (IBAction) scanNetworkCancelClicked:(id)sender;
- (IBAction) scanNetworkScanClicked:(id)sender;


@end
