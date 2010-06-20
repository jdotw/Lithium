//
//  LCServiceEditWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 7/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCServiceEditWindowController.h"
#import "LCServiceScriptManagerController.h"
#import "LCScriptConfigVariable.h"

@implementation LCServiceEditWindowController

#pragma mark "Constructors"

+ (LCServiceEditWindowController *) beginSheetForNewService:(LCDevice *)initDevice
											 windowForSheet:(NSWindow *)initWindow
												   delegate:(id)initDelegate
{
	return [[[LCServiceEditWindowController alloc] initForNewServiceUsingScript:nil
																	   atDevice:initDevice
																 windowForSheet:initWindow
																	   delegate:initDelegate] autorelease];
}

+ (LCServiceEditWindowController *) beginSheetForNewHTTPService:(LCDevice *)initDevice
												 windowForSheet:(NSWindow *)initWindow
													   delegate:(id)initDelegate
{
	return [[[LCServiceEditWindowController alloc] initForNewServiceUsingScript:@"http_check.pl"
																	   atDevice:initDevice
																 windowForSheet:initWindow
																	   delegate:initDelegate] autorelease];
}

+ (LCServiceEditWindowController *) beginSheetForNewSMTPService:(LCDevice *)initDevice
												 windowForSheet:(NSWindow *)initWindow
													   delegate:(id)initDelegate
{
	return [[[LCServiceEditWindowController alloc] initForNewServiceUsingScript:@"smtp_check.pl"
																	   atDevice:initDevice
																 windowForSheet:initWindow
																	   delegate:initDelegate] autorelease];
}

+ (LCServiceEditWindowController *) beginSheetForNewPOPService:(LCDevice *)initDevice
												 windowForSheet:(NSWindow *)initWindow
													   delegate:(id)initDelegate
{
	return [[[LCServiceEditWindowController alloc] initForNewServiceUsingScript:@"pop_check.pl"
																	   atDevice:initDevice
																 windowForSheet:initWindow
																	   delegate:initDelegate] autorelease];
}

+ (LCServiceEditWindowController *) beginSheetForNewIMAPService:(LCDevice *)initDevice
												 windowForSheet:(NSWindow *)initWindow
													   delegate:(id)initDelegate
{
	return [[[LCServiceEditWindowController alloc] initForNewServiceUsingScript:@"imap_check.pl"
																	   atDevice:initDevice
																 windowForSheet:initWindow
																	   delegate:initDelegate] autorelease];
}

+ (LCServiceEditWindowController *) beginSheetForNewDNSService:(LCDevice *)initDevice
												 windowForSheet:(NSWindow *)initWindow
													   delegate:(id)initDelegate
{
	return [[[LCServiceEditWindowController alloc] initForNewServiceUsingScript:@"dns_check.pl"
																	   atDevice:initDevice
																 windowForSheet:initWindow
																	   delegate:initDelegate] autorelease];
}

+ (LCServiceEditWindowController *) beginSheetForServiceToEdit:(LCObject *)initObject
												windowForSheet:(NSWindow *)initWindow
													  delegate:(id)initDelegate
{
	return [[[LCServiceEditWindowController alloc] initWithServiceObject:initObject 
														  windowForSheet:initWindow
																delegate:initDelegate] autorelease];
}

+ (LCServiceEditWindowController *) beginSheetForServiceToDelete:(LCObject *)initObject
												  windowForSheet:(NSWindow *)initWindow
{
	return [[[LCServiceEditWindowController alloc] initWithServiceToDelete:initObject
															windowForSheet:initWindow] autorelease];
}

- (id) initForNewServiceUsingScript:(NSString *)scriptName
						  atDevice:(LCDevice *)initDevice 
					 windowForSheet:(NSWindow *)initWindow
						   delegate:(id)initDelegate
{
	/*
	 * Initializer for NEW Service (Add)
	 */
	
	/* Super-class init */
	self = [self initWithDevice:initDevice
				 windowForSheet:initWindow
					   delegate:initDelegate];
	if (!self) return nil;
	
	/* Create service */
	self.service = [LCService newServiceForDevice:device];
	service.delegate = self;
	[service.scriptList highPriorityRefresh];
	
	/* Setup assistant */
	if (scriptName)
	{
		/* Switch to assistant view */
		[self setContentIndex:1];
		[assistantTabView selectTabViewItemWithIdentifier:scriptName];
		[self showStep0];
		
		/* Get notification of refresh to set selected script */
		[service.scriptList setDelegate:self];
	}
	
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

- (id) initWithServiceObject:(LCObject *)initServiceObject
			  windowForSheet:(NSWindow *)initWindow
					delegate:(id)initDelegate
{
	/*
	 * Initializer for EXISTING Service (Edit)
	 */

	/* Super-class init */
	self = [self initWithDevice:[initServiceObject device]
				 windowForSheet:initWindow
					   delegate:initDelegate];
	if (!self) return nil;
	
	/* Create service */
	self.service = [LCService serviceWithObject:initServiceObject];
	service.delegate = self;
	[service performXmlRefresh];
	[service.scriptList highPriorityRefresh];
	[service.configVariables highPriorityRefresh];
	
	/* Refresh config variables */
	[[service configVariables] highPriorityRefresh];
	
	/* Window setup */
	[saveButton setTitle:@"Save"];
	
	/* Display */
	[NSApp beginSheet:[self window]
	   modalForWindow:windowForSheet
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];		
	
	return self;
}

- (id) initWithDevice:(LCDevice *)initDevice
	   windowForSheet:(NSWindow *)initWindow
			 delegate:(id)initDelegate
{
	/* 
	 * Common (Add+Update) Initializer
	 */
	
	self = [super initWithWindowNibName:@"ServiceEditWindow"];
	if (!self) return nil;

	/* Load the NIB */
	[self window];
	[self setContentIndex:0];
	
	/* Set Device */
	self.device = initDevice;
	self.windowForSheet = initWindow;
	self.delegate = initDelegate;
	
	return self;
}

- (id) initWithServiceToDelete:(LCObject *)initServiceObject windowForSheet:(NSWindow *)initWindow
{
	self = [super init];
	if (!self) return nil;
	
	self.service = [LCService serviceWithObject:initServiceObject];
	self.service.delegate = self;
	self.device = initServiceObject.device;
	self.windowForSheet = initWindow;
	
	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm Service Delete"
									 defaultButton:@"Delete"
								   alternateButton:@"Cancel"
									   otherButton:nil
						 informativeTextWithFormat:[NSString stringWithFormat:@"The %@ Monitored Service will be removed and will no longer be checked.", initServiceObject.desc]];
	[alert setAlertStyle:NSCriticalAlertStyle];
	[alert beginSheetModalForWindow:windowForSheet
					  modalDelegate:self
					 didEndSelector:@selector(removeServiceAlertDidEnd:returnCode:contextInfo:)
						contextInfo:nil];
	[self retain];
	
	return self;
}

- (void) dealloc
{
	[service release];
	[windowForSheet release];
	[device release];
	[assistantIdentifier release];
	[super dealloc];
}

#pragma mark "Window Management"

- (void) windowWillClose:(NSNotification *)notification
{
	[controllerAlias setContent:nil];
}

#pragma mark "UI Action"

- (IBAction) saveClicked:(id)sender
{
	/* Validate */
	if ([service.desc length] < 1)
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"A description for the Service must be set"
										 defaultButton:@"OK" 
									   alternateButton:nil 
										   otherButton:nil 
							 informativeTextWithFormat:@"Please set a Description and try again."];
		[alert setIcon:[NSImage imageNamed:@"ok_48.tif"]];
		[alert runModal];
		return;
	}
	if (!service.selectedScript)
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"No Service Script was selected."
										 defaultButton:@"OK" 
									   alternateButton:nil 
										   otherButton:nil 
							 informativeTextWithFormat:@"Please select an Service Script and try again."];
		[alert setIcon:[NSImage imageNamed:@"ok_48.tif"]];
		[alert runModal];
		return;
	}
	NSEnumerator *varEnum = [[[service selectedScript] configVariables] objectEnumerator];
	LCScriptConfigVariable *var;
	while (var=[varEnum nextObject])
	{
		if ([var required] && (![var value] || [[var value] length] < 1))
		{
			NSString *inform = [NSString stringWithFormat:@"The script you have selected (%@) requires a %@ value be set.",
				[[service selectedScript] desc], [var desc]];
			NSAlert *alert = [NSAlert alertWithMessageText:@"A required script configuration variable is not set."
											 defaultButton:@"OK" 
										   alternateButton:nil 
											   otherButton:nil 
								 informativeTextWithFormat:inform];
			[alert setIcon:[NSImage imageNamed:@"ok_48.tif"]];
			[alert runModal];
			return;
		}
	}
	
	/* Perform add/update */
	if (service.taskID)
	{ 
		/* Update service */
		[service performXmlUpdate]; 
	}
	else
	{ 		
		/* Add service */
		[service performXmlAdd]; 
	}
}

- (IBAction) cancelClicked:(id)sender
{
	[NSApp endSheet:[self window]];
	[[self window] close];
}
	 
- (void) removeServiceAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSAlertDefaultReturn)
	{
		[service performXmlDelete];
	}
	else 
	{
		[self release];
	}
}


- (void) xmlObjectAddFinished:(id)sender
{
	/* Refresh our device */
	[self.device highPriorityRefresh];
	[[self.device.customer serviceList] highPriorityRefresh];
	
	/* Close sheet */
	[NSApp endSheet:[self window]];
	[[self window] close];
}

- (void) xmlObjectUpdateFinished:(id)sender
{
	/* Refresh our device */
	[self.device highPriorityRefresh];
	[[self.device.customer serviceList] highPriorityRefresh];
	
	/* Close sheet */
	[NSApp endSheet:[self window]];
	[[self window] close];
}

- (void) xmlObjectDeleteFinished:(id)sender
{
	/* Refresh device and groups */
	[self.device highPriorityRefresh];
	[[self.device.customer groupTree] highPriorityRefresh];
	[[self.device.customer serviceList] highPriorityRefresh];
	
	/* Release self */
	[self release];
}


#pragma mark "Assistant Methods"

- (void) scriptListRefreshFinished:(id)sender
{
	LCScript *script = [service.scriptList.scriptDict objectForKey:[[assistantTabView selectedTabViewItem] identifier]];
	if (script)
	{
		service.selectedScript = script;
		[self showStep1];		
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
							 informativeTextWithFormat:[NSString stringWithFormat:@"The %@ script is required to monitor this service type but was not found on the %@ Lithium Core deployment.",
														[[assistantTabView selectedTabViewItem] identifier], ((LCCustomer *)device.customer).desc]];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:windowForSheet
						  modalDelegate:nil
						 didEndSelector:nil
							contextInfo:nil];
	}
}

- (IBAction) nextClicked:(id)sender
{
	int curIndex = [stepTabView indexOfTabViewItem:[stepTabView selectedTabViewItem]];
	if (curIndex == 1)
	{ [self saveClicked:sender]; }
}

- (IBAction) backClicked:(id)sender
{
}

- (void) showStep0
{
	[self setStepIndex:0];
}

- (void) showStep1
{
	[self setStepIndex:1];
}

- (void) showStep2
{
	[self setStepIndex:2];
}

@synthesize contentIndex;
@synthesize assistantIdentifier;
@synthesize stepIndex;

#pragma mark "Accessors"
@synthesize service;
@synthesize device;
@synthesize windowForSheet;
@synthesize delegate;

@end
