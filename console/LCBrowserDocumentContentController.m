//
//  LCBrowserDocumentContentController.m
//  Lithium Console
//
//  Created by James Wilson on 23/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserDocumentContentController.h"


@implementation LCBrowserDocumentContentController

#pragma mark "Constructors"

- (id) initWithNibName:(NSString *)nibName document:(LCDocument *)initDocument inBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithNibName:nibName bundle:nil];
	[self loadView];
	
	self.browser = initBrowser;
	self.document = initDocument;
	[self getDocument];
	
	/* Setup Breadcrum View */
	[crumView bind:@"editing" toObject:self.document withKeyPath:@"editing" options:nil];
	crumView.delegate = self;
	
	return self;
}

- (void) dealloc
{
	[document release];
	[super dealloc];
}

#pragma mark "Browser Delegate"

- (BOOL) treeSelectionCanChangeToRepresent:(id)obj
{
	if ([[obj class] isSubclassOfClass:[LCDocument class]])
	{
		LCDocument *candidateDocument = (LCDocument *) obj;
		if (candidateDocument.customer == document.customer && candidateDocument.documentID == document.documentID) return YES;
	}
	if (document.editing)
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Document has unsaved changes"
										 defaultButton:@"Save"
									   alternateButton:@"Cancel" 
										   otherButton:nil 
							 informativeTextWithFormat:@"This document has been edited but the changes have not been saved. Click on 'Save' to commit the changes to Lithium."];
		[alert beginSheetModalForWindow:[[self view] window]
						  modalDelegate:self
						 didEndSelector:@selector(unsavedAlertDidEnd:returnCode:contextInfo:)
							contextInfo:nil];
		
		return NO;
	}
	else
	{ 
		return YES;
	}
}

- (void) unsavedAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo;
{
	if (returnCode == NSOKButton)
	{
		[self commitDocument];
	}
}

#pragma mark "UI Actions"

- (IBAction) editClicked:(id)sender
{
	if (document.editing)
	{
		/* Save Clicked */
		[self commitDocument];
	}
	else
	{
		/* Edit Clicked */
		[self editDocument];
	}
}

- (IBAction) saveClicked:(id)sender
{
	[self commitDocument];
}

- (IBAction) cancelClicked:(id)sender
{
	if (document.editing)
	{
		[self cancelEditDocument];
	}
	[editButton setTitle:@"Edit"];
}

- (IBAction) breadcrumClicked:(id)sender
{
}

#pragma mark "XML Methods"

- (void) getDocument
{
	[self performXmlAction:@"document_get" force:NO];
}

- (void) editDocument
{
	[self performXmlAction:@"document_edit" force:NO];
}

- (void) cancelEditDocument
{
	[self performXmlAction:@"document_edit_cancel" force:NO];
	self.editing = NO;
}

- (void) commitDocument
{
	[self performXmlAction:@"document_commit" force:NO];	
	self.editing = NO;
}

- (void) deleteDocument
{
	[self performXmlAction:@"document_delete" force:NO];	
	self.editing = NO;
}

- (void) performXmlAction:(NSString *)xmlName force:(BOOL)force
{
	/* Create XML */
	NSXMLDocument *xmldoc = [document xmlDocument];
	NSXMLElement *rootnode = (NSXMLElement *) [xmldoc rootElement];
	[rootnode addChild:[NSXMLNode elementWithName:@"force" 
									  stringValue:[NSString stringWithFormat:@"%i", force]]];	
	
	/* Create and perform request */
	LCXMLRequest *xmlRequest = [[LCXMLRequest requestWithCriteria:(LCCustomer *)document.customer
														 resource:[(LCCustomer *)document.customer resourceAddress]
														   entity:[(LCCustomer *)document.customer entityAddress]											
														  xmlname:xmlName
														   refsec:0
														   xmlout:xmldoc] retain];
	[xmlRequest setDelegate:self];
	[xmlRequest setThreadedXmlDelegate:self];
	[xmlRequest setPriority:XMLREQ_PRIO_HIGH];
	[xmlRequest performAsyncRequest];
	
	/* Set Status */
	self.xmlAction = xmlName;
	self.xmlOperationInProgress = YES;		
}

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	/* Retain XML data for main-thread parsing */
	xmlNode = [rootNode retain];
	
	/* Update document */
	[document setXmlValuesUsingXmlNode:rootNode];
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	if ([xmlAction isEqualToString:@"document_edit"])
	{
		/* Check Status */
		NSString *result = [xmlNode.properties objectForKey:@"result"];
		if ([result isEqualToString:@"LOCKED"])
		{
			/* File is marked as already being edited */
			if ([[xmlNode.properties objectForKey:@"editor"] isEqualToString:[(LCCustomer *)document.customer username]])
			{
				/* We're the editor */
				NSAlert *alert = [NSAlert alertWithMessageText:@"Document is already locked for editing by you."				 
												 defaultButton:@"Edit"
											   alternateButton:@"Cancel"
												   otherButton:nil
									 informativeTextWithFormat:@"This document is already marked as being open for editing by you. This may be from a previous session or due to the document being edited in another window. Click Edit Anyway below to start a new edit session for the document in this window."];
				[alert beginSheetModalForWindow:[[self view] window]
								  modalDelegate:self
								 didEndSelector:@selector(lockedAlertDidEnd:returnCode:contextInfo:)
									contextInfo:nil];
			}
			else
			{
				/* Another user is the editor */
				NSAlert *alert = [NSAlert alertWithMessageText:[NSString stringWithFormat:@"Document is already locked for editing by %@", document.editor]
												 defaultButton:@"Edit"
											   alternateButton:@"Cancel"
												   otherButton:nil
									 informativeTextWithFormat:[NSString stringWithFormat:@"The user %@ is currently editing this document. Click Edit Anyway below to edit the document but please note that any changes you make may override or be overridden by the other user.", document.editor]];
				[alert beginSheetModalForWindow:[[self view] window]
								  modalDelegate:self
								 didEndSelector:@selector(lockedAlertDidEnd:returnCode:contextInfo:)
									contextInfo:nil];
			}				
		}
		else if ([result isEqualToString:@"OK"])
		{
			/* File is now ready to be edited */
			self.editing = YES;
		}
		else
		{
			/* Failed to lock the file */
			self.editing = NO;
			NSString *message = [xmlNode.properties objectForKey:@"message"] ? : @"Console was unable to request a lock on the document for editing";			
			NSAlert *alert = [NSAlert alertWithMessageText:@"Failed to lock document for editing"
											 defaultButton:@"Cancel"
										   alternateButton:nil
											   otherButton:nil
								 informativeTextWithFormat:message];
			[alert beginSheetModalForWindow:[[self view] window]
							  modalDelegate:self
							 didEndSelector:nil
								contextInfo:nil];		
		}
	}
	
	/* Update status */
	self.xmlOperationInProgress = NO;
	self.xmlAction = nil;
	
	/* Cleanup */
	[sender release];
}

- (void) lockedAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo;
{
	if (returnCode == NSOKButton)
	{
		/* Force the edit */
		[self performXmlAction:@"document_edit" force:YES];
	}
}

#pragma mark "UI Validation"
	  
- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item
{
	SEL action = [item action];
	
	if (action == @selector(editClicked:))
	{ if (!document.editing) return YES; }
	
	if (action == @selector(cancelClicked:))
	{ if (document.editing) return YES; }
	
	if (action == @selector(saveClicked:))
	{ if (document.editing) return YES; }

	return NO;
}  

#pragma mark "Properties"

@synthesize browser;
@synthesize document;
@synthesize editing;
- (void) setEditing:(BOOL)flag
{
	document.editing = flag;
	editing = flag;
	if (editing) [editButton setTitle:@"Save"];
	else [editButton setTitle:@"Edit"];
}
@synthesize xmlOperationInProgress;
- (void) setXmlOperationInProgress:(BOOL)flag
{
	xmlOperationInProgress = flag;
	document.xmlOperationInProgress = flag;
}
@synthesize xmlAction;


@end
