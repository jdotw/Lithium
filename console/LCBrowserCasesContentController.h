//
//  LCBrowserCasesContentController.h
//  Lithium Console
//
//  Created by James Wilson on 27/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2ContentViewController.h"
#import "LCCustomerList.h"
#import "LCBrowser2Controller.h"
#import "LCInspectorController.h"
#import "LCCase.h"

@interface LCBrowserCasesContentController : LCBrowser2ContentViewController 
{
	/* Related Objects */
	LCBrowser2Controller *browser;
	
	/* Selection */
	LCCase *selectedCase;
	NSArray *selectedCases;
	
	/* UI Elements */
	IBOutlet NSArrayController *caseArrayController;	
	IBOutlet LCInspectorController *inspectorController;	
	IBOutlet LCBrowserTableView *entitySummaryTableView;
	IBOutlet LCBrowserTableView *logSummaryTableView;
	IBOutlet NSMenu *openNewCaseMenu;
	IBOutlet NSButton *openNewCaseButton;
}

#pragma mark "Constructor"
- (id) initInBrowser:(LCBrowser2Controller *)initBrowser;

#pragma mark "Selection"
@property (retain) LCCase *selectedCase;
@property (retain) NSArray *selectedCases;

#pragma mark "Case Action Methods"
- (IBAction) viewCaseClicked:(id)sender;
- (IBAction) newCaseClicked:(id)sender;
- (IBAction) closeCaseClicked:(id)sender;
- (IBAction) updateLogClicked:(id)sender;
- (IBAction) addNewCasePopupClicked:(id)sender;
- (IBAction) addNewCaseForCustomerMenuClicked:(NSMenuItem *)sender;

#pragma mark "Properties"
@property (readonly) LCCustomerList *customerList;
@property (assign) LCBrowser2Controller *browser;


@end
