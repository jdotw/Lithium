//
//  LCBrowserScriptsContentController.m
//  Lithium Console
//
//  Created by James Wilson on 10/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserScriptsContentController.h"


@implementation LCBrowserScriptsContentController

#pragma mark "Constructors"

- (id) initWithCustomer:(LCCustomer *)initCustomer 
			  inBrowser:(LCBrowser2Controller *)initBrowser 
			   usingNib:(NSString *)nibName
		  forScriptType:(NSString *)initType
{
	self = [super initWithNibName:nibName bundle:nil];
	if (!self) return nil;
	
	/* Set properties */
	self.customer = initCustomer;
	self.browser = initBrowser;
	self.type = initType;
	
	/* Load NIB */
	[self loadView];
	
	/* Load Common Content NIB */
	[NSBundle loadNibNamed:@"ScriptsCommonContent" owner:self];
	
	/* Observe Selection */
	[scriptArrayController addObserver:self 
							forKeyPath:@"selection" 
							   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
							   context:nil];
	
	return self;
}

- (void) removedFromBrowserWindow
{
	/* Shutdown and prepare to be autoreleased */
	[scriptArrayController removeObserver:self forKeyPath:@"selection"];
	[super removedFromBrowserWindow];
}

- (void) dealloc
{
	[customer release];
	[selectedScripts release];
	[super dealloc];
}

#pragma mark "Selection (KVO Observable)"

@synthesize selectedScript;
@synthesize selectedScripts;
- (void) setSelectedScripts:(NSArray *)value
{
	[selectedScripts release];
	selectedScripts = [value copy];
	
	if (selectedScripts.count > 0) self.selectedScript = [selectedScripts objectAtIndex:0];
	else self.selectedScript = nil;
}

#pragma mark "KVO and Notification Observing"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (object == scriptArrayController)
	{
		self.selectedScripts = [scriptArrayController selectedObjects];
	}
}

#pragma mark "UI Actions"
- (IBAction) uploadScriptClicked:(id)sender
{
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];
	[openPanel setTitle:@"Select script to upload (2Mb max)"];
	[openPanel setAllowsMultipleSelection:NO];
	
	int result = [openPanel runModalForDirectory:nil file:nil];
	if (result == NSOKButton)
	{
		uploadRequest = [[LCScriptUploadRequest uploadScript:[openPanel filename] 
														type:self.type
												 forCustomer:customer] retain];
		[uploadRequest setDelegate:self];
		[uploadRequest performAsyncRequest];
		
		/* Open sheet */
		[NSApp beginSheet:uploadSheet
		   modalForWindow:[browser window] 
			modalDelegate:self 
		   didEndSelector:nil 
			  contextInfo:nil];			
	}	
}

- (void) scriptUploadFinished:(id)sender
{
	[NSApp endSheet:uploadSheet];
	[uploadSheet close];
	
	if (!uploadRequest.success)
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Failed to upload script"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"An error occurred while uploading the script."];
		[alert beginSheetModalForWindow:[browser window]
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
	}

	if ([self respondsToSelector:@selector(scriptList)])
	{
		LCScriptList *scriptList = [self valueForKey:@"scriptList"];
		[scriptList highPriorityRefresh];
	}
	
	[uploadRequest release];
	uploadRequest = nil;
}

- (IBAction) uploadCancelClicked:(id)sender
{
	[NSApp endSheet:uploadSheet];
	[uploadSheet close];
	[uploadRequest cancel];
	[uploadRequest release];
	uploadRequest = nil;
}

- (IBAction) downloadSelectedScriptClicked:(id)sender
{
	if (!self.selectedScript) return;
	
	NSSavePanel *savePanel = [NSSavePanel savePanel];
	[savePanel setTitle:@"Save script to..."];
	int result = [savePanel runModalForDirectory:nil file:self.selectedScript.name];
	if (result == NSOKButton)
	{
		downloadRequest = [[LCScriptDownloadRequest downloadScript:self.selectedScript.name
															  type:self.type
													   forCustomer:customer
																to:[savePanel filename]] retain];
		[downloadRequest setDelegate:self];
		[downloadRequest performAsyncRequest];
		
		/* Open sheet */
		[NSApp beginSheet:downloadSheet
		   modalForWindow:[browser window] 
			modalDelegate:self 
		   didEndSelector:nil 
			  contextInfo:nil];			
	}	
	
}

- (void) scriptDownloadFinished:(id)sender
{
	[NSApp endSheet:downloadSheet];
	[downloadSheet close];
	
	if (!downloadRequest.success)
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Failed to download script"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"An error occurred while downloading the script."];
		[alert beginSheetModalForWindow:[browser window]
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
	}
	
	if ([self respondsToSelector:@selector(scriptList)])
	{
		LCScriptList *scriptList = [self valueForKey:@"scriptList"];
		[scriptList highPriorityRefresh];
	}
	
	[downloadRequest release];
	downloadRequest = nil;	
}

- (IBAction) downloadCancelClicked:(id)sender
{
	[NSApp endSheet:downloadSheet];
	[downloadSheet close];
	[downloadRequest cancel];
	[downloadRequest release];
	downloadRequest = nil;
}

- (IBAction) deleteSelectedScriptClicked:(id)sender
{
	/* Deletes script */
	if (!selectedScript) return;
	
	NSAlert *alert = [NSAlert alertWithMessageText:[NSString stringWithFormat:@"Script %@ will be Deleted", selectedScript.name]
									 defaultButton:@"Delete"
								   alternateButton:@"Cancel"
									   otherButton:nil
						 informativeTextWithFormat:@"This action can not be undone."];
	[alert setAlertStyle:NSCriticalAlertStyle];
	[alert beginSheetModalForWindow:[browser window]
					  modalDelegate:self
					 didEndSelector:@selector(deleteAlertDidEnd:returnCode:contextInfo:)
						contextInfo:nil];
}

- (void) deleteAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	/* Create and perform request */
	LCXMLRequest *req = [[LCXMLRequest requestWithCriteria:customer
												  resource:[customer resourceAddress]
													entity:[customer entityAddress]
												   xmlname:@"script_delete"
													refsec:0
													xmlout:[selectedScript xmlDocument]] retain];
	[req setPriority:XMLREQ_PRIO_HIGH];
	[req performAsyncRequest];
	
	/* Remove from list */
	if ([self respondsToSelector:@selector(scriptList)])
	{
		LCScriptList *scriptList = [self valueForKey:@"scriptList"];
		[scriptList removeObjectFromScriptsAtIndex:[scriptList.scripts indexOfObject:selectedScript]];
	}	
}

- (IBAction) refreshScriptListClicked:(id)sender
{
	if ([self respondsToSelector:@selector(scriptList)])
	{
		LCScriptList *scriptList = [self valueForKey:@"scriptList"];
		[scriptList highPriorityRefresh];
	}	
}


#pragma mark "Properties"

@synthesize browser;
@synthesize customer;
@synthesize type;

@end
