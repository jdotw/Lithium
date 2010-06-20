//
//  LCImportDocument.m
//  Lithium Console
//
//  Created by James Wilson on 26/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCImportDocument.h"
#import "LCConsoleController.h"

@implementation LCImportDocument

#pragma mark "Constructors"

- (void) dealloc
{
	[document release];
	[customer release];
	[createRequest cancel];
	[createRequest release];
	[importStatusString release];
	[super dealloc];
}

#pragma mark "NSDocument Methods"

- (NSString *)windowNibName
{
    return @"DocumentImportWindow";
}

- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{
	/* Super-init */
    [super windowControllerDidLoadNib:aController];
	
	/* Set Customer */
	if ([[LCCustomerList masterArray] count] > 0)
	{ self.customer = [[LCCustomerList masterArray] objectAtIndex:0]; }

	/* Get Browser */
	self.browser = [[LCConsoleController masterController] browserForSheet];
	[NSApp beginSheet:sheetWindow
	   modalForWindow:[browser window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError
{	
	return nil;
}

- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{    
    return NO;
}

- (void) windowWillClose:(id)sender
{
	[controllerAlias setContent:nil];
}

#pragma mark "XML Methods"

- (NSXMLDocument *) xmlDocument
{
	/* Create XML */
	return [document xmlDocument];
}

- (void) import
{
	/* Create and perform request */
	createRequest = [[LCXMLRequest requestWithCriteria:customer
												 resource:[customer resourceAddress]
												   entity:[customer entityAddress]
												  xmlname:@"document_commit"
												   refsec:0
												   xmlout:[self xmlDocument]] retain];
	[createRequest setDelegate:self];
	[createRequest setThreadedXmlDelegate:self];
	[createRequest setPriority:XMLREQ_PRIO_HIGH];
	
	[createRequest performAsyncRequest];
	
	self.importInProgress = YES;
	self.importStatusString = @"Sending data to Lithium Core...";
	
}

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	if ([rootNode.properties objectForKey:@"id"])
	{
		self.importStatusString = @"Done.";
		importSucceeded = YES;
		[customer.documentList highPriorityRefresh];
	}
	else
	{
		importSucceeded = NO;
		NSAlert *alert = [NSAlert alertWithMessageText:@"Failed to Import Document"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"Lithium Console failed to send the document data to Lithium Core."];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:sheetWindow
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
	}
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	if (sender == createRequest)
	{
		[createRequest release];
		createRequest = nil;
	}
	
	self.importInProgress = NO;
	
	if (importSucceeded)
	{
		[NSApp endSheet:sheetWindow];
		[sheetWindow close];
	}
}

#pragma mark "UI Actions"

- (IBAction) importClicked:(id)sender
{
	[self import];
}

- (IBAction) cancelClicked:(id)sender
{
	[NSApp endSheet:sheetWindow];
	[sheetWindow close];
}

#pragma mark "Properties"

@synthesize customer;
@synthesize document;
@synthesize importInProgress;
@synthesize importStatusString;
@synthesize browser;
- (LCCustomerList *) customerList
{ return [LCCustomerList masterList]; }

@end
