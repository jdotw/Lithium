//
//  LCBrowserActionsContentController.h
//  Lithium Console
//
//  Created by James Wilson on 13/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2Controller.h"
#import "LCBrowser2ContentViewController.h"
#import "LCCustomer.h"
#import "LCAction.h"
#import "LCActionList.h"

@interface LCBrowserActionsContentController : LCBrowser2ContentViewController 
{
	/* Objects */
	LCCustomer *customer;
	LCBrowser2Controller *browser;
	
	/* List */
	LCActionList *actionList;
	
	/* UI Elements */
	IBOutlet NSArrayController *arrayController;
	
	/* Selection */
	LCAction *selectedAction;
	NSArray *selectedActions;
}

#pragma mark "Constructors"
- (id) initWithCustomer:(LCCustomer *)initCustomer inBrowser:(LCBrowser2Controller *)initBrowser;

#pragma mark "Selection (KVO Observable)"
@property (assign) LCAction *selectedAction;
@property (copy) NSArray *selectedActions;

#pragma mark "UI Actions"
- (IBAction) addNewClicked:(id)sender;
- (IBAction) editSelectedClicked:(id)sender;
- (IBAction) deleteSelectedClicked:(id)sender;
- (IBAction) tableDoubleClicked:(NSArray *)tableSelectedObjects;
- (IBAction) refreshListClicked:(id)sender;

#pragma mark "Properties"
@property (retain) LCActionList *actionList;
@property (assign) LCBrowser2Controller *browser;
@property (retain) LCCustomer *customer;

@end
