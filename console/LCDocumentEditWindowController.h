//
//  LCDocumentEditWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 16/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"
#import "LCDocument.h"
#import "LCXMLRequest.h"

@interface LCDocumentEditWindowController : NSWindowController 
{
	/* UI Elements */
	IBOutlet NSTextField *titleTextField;
	IBOutlet NSButton *okButton;
	
	/* Document properties */
	NSString *desc;
	
	/* Related Objects */
	LCDocument *doc;
	LCCustomer *customer;
	NSWindow *windowForSheet;
	
	/* XML Properties */
	LCXMLRequest *xmlRequest;
	BOOL xmlOperationInProgress;
	NSString *status;
	BOOL shouldClose;
	BOOL addDocument;
}

#pragma mark "Constructors"
- (id) initForNewDocumentWithCustomer:(LCCustomer *)initCustomer 
								 type:(NSString *)initType
					   windowForSheet:(NSWindow *)initWindow;
- (id) initWithDocumentToEdit:(LCDocument *)initDocument 
					 customer:(LCCustomer *)initCustomer 
			   windowForSheet:(NSWindow *)initWindow;
- (id) initWithDocumentToDelete:(LCDocument *)initDocument
					   customer:(LCCustomer *)initCustomer
				 windowForSheet:(NSWindow *)initWindow;

#pragma mark "UI Actions"
- (IBAction) cancelClicked:(id)sender;
- (IBAction) saveClicked:(id)sender;

#pragma mark "Properties"
@property (copy) NSString *desc;
@property (retain) LCDocument *doc;
@property (retain) LCCustomer *customer;
@property (assign) BOOL xmlOperationInProgress;
@property (copy) NSString *status;
@property (retain) NSWindow *windowForSheet;

@end
