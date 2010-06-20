//
//  LCActionEditWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 1/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCActionEditWindowController.h"

@implementation LCActionEditWindowController

#pragma mark "Constructors"

+ (LCActionEditWindowController *) beginSheetForNewAction:(LCCustomer *)initCustomer
										   windowForSheet:(NSWindow *)initWindow
												 delegate:(id)initDelegate
{
	return [[[LCActionEditWindowController alloc] initForNewAction:initCustomer
												   windowForSheet:initWindow
														  delegate:initDelegate] autorelease];
}

+ (LCActionEditWindowController *) beginSheetForNewEmailAction:(LCCustomer *)initCustomer
												windowForSheet:(NSWindow *)initWindow
													  delegate:(id)initDelegate
{
	return [[[LCActionEditWindowController alloc] initForNewEmailAction:initCustomer
														 windowForSheet:initWindow
															   delegate:initDelegate] autorelease];
	
}

+ (LCActionEditWindowController *) beginSheetForAction:(LCAction *)initAction
										windowForSheet:(NSWindow *)initWindow
											  delegate:(id)initDelegate
{
	return [[[LCActionEditWindowController alloc] initForAction:initAction
												 windowForSheet:initWindow
													   delegate:initDelegate] autorelease];	
}

+ (LCActionEditWindowController *) beginSheetToDeleteAction:(LCAction *)initAction
											 windowForSheet:(NSWindow *)initWindow
												   delegate:(id)initDelegate
{
	return [[[LCActionEditWindowController alloc] initWithActionToDelete:initAction
														  windowForSheet:initWindow
																delegate:initDelegate] autorelease];	
}

- (id) initForNewAction:(LCCustomer *)initCustomer
		 windowForSheet:(NSWindow *)initWindow
			   delegate:(id)initDelegate
{
	/* Super-class init */
	[self initWithCustomer:initCustomer windowForSheet:initWindow delegate:initDelegate];

	/* Create action */
	self.action = [[[LCAction alloc] initWithCustomer:self.customer] autorelease];
	action.activationMode = 1;
	action.delegate = self;
	[action.scriptList highPriorityRefresh];
	
	/* Window setup */
	[saveButton setTitle:@"Add"];

	/* Display */
	[NSApp beginSheet:[self window]
	   modalForWindow:windowForSheet
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];	
	
	return self;
}

- (id) initForNewEmailAction:(LCCustomer *)initCustomer
			  windowForSheet:(NSWindow *)initWindow
					delegate:(id)initDelegate
{
	/* Super-class init */
	[self initWithCustomer:initCustomer windowForSheet:initWindow delegate:initDelegate];
	
	/* Create action */
	self.action = [[[LCAction alloc] initWithCustomer:self.customer] autorelease];
	action.activationMode = 1;
	action.delegate = self;
	[action.scriptList setDelegate:self];
	[action.scriptList highPriorityRefresh];

	/* Setup window */
	self.contentIndex = 1;
	self.assistantIndex = 0;
	[self showStep0];
	
	/* Window setup */
	[saveButton setTitle:@"Add"];

	/* Display */
	[NSApp beginSheet:[self window]
	   modalForWindow:windowForSheet
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];		
	
	return self;
}

- (id) initForAction:(LCAction *)initAction
	  windowForSheet:(NSWindow *)initWindow
			delegate:(id)initDelegate
{
	/* Super-class init */
	[self initWithCustomer:initAction.hostEntity.customer windowForSheet:initWindow delegate:initDelegate];
	
	/* Create action */
	self.action = [initAction copy];
	action.delegate = self;
	[action.scriptList highPriorityRefresh];
	[action.entityList highPriorityRefresh];
	
	/* Refresh config variables */
	[action.configVariables highPriorityRefresh];
	
	/* Set script name */
	if (action.scriptName)
	{
		if ([action.scriptList.scriptDict objectForKey:action.scriptName])
		{ action.selectedScript = [action.scriptList.scriptDict objectForKey:action.scriptName]; }
		else
		{
			LCActionScript *script = [LCActionScript new];
			script.name = action.scriptName;
			script.desc = action.scriptName;
			[action.scriptList.scriptDict setObject:script forKey:script.name];
			[action.scriptList insertObject:script inScriptsAtIndex:[action.scriptList.scripts count]];
			action.selectedScript = script;
		}
	}
	
	/* Display */
	[NSApp beginSheet:[self window]
	   modalForWindow:windowForSheet
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];	

	return self;
}

- (id) initWithActionToDelete:(LCAction *)initAction
			   windowForSheet:(NSWindow *)initWindow
					 delegate:(id)initDelegate
{
	/* Super-class init */
	[self initWithCustomer:initAction.hostEntity.customer windowForSheet:initWindow delegate:initDelegate];
	
	/* Create action */
	self.action = initAction;
	
	/* Show Alert */
	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm Action Delete"
									 defaultButton:@"Delete"
								   alternateButton:@"Cancel"
									   otherButton:nil
						 informativeTextWithFormat:@"This action can not be undone."];
	[alert setAlertStyle:NSCriticalAlertStyle];
	[alert beginSheetModalForWindow:windowForSheet
					  modalDelegate:self
					 didEndSelector:@selector(deleteSheetEnded:returnCode:contextInfo:)
						contextInfo:nil];
	
	return self;
}


- (id) initWithCustomer:(LCCustomer *)initCustomer
		 windowForSheet:(NSWindow *)initWindow
			   delegate:(id)initDelegate
{
	/* Load the NIB */
	[super initWithWindowNibName:@"ActionEditWindow"];
	[self window];
	self.contentIndex = 0;
	
	/* UI Setup */
	[entityArrayController setAllowDrop:YES];
	[emailEntityArrayController setAllowDrop:YES];
	
	/* Set Properties */
	self.customer = initCustomer;
	self.windowForSheet = initWindow;
	self.delegate = initDelegate;
	
	return self;
}

- (void) dealloc
{
	[action release];
	[customer release];
	[super dealloc];
}

#pragma mark "Window Management"

- (void) windowWillClose:(NSNotification *)notification
{
	[controllerAlias setContent:nil];
}

#pragma mark "UI Actions"

- (IBAction) saveClicked:(id)sender
{
	/* Validate */
	if ([action.desc length] < 1)
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"A description for the Action must be set"
										 defaultButton:@"OK" 
									   alternateButton:nil 
										   otherButton:nil 
							 informativeTextWithFormat:@"Please set a Description and try again."];
		[alert setIcon:[NSImage imageNamed:@"tools_48.tif"]];
		[alert beginSheetModalForWindow:[self window]
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
		return;
	}
	if ([action.scriptName length] < 1)
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"No Action Script was selected."
										 defaultButton:@"OK" 
									   alternateButton:nil 
										   otherButton:nil 
							 informativeTextWithFormat:@"Please select an Action Script and try again."];
		[alert setIcon:[NSImage imageNamed:@"tools_48.tif"]];
		[alert beginSheetModalForWindow:[self window]
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
		return;
	}
	for (LCScriptConfigVariable *var in action.selectedScript.configVariables)
	{
		if (var.required && [var.value length] < 1)
		{
			NSString *inform = [NSString stringWithFormat:@"The script you have selected (%@) requires a %@ value be set.",
				[[action selectedScript] desc], [var desc]];
			NSAlert *alert = [NSAlert alertWithMessageText:@"A required script configuration variable is not set."
											 defaultButton:@"OK" 
										   alternateButton:nil 
											   otherButton:nil 
								 informativeTextWithFormat:inform];
			[alert setIcon:[NSImage imageNamed:@"tools_48.tif"]];
			[alert beginSheetModalForWindow:[self window]
							  modalDelegate:self
							 didEndSelector:nil
								contextInfo:nil];
			return;
		}
	}
	
	/* Perform add/update */
	if ([action taskID])
	{ 
		/* Update action */
		[action performXmlUpdate];
		if ([delegate respondsToSelector:@selector(actionEditDidUpdateAction:)])
		{ [delegate performSelector:@selector(actionEditDidUpdateAction:) withObject:action]; }
		[[NSNotificationCenter defaultCenter] postNotificationName:@"LCActionListChanged" object:customer];
	}
	else
	{ 		
		/* Add action */
		[action performXmlAdd]; 
		if ([delegate respondsToSelector:@selector(actionEditDidAddAction:)])
		{ [delegate performSelector:@selector(actionEditDidAddAction:) withObject:action]; }
		[[NSNotificationCenter defaultCenter] postNotificationName:@"LCActionListChanged" object:customer];
	}
}

- (void) xmlObjectAddFinished:(id)sender
{
	[NSApp endSheet:[self window]];
	[[self window] close]; 
}

- (void) xmlObjectUpdateFinished:(id)sender
{
	[NSApp endSheet:[self window]];
	[[self window] close]; 
}

- (IBAction) cancelClicked:(id)sender
{
	/* Clean up and close */
	[NSApp endSheet:[self window]];
	[[self window] close]; 
}

- (IBAction) deleteSelectedEntityClicked:(id)sender
{
	NSEnumerator *entityEnum = [[entityArrayController selectedObjects] objectEnumerator];
	LCEntity *entity;
	while (entity = [entityEnum nextObject])
	{ [[action entityList] removeObjectFromObjectsAtIndex:[[[action entityList] objects] indexOfObject:entity]]; }
}

- (void) deleteSheetEnded:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSOKButton)
	{ 
		[action performXmlDelete]; 
		if ([delegate respondsToSelector:@selector(actionEditDidDeleteAction:)])
		{ [delegate performSelector:@selector(actionEditDidDeleteAction:) withObject:action]; }
		[[NSNotificationCenter defaultCenter] postNotificationName:@"LCActionListChanged" object:customer];
	}	
}

#pragma mark "Assistant Methods"

- (void) scriptListRefreshFinished:(id)sender
{
	if ([self assistantIndex] == 0)
	{
		/* Email */
		if ([action.scriptList.scriptDict objectForKey:@"email_alert.pl"])
		{
			[action setSelectedScript:[action.scriptList.scriptDict objectForKey:@"email_alert.pl"]];
		}
		else
		{
			/* Script not found, close and display error */
			[NSApp endSheet:[self window]];
			[[self window] close];
			
			NSAlert *alert = [NSAlert alertWithMessageText:@"Required Script Not Found"
											 defaultButton:@"OK"
										   alternateButton:nil
											   otherButton:nil
								 informativeTextWithFormat:[NSString stringWithFormat:@"The email_alert.pl script is required to monitor this service type but was not found on the %@ Lithium Core deployment.", customer.desc]];
			[alert setAlertStyle:NSCriticalAlertStyle];
			[alert beginSheetModalForWindow:windowForSheet
							  modalDelegate:nil
							 didEndSelector:nil
								contextInfo:nil];
			return;
		}		
	}
	[self showStep1];
}

- (IBAction) nextClicked:(id)sender
{
	int curIndex = [stepTabView indexOfTabViewItem:[stepTabView selectedTabViewItem]];
	if (curIndex == 1)
	{ [self showStep2]; }
	else if (curIndex == 2)
	{ [self saveClicked:sender]; }
}

- (IBAction) backClicked:(id)sender
{
	int curIndex = [stepTabView indexOfTabViewItem:[stepTabView selectedTabViewItem]];
	if (curIndex == 2)
	{ [self showStep1]; }
}

- (void) showStep0
{
	[self setStepIndex:0];
	[nextButton setTitle:@"Next"];	
	[backButton setEnabled:NO];
}

- (void) showStep1
{
	[self setStepIndex:1];
	[nextButton setTitle:@"Next"];
	[backButton setEnabled:NO];
}

- (void) showStep2
{
	[self setStepIndex:2];
	[nextButton setTitle:@"Save"];
	[backButton setEnabled:YES];
}

@synthesize contentIndex;
@synthesize assistantIndex;
@synthesize stepIndex;

#pragma mark "Accessors"
@synthesize action;
- (void) setAction:(LCAction *)value
{ 
	[action setUpdateDelegate:nil];
	[action release];
	action = [value retain];
	[action setUpdateDelegate:self];
}
@synthesize customer;
@synthesize windowForSheet;
@synthesize delegate;

@end
