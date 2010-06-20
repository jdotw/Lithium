//
//  LCDocumentEditWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 16/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCDocumentEditWindowController.h"
#import "LCSSceneDocument.h"
#import "LCVRackDocument.h"

@interface LCDocumentEditWindowController (private)

- (void) performXmlAction:(NSString *)xmlName;
- (void) updateLiveDocument;

@end


@implementation LCDocumentEditWindowController

#pragma mark "Constructors"

- (id) initForNewDocumentWithCustomer:(LCCustomer *)initCustomer 
								 type:(NSString *)initType
					   windowForSheet:(NSWindow *)initWindow
{
	self = [self initWithWindowNibName:@"DocumentEditWindow" owner:self];
	if (!self) return nil;
	[self loadWindow];

	self.customer = initCustomer;
	self.windowForSheet = initWindow;
	if ([initType isEqualToString:@"scene"])
	{ self.doc = (LCDocument *) [[LCSSceneDocument new] autorelease]; }
	else if ([initType isEqualToString:@"vrack"])
	{ self.doc = (LCDocument *) [[LCVRackDocument new] autorelease]; }
	self.doc.type = initType;
	self.doc.customer = initCustomer;
	
	addDocument = YES;
	
	[titleTextField setStringValue:@"Create Document"];
	[okButton setTitle:@"Create"];
	
	[NSApp beginSheet:[self window]
	   modalForWindow:self.windowForSheet
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
	
	return self;
}

- (id) initWithDocumentToEdit:(LCDocument *)initDocument 
					 customer:(LCCustomer *)initCustomer 
			   windowForSheet:(NSWindow *)initWindow
{
	self = [self initWithWindowNibName:@"DocumentEditWindow" owner:self];
	if (!self) return nil;
	[self loadWindow];

	self.customer = initCustomer;
	self.doc = initDocument;
	self.desc = self.doc.desc;
	self.windowForSheet = initWindow;

	[titleTextField setStringValue:@"Rename Document"];
	[okButton setTitle:@"Save"];
	
	[NSApp beginSheet:[self window]
	   modalForWindow:self.windowForSheet
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];	
	
	return self;
}

- (id) initWithDocumentToDelete:(LCDocument *)initDocument
					   customer:(LCCustomer *)initCustomer
				 windowForSheet:(NSWindow *)initWindow
{
	self = [self initWithWindowNibName:@"DocumentEditWindow" owner:self];
	if (!self) return nil;
	[self loadWindow];
	
	self.customer = initCustomer;
	self.doc = initDocument;
	self.desc = self.doc.desc;
	self.windowForSheet = initWindow;
	
	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm Document Delete"
									 defaultButton:@"Delete"
								   alternateButton:@"Cancel"
									   otherButton:nil
						 informativeTextWithFormat:@"This action can not be undone."];
	
	[alert beginSheetModalForWindow:self.windowForSheet
					  modalDelegate:self
					 didEndSelector:@selector(deleteDocumentAlertDidEnd:returnCode:contextInfo:)
						contextInfo:nil];	
	
	return self;
	
}

- (void) dealloc
{
	[desc release];
	[doc release];
	[customer release];
	[super dealloc];
}

#pragma mark "UI Actions"

- (IBAction) cancelClicked:(id)sender
{
	[NSApp endSheet:[self window]];
	[[self window] close];
	[self autorelease];
}

- (IBAction) saveClicked:(id)sender
{
	[self updateLiveDocument];
	
	if (doc.documentID == 0)
	{ [self performXmlAction:@"document_create"]; }
	else
	{ [self performXmlAction:@"document_update"]; }
}

- (void) deleteDocumentAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		/* Perform delete */
		[self performXmlAction:@"document_delete"];
		[customer.documentList removeObjectFromDocumentsAtIndex:[customer.documentList.documents indexOfObject:doc]];
	}
}

#pragma mark "XML Methods"

- (void) updateLiveDocument
{
	/* Updates the live LCDocument based on the user input */
	if (self.desc && [self.desc length] > 0)
	{ self.doc.desc = self.desc; }
	else
	{ self.doc.desc = @"Untitled Document"; }
}

- (void) performXmlAction:(NSString *)xmlName
{
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"document"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"type" 
									  stringValue:self.doc.type]];
	[rootnode addChild:[NSXMLNode elementWithName:@"desc" 
									  stringValue:self.doc.desc]];
	if (self.doc.documentID > 0)
	{
		[rootnode addChild:[NSXMLNode elementWithName:@"id" 
										  stringValue:[NSString stringWithFormat:@"%i", self.doc.documentID]]];
	}

	
	/* Create and perform request */
	xmlRequest = [[LCXMLRequest requestWithCriteria:customer
										   resource:[customer resourceAddress]
											 entity:[customer entityAddress]
											xmlname:xmlName
											 refsec:0
											 xmlout:xmldoc] retain];
	[xmlRequest setDelegate:self];
	[xmlRequest setThreadedXmlDelegate:self];
	[xmlRequest setPriority:XMLREQ_PRIO_HIGH];
	[xmlRequest performAsyncRequest];

	/* Set Status */
	self.xmlOperationInProgress = YES;		
}

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	/* Check for Error */
	if ([rootNode.properties objectForKey:@"error"])
	{
		/* Error Occurred */
		self.status = [rootNode.properties objectForKey:@"error"];
		shouldClose = NO;
	}
	else if ([rootNode.properties objectForKey:@"id"])
	{
		/* ID Received */
		self.doc.documentID = [[rootNode.properties objectForKey:@"id"] intValue];
		shouldClose = YES;
		if (addDocument)
		{
			[customer.documentList insertObject:doc inDocumentsAtIndex:customer.documentList.documents.count];
		}
	}
	else if ([rootNode.properties objectForKey:@"result"])
	{
		shouldClose = YES;
	}
	else
	{
		/* Neither an error nor an ID received */
		self.status = @"ERROR: Server did not return a response.";
		shouldClose = NO;
	}
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Update status */
	self.xmlOperationInProgress = NO;
	if (![sender success])
	{ self.status = @"Failed to send request to Lithium Core"; }
	
	/* Close window if finished */
	if (shouldClose)
	{
		[NSApp endSheet:[self window]];
		[[self window] close];
		[self autorelease];
	}

	/* Cleanup */
	[xmlRequest release];
	xmlRequest = nil;
}

#pragma mark "Properties"

@synthesize desc;
@synthesize doc;
@synthesize customer;
@synthesize xmlOperationInProgress;
@synthesize status;
@synthesize windowForSheet;

@end
