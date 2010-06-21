//
//  LCTriggerTuningWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 6/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCTriggersetList.h"
#import "LCTriggersetAppRule.h"
#import "LCTriggersetValRule.h"
#import "LCBackgroundView.h"
#import "LCBrowser2ObjectTableView.h"

@interface LCTriggerTuningWindowController : NSWindowController 
{
	LCEntity *object;
	LCTriggersetList *tsets;
	
	/* UI Objects */
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet LCBackgroundView *backgroundView;
	IBOutlet LCTableView *triggerSetTableView;
	IBOutlet LCTableView *triggerTableView;
	IBOutlet LCTableView *appRulesTableView;
	IBOutlet LCTableView *valRuleTriggerTableView;
	IBOutlet LCTableView *valRuleTableView;
	IBOutlet LCBrowser2ObjectTableView *objectTableView;	
	
	/* ArrayControllers */
	IBOutlet NSArrayController *appRuleArrayController;
	IBOutlet NSArrayController *valRuleArrayController;
	IBOutlet NSArrayController *tsetArrayController; 
	IBOutlet NSArrayController *triggerArrayController;
	
	/* AppRuleSheet */
	IBOutlet NSWindow *appRuleEditSheet;
	IBOutlet NSTextField *appHeaderLabel;
	IBOutlet NSTextField *appTsetLabel;
	IBOutlet NSTextField *appMetLabel;
	IBOutlet NSComboBox *appSiteCombo;
	IBOutlet NSComboBox *appDeviceCombo;
	IBOutlet NSTextField *appContLabel;
	IBOutlet NSComboBox *appObjCombo;
	IBOutlet NSComboBox *appActionCombo;
	IBOutlet NSProgressIndicator *appProgress;
	IBOutlet NSTextField *appStatusLabel;
	IBOutlet NSButton *appSubmitButton;
	IBOutlet NSButton *appCancelButton;
	
	/* AppRule Editing */
	LCTriggersetAppRule *editAppRule;
	BOOL appEditSheetShown;
		
	/* ValRuleSheet */
	IBOutlet NSWindow *valRuleEditSheet;
	IBOutlet NSTextField *valHeaderLabel;
	IBOutlet NSTextField *valTsetLabel;
	IBOutlet NSTextField *valTrgLabel;
	IBOutlet NSComboBox *valSiteCombo;
	IBOutlet NSComboBox *valDeviceCombo;
	IBOutlet NSTextField *valContLabel;
	IBOutlet NSComboBox *valObjectCombo;
	IBOutlet NSTextField *valMetricLabel;
	IBOutlet NSComboBox *valConditionCombo;
	IBOutlet NSTextField *valXText;
	IBOutlet NSTextField *valXUnitsText;
	IBOutlet NSTextField *valYText;
	IBOutlet NSTextField *valYUnitsText;
	IBOutlet NSTextField *valDurationText;
	IBOutlet NSTextField *valEffectLabel;
	IBOutlet NSComboBox *valAdminCombo;
	IBOutlet NSProgressIndicator *valProgress;
	IBOutlet NSTextField *valStatusLabel;
	IBOutlet NSButton *valSubmitButton;
	IBOutlet NSButton *valCancelButton;
	
	/* ValRule Editing */
	LCTriggersetValRule *editValRule;
	BOOL valEditSheetShown;
	
	/* RuleDeleteSheet */
	IBOutlet NSWindow *ruleDeleteSheet;
	IBOutlet NSProgressIndicator *ruleDeleteProgress;
	BOOL deleteSheetShown;
	
	/* Misc */
	NSArray *conditions;
	NSArray *effects;
}

#pragma mark "Initialisation"

- (id) initWithObject:(LCEntity *)initObject;
- (void) dealloc;

#pragma mark "Window Delegate Methods"
- (void) windowWillClose:(NSNotification *)notification;

#pragma mark "Triggerset List Refresh"
- (IBAction) refreshTriggersetListClicked:(id)sender;

#pragma mark "AppRule Editing"
- (IBAction) appRuleTableDoubleClicked:(id)sender;
- (IBAction) addAppRuleClicked:(id)sender;
- (IBAction) deleteAppRuleClicked:(id)sender;
- (IBAction) refreshAppRulesClicked:(id)sender;
- (IBAction) enableSetForObjectClicked:(id)sender;
- (IBAction) disableSetForObjectClicked:(id)sender;
- (IBAction) enableSetForDeviceClicked:(id)sender;
- (IBAction) disableSetForDeviceClicked:(id)sender;
- (void) beginAppRuleEdit:(LCTriggersetAppRule *)rule;
- (IBAction) appRuleSubmitClicked:(id)sender;
- (IBAction) appRuleCancelClicked:(id)sender;

#pragma mark "ValRule Editing"
- (IBAction) valRuleTableDoubleClicked:(id)sender;
- (IBAction) addValueRuleClicked:(id)sender;
- (IBAction) deleteValueRuleClicked:(id)sender;
- (IBAction) refreshValueRulesClicked:(id)sender;
- (void) beginValRuleEdit:(LCTriggersetValRule *)rule;
- (IBAction) valRuleSubmitClicked:(id)sender;
- (void) valRuleOperationFinished:(LCTriggersetValRule *)rule;
- (IBAction) valRuleCancelClicked:(id)sender;
- (IBAction) closeClicked:(id)sender;

#pragma mark "Delete Sheet"
- (void) showDeleteSheet;
- (void) endDeleteSheet;

#pragma mark "Accessors"
@property (readonly) NSArray *conditions;
@property (readonly) NSArray *effects;
@property (nonatomic,retain) LCEntity *object;
@property (nonatomic,retain) LCTriggersetAppRule *editAppRule;
@property (nonatomic,retain) LCTriggersetValRule *editValRule;

@end
