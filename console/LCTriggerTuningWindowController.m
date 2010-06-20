//
//  LCTriggerTuningWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 6/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCTriggerTuningWindowController.h"


@implementation LCTriggerTuningWindowController

#pragma mark "Initialisation"

- (id) initWithObject:(LCEntity *)initObject
{
	/* Set object */
	self.object = initObject;
	
	/* Create tset list */
	tsets = [[LCTriggersetList listWithObject:object] retain];
	tsets.delegate = self;
	[tsets highPriorityRefresh];

	/* Create effects */
	effects = [[NSMutableArray arrayWithObjects:@"Warning", @"Impaired", @"Critical", nil] retain];
	conditions = [[NSMutableArray arrayWithObjects:@"Equals", @"Less Than X", @"Greater Than X", @"Not Equal To X", @"Between X and Y", nil] retain];
	
	/* Super-class init */
	[self initWithWindowNibName:@"TriggerTuningWindow"];
	[self window];
	
	return self;
}

- (void) dealloc
{
	[tsets release];
	[effects release];
	[conditions release];
	[object release];
	[super dealloc];
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Autorelease */
	[controllerAlias setContent:nil];
	[self autorelease];
}

#pragma mark "Triggerset List Refresh"

- (IBAction) refreshTriggersetListClicked:(id)sender
{
	[tsets highPriorityRefresh];
}

#pragma mark "AppRule Editing"

- (IBAction) appRuleTableDoubleClicked:(id)sender
{
	if ([[appRuleArrayController selectedObjects] count] < 1) return;
	LCTriggersetAppRule *selectedRule;	
	selectedRule = [[appRuleArrayController selectedObjects] objectAtIndex:0];
	[self beginAppRuleEdit:selectedRule];
}

- (IBAction) addAppRuleClicked:(id)sender
{
	LCTriggerset *tset;
	if ([[tsetArrayController selectedObjects] count] < 1) return;
	tset = [[tsetArrayController selectedObjects] objectAtIndex:0];
	LCTriggersetAppRule *appRule = [[LCTriggersetAppRule alloc] initWithObject:object 
															triggerset:tset 
														  criteriaSite:[object site] 
														criteriaDevice:[object device] 
														criteriaObject:object 
													 criteriaApplyFlag:NO];
	[self beginAppRuleEdit:appRule];
	[appRule autorelease];
}

- (IBAction) deleteAppRuleClicked:(id)sender
{
	/* Get rule */
	LCTriggersetAppRule *rule;
	if ([[appRuleArrayController selectedObjects] count] < 1) return;
	rule = [[appRuleArrayController selectedObjects] objectAtIndex:0];
	
	/* Show delete sheet */
	[self showDeleteSheet];
	
	/* Delete rule */
	[rule setDelegate:self];
	[rule delete];
}

- (IBAction) refreshAppRulesClicked:(id)sender
{
	LCTriggerset *tset;
	if ([[tsetArrayController selectedObjects] count] < 1) return;
	tset = [[tsetArrayController selectedObjects] objectAtIndex:0];
	[[tset appRules] highPriorityRefresh];
}

- (IBAction) enableSetForObjectClicked:(id)sender
{
	LCTriggerset *tset;
	if ([[tsetArrayController selectedObjects] count] < 1) return;
	tset = [[tsetArrayController selectedObjects] objectAtIndex:0];
	LCTriggersetAppRule *rule;
	rule = [[LCTriggersetAppRule alloc] initWithObject:object
											triggerset:tset
										  criteriaSite:[object site]
										criteriaDevice:[object device]
										criteriaObject:object
									 criteriaApplyFlag:YES];
	[self beginAppRuleEdit:rule];
	[rule autorelease];
}

- (IBAction) disableSetForObjectClicked:(id)sender
{
	LCTriggerset *tset;
	if ([[tsetArrayController selectedObjects] count] < 1) return;
	tset = [[tsetArrayController selectedObjects] objectAtIndex:0];
	LCTriggersetAppRule *rule;
	rule = [[LCTriggersetAppRule alloc] initWithObject:object
											triggerset:tset
										  criteriaSite:[object site]
										criteriaDevice:[object device]
										criteriaObject:object
									 criteriaApplyFlag:NO];
	[self beginAppRuleEdit:rule];	
	[rule autorelease];
}

- (IBAction) enableSetForDeviceClicked:(id)sender
{
	LCTriggerset *tset;
	if ([[tsetArrayController selectedObjects] count] < 1) return;
	tset = [[tsetArrayController selectedObjects] objectAtIndex:0];
	LCTriggersetAppRule *rule;
	rule = [[LCTriggersetAppRule alloc] initWithObject:object
											triggerset:tset
										  criteriaSite:[object site]
										criteriaDevice:[object device]
										criteriaObject:nil
									 criteriaApplyFlag:YES];
	[self beginAppRuleEdit:rule];
	[rule autorelease];	
}

- (IBAction) disableSetForDeviceClicked:(id)sender
{
	LCTriggerset *tset;
	if ([[tsetArrayController selectedObjects] count] < 1) return;
	tset = [[tsetArrayController selectedObjects] objectAtIndex:0];
	LCTriggersetAppRule *rule;
	rule = [[LCTriggersetAppRule alloc] initWithObject:object
											triggerset:tset
										  criteriaSite:[object site]
										criteriaDevice:[object device]
										criteriaObject:nil
									 criteriaApplyFlag:NO];
	[self beginAppRuleEdit:rule];	
	[rule autorelease];
}

- (void) beginAppRuleEdit:(LCTriggersetAppRule *)rule
{
	/* Set edit rule */
	[self setEditAppRule:rule];
	
	/* Set header */
	if ([rule ID])
	{ 
		[appHeaderLabel setStringValue:@"Edit Existing Trigger Set Application Rule"]; 
		[appSubmitButton setTitle:@"Submit"];
	}
	else
	{ 
		[appHeaderLabel setStringValue:@"Add New Trigger Set Application Rule"]; 
		[appSubmitButton setTitle:@"Add"];
	}
	
	/* Reset status */
	[appStatusLabel setStringValue:@""];
	[appProgress setHidden:YES];
	[appSubmitButton setEnabled:YES];
	[appCancelButton setEnabled:YES];
	
	/* Set labels */
	if ([[rule triggerset] desc])
	{ [appTsetLabel setStringValue:[[rule triggerset] desc]]; }
	else
	{ [appTsetLabel setStringValue:@""]; }
	if ([[rule triggerset] metricDesc])
	{ [appMetLabel setStringValue:[[rule triggerset] metricDesc]]; } 
	else [appMetLabel setStringValue:@""];
	if ([[[rule object] container] desc])
	{ [appContLabel setStringValue:[[[rule object] container] desc]]; }
	else
	{ [appContLabel setStringValue:@""]; }
	
	/* Site criteria */
	[appSiteCombo removeAllItems];
	[appSiteCombo addItemWithObjectValue:[[[rule object] site] desc]];
	[appSiteCombo addItemWithObjectValue:@"*ALL*"];
	if([rule siteName])
	{ [appSiteCombo selectItemAtIndex:0]; }
	else
	{ [appSiteCombo selectItemAtIndex:1]; }

	/* Device criteria */
	[appDeviceCombo removeAllItems];
	[appDeviceCombo addItemWithObjectValue:[[[rule object] device] desc]];
	[appDeviceCombo addItemWithObjectValue:@"*ALL*"];
	if([rule deviceName])
	{ [appDeviceCombo selectItemAtIndex:0]; }
	else
	{ [appDeviceCombo selectItemAtIndex:1]; }
	
	/* Object criteria */
	[appObjCombo removeAllItems];
	[appObjCombo addItemWithObjectValue:[[rule object] desc]];
	[appObjCombo addItemWithObjectValue:@"*ALL*"];
	if([rule objectName])
	{ [appObjCombo selectItemAtIndex:0]; }
	else
	{ [appObjCombo selectItemAtIndex:1]; }
	
	/* Action criteria */
	if ([rule apply])
	{ [appActionCombo selectItemAtIndex:1]; }
	else
	{ [appActionCombo selectItemAtIndex:0]; }
	
	/* Open sheet */
	[appRuleEditSheet setBackgroundColor:[NSColor colorWithCalibratedWhite:0.8 alpha:1.0]];
	[NSApp beginSheet:appRuleEditSheet
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
	appEditSheetShown = YES;
}

- (IBAction) appRuleSubmitClicked:(id)sender
{
	/* Set delegate */
	[editAppRule setDelegate:self];
	
	/* Update the site */
	if ([appSiteCombo indexOfSelectedItem] == 0)
	{ 
		[editAppRule setSiteName:[[[editAppRule object] site] name]];
		[editAppRule setSiteDesc:[[[editAppRule object] site] desc]];
	}
	else
	{
		[editAppRule setSiteName:nil];
		[editAppRule setSiteDesc:nil];
	}

	/* Update the device */
	if ([appDeviceCombo indexOfSelectedItem] == 0)
	{ 
		[editAppRule setDeviceName:[[[editAppRule object] device] name]];
		[editAppRule setDeviceDesc:[[[editAppRule object] device] desc]];
	}
	else
	{
		[editAppRule setDeviceName:nil];
		[editAppRule setDeviceDesc:nil];
	}
	
	/* Update the object */
	if ([appObjCombo indexOfSelectedItem] == 0)
	{ 
		[editAppRule setObjectName:[[editAppRule object] name]];
		[editAppRule setObjectDesc:[[editAppRule object] desc]];
	}
	else
	{
		[editAppRule setObjectName:nil];
		[editAppRule setObjectDesc:nil];
	}
		
	/* Update the action */
	if ([appActionCombo indexOfSelectedItem] == 0)
	{ [editAppRule setApply:NO]; }
	else
	{ [editAppRule setApply:YES]; }		
	
	/* Adjust sheet */
	[appSubmitButton setEnabled:NO];
	[appCancelButton setEnabled:NO];
	[appProgress setHidden:NO];
	[appProgress startAnimation:self];
	[appStatusLabel setStringValue:@"Submitting rule..."];
	
	/* Update rule */
	[editAppRule update];
}

- (void) appRuleOperationFinished:(LCTriggersetAppRule *)rule
{
	/* Refresh */
	[tsets highPriorityRefresh];
	[[[editAppRule triggerset] appRules] highPriorityRefresh];

	/* Close sheets */
	if (appEditSheetShown)
	{
		/* Adjust sheet */
		[appStatusLabel setStringValue:@"Completed"];

		/* Close sheet */
		[NSApp endSheet:appRuleEditSheet];
		[appRuleEditSheet close];
		appEditSheetShown = NO;

		/* Release rule */
		[self setEditAppRule:nil];
	}
	if (deleteSheetShown)
	{
		/* Remove from list */
		[[[rule triggerset] appRules] performSelector:@selector(removeRule:) withObject:rule afterDelay:0.0];
//		[[[rule triggerset] appRules] removeObjectFromRulesAtIndex:[[[[rule triggerset] appRules] rules] indexOfObject:rule]];

		/* Close sheet */
		[self endDeleteSheet]; 
		deleteSheetShown = NO;
	}
}

- (IBAction) appRuleCancelClicked:(id)sender
{
	/* Release rule */
	[self setEditAppRule:nil];

	/* Close sheet */
	[NSApp endSheet:appRuleEditSheet];
	[appRuleEditSheet close];
	appEditSheetShown = NO;
}

#pragma mark "ValRule Editing"

- (IBAction) valRuleTableDoubleClicked:(id)sender
{
	if ([[valRuleArrayController selectedObjects] count] < 1) return;
	LCTriggersetValRule *selectedRule;	
	selectedRule = [[valRuleArrayController selectedObjects] objectAtIndex:0];
	[self beginValRuleEdit:selectedRule];
}

- (IBAction) addValueRuleClicked:(id)sender
{
	if ([[triggerArrayController selectedObjects] count] < 1) return;
	LCTriggersetTrigger *selectedTrg = [[triggerArrayController selectedObjects] objectAtIndex:0];	
	LCTriggersetValRule *newRule;
	newRule = [[LCTriggersetValRule alloc] initWithObject:object 
											   triggerset:[selectedTrg triggerset]
												  trigger:selectedTrg
											 criteriaSite:[object site] 
										   criteriaDevice:[object device]
										   criteriaObject:object
										  criteriaTrgType:1
										   criteriaXValue:@"0"
										   criteriaYValue:@"" 
										 criteriaDuration:@"0"
									   criteriaAdminstate:0];
	[self beginValRuleEdit:newRule];	
	[newRule autorelease];
}

- (IBAction) deleteValueRuleClicked:(id)sender
{
	/* Find selected */
	if ([[valRuleArrayController selectedObjects] count] < 1) return;
	LCTriggersetValRule *selectedRule = [[valRuleArrayController selectedObjects] objectAtIndex:0];
	
	/* Show sheet */
	[self showDeleteSheet];
	
	/* Delete */
	[selectedRule setDelegate:self];
	[selectedRule delete];
}

- (IBAction) refreshValueRulesClicked:(id)sender
{
	if ([[triggerArrayController selectedObjects] count] < 1) return;
	LCTriggersetTrigger *selectedTrg = [[triggerArrayController selectedObjects] objectAtIndex:0];	
	[[selectedTrg valRules] highPriorityRefresh];
}

- (void) beginValRuleEdit:(LCTriggersetValRule *)rule
{
	/* Set edit rule */
	[self setEditValRule:rule];
	
	/* Set header */
	if ([rule ID])
	{ 
		[valHeaderLabel setStringValue:@"Edit Existing Trigger Value Rule"]; 
		[valSubmitButton setTitle:@"Submit"];
	}
	else
	{ 
		[valHeaderLabel setStringValue:@"Add New Trigger Value Rule"]; 
		[valSubmitButton setTitle:@"Add"];
	}
	
	/* Reset status */
	[valStatusLabel setStringValue:@""];
	[valProgress setHidden:YES];
	[valSubmitButton setEnabled:YES];
	[valCancelButton setEnabled:YES];
	
	/* Set labels */
	if ([[rule triggerset] desc])
	{ [valTsetLabel setStringValue:[[rule triggerset] desc]]; }
	else
	{ [valTsetLabel setStringValue:@""]; }

	if ([[rule triggerset] metricDesc])
	{ [valMetricLabel setStringValue:[[rule triggerset] metricDesc]]; } 
	else [valMetricLabel setStringValue:@""];

	if ([[[rule object] container] desc])
	{ [valContLabel setStringValue:[[[rule object] container] desc]]; }
	else
	{ [valContLabel setStringValue:@""]; }

	[valTrgLabel setStringValue:[rule triggerDesc]];
	
	/* Site criteria */
	[valSiteCombo removeAllItems];
	[valSiteCombo addItemWithObjectValue:[[[rule object] site] desc]];
	[valSiteCombo addItemWithObjectValue:@"*ALL*"];
	if([rule siteName])
	{ [valSiteCombo selectItemAtIndex:0]; }
	else
	{ [valSiteCombo selectItemAtIndex:1]; }
	
	/* Device criteria */
	[valDeviceCombo removeAllItems];
	[valDeviceCombo addItemWithObjectValue:[[[rule object] device] desc]];
	[valDeviceCombo addItemWithObjectValue:@"*ALL*"];
	if([rule deviceName])
	{ [valDeviceCombo selectItemAtIndex:0]; }
	else
	{ [valDeviceCombo selectItemAtIndex:1]; }
	
	/* Object criteria */
	[valObjectCombo removeAllItems];
	[valObjectCombo addItemWithObjectValue:[[rule object] desc]];
	[valObjectCombo addItemWithObjectValue:@"*ALL*"];
	if([rule objectName])
	{ [valObjectCombo selectItemAtIndex:0]; }
	else
	{ [valObjectCombo selectItemAtIndex:1]; }
	
	/* Condition/Trigger type */
	[valConditionCombo selectItemAtIndex:([[rule triggerTypeInteger] intValue] - 1)];
	
	/* Value */
	if ([rule xValue])
	{ [valXText setStringValue:[rule xValue]]; }
	else
	{ [valXText setStringValue:@""]; }
	if ([rule units])
	{ 
		[valXUnitsText setStringValue:[rule units]]; 
		[valYUnitsText setStringValue:[rule units]]; 
	}
	else
	{ 
		[valXUnitsText setStringValue:@""]; 
		[valYUnitsText setStringValue:@""]; 
	}
	if ([rule yValue])
	{ [valYText setStringValue:[rule yValue]]; }
	else
	{ [valYText setStringValue:@""]; }
	
	/* Duration */
	[valDurationText setStringValue:[rule duration]];
	
	/* Effect */
	[valEffectLabel setStringValue:[[rule trigger] effect]];
	
	/* Admin state */
	[valAdminCombo selectItemAtIndex:[[rule adminStateInteger] intValue]];
	
	/* Open sheet */
	[valRuleEditSheet setBackgroundColor:[NSColor colorWithCalibratedWhite:0.8 alpha:1.0]];
	[NSApp beginSheet:valRuleEditSheet
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
	valEditSheetShown = YES;	
}

- (IBAction) valRuleSubmitClicked:(id)sender
{
	/* Set delegate */
	[editValRule setDelegate:self];
	
	/* Update the site */
	if ([valSiteCombo indexOfSelectedItem] == 0)
	{ 
		[editValRule setSiteName:[[[editValRule object] site] name]];
		[editValRule setSiteDesc:[[[editValRule object] site] desc]];
	}
	else
	{
		[editValRule setSiteName:nil];
		[editValRule setSiteDesc:nil];
	}
	
	/* Update the device */
	if ([valDeviceCombo indexOfSelectedItem] == 0)
	{ 
		[editValRule setDeviceName:[[[editValRule object] device] name]];
		[editValRule setDeviceDesc:[[[editValRule object] device] desc]];
	}
	else
	{
		[editValRule setDeviceName:nil];
		[editValRule setDeviceDesc:nil];
	}
	
	/* Update the object */
	if ([valObjectCombo indexOfSelectedItem] == 0)
	{ 
		[editValRule setObjectName:[[editValRule object] name]];
		[editValRule setObjectDesc:[[editValRule object] desc]];
	}
	else
	{
		[editValRule setObjectName:nil];
		[editValRule setObjectDesc:nil];
	}
	
	/* Update the condition/type */
	[editValRule setTriggerType:([valConditionCombo indexOfSelectedItem]+1)];
	
	/* Values */
	[editValRule setXValue:[valXText stringValue]];
	[editValRule setYValue:[valYText stringValue]];
	
	/* Duration */
	[editValRule setDuration:[valDurationText stringValue]];
	
	/* Admin state */
	[editValRule setAdminState:[valAdminCombo indexOfSelectedItem]];
	
	/* Adjust sheet */
	[valSubmitButton setEnabled:NO];
	[valCancelButton setEnabled:NO];
	[valProgress setHidden:NO];
	[valProgress startAnimation:self];
	[valStatusLabel setStringValue:@"Submitting rule..."];
	
	/* Update rule */
	[editValRule update];	
}

- (void) valRuleOperationFinished:(LCTriggersetValRule *)rule
{
	/* Refresh */
	[[[rule trigger] valRules] highPriorityRefresh];
	[tsets highPriorityRefresh];
	
	/* Close sheets */
	if (valEditSheetShown)
	{
		/* Adjust sheet */
		[valStatusLabel setStringValue:@"Completed"];
		
		/* Close sheet */
		[NSApp endSheet:valRuleEditSheet];
		[valRuleEditSheet close];
		valEditSheetShown = NO;
		
		/* Release rule */
		[self setEditValRule:nil];
	}
	if (deleteSheetShown)
	{
		/* Remove from list */
//		[NSTimer scheduledTimerWithTimeInterval:0.0 target:[[rule trigger] valRules] selector:@selector(removeRule:) userInfo:nil repeats:NO];
		[[[rule trigger] valRules] performSelector:@selector(removeRule:) withObject:rule afterDelay:0.0];
//		[[[rule trigger] valRules] removeObjectFromRulesAtIndex:[[[[rule trigger] valRules] rules] indexOfObject:rule]];
		
		/* Close sheet */
		[self endDeleteSheet]; 
		deleteSheetShown = NO;
	}
}

- (IBAction) valRuleCancelClicked:(id)sender
{
	/* Release rule */
	[self setEditValRule:nil];
	
	/* Close sheet */
	[NSApp endSheet:valRuleEditSheet];
	[valRuleEditSheet close];
	valEditSheetShown = NO;
}

#pragma mark "General UI Actions"

- (IBAction) closeClicked:(id)sender
{
	[NSApp endSheet:[self window]];
	[[self window] close];
}

#pragma mark "Delete Sheet"

- (void) showDeleteSheet
{
	/* Open sheet */
	[ruleDeleteSheet setBackgroundColor:[NSColor colorWithCalibratedWhite:0.8 alpha:1.0]];
	[NSApp beginSheet:ruleDeleteSheet
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
	deleteSheetShown = YES;	
	[ruleDeleteProgress startAnimation:self];
}

- (void) endDeleteSheet
{
	[ruleDeleteProgress stopAnimation:self];
	[NSApp endSheet:ruleDeleteSheet];
	[ruleDeleteSheet close];
	deleteSheetShown = NO;
}

#pragma mark "Accessors"

@synthesize object;
@synthesize editAppRule;
@synthesize editValRule;
@synthesize conditions;
@synthesize effects;

@end
