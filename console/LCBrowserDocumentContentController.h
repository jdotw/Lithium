//
//  LCBrowserDocumentContentController.h
//  Lithium Console
//
//  Created by James Wilson on 23/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2ContentViewController.h"
#import "LCBrowser2Controller.h"
#import "LCDocument.h"
#import "LCDocumentBreadcrumView.h"

@interface LCBrowserDocumentContentController : LCBrowser2ContentViewController 
{
	/* Document */
	LCDocument *document;
	
	/* Browser */
	LCBrowser2Controller *browser;
	
	/* State */
	BOOL editing;
	
	/* UI Elements */
	IBOutlet NSButton *editButton;
	IBOutlet LCDocumentBreadcrumView *crumView;
	
	/* XML */
	LCXMLNode *xmlNode;
	BOOL xmlOperationInProgress;
	NSString *xmlAction;
}

#pragma mark "Constructors"
- (id) initWithNibName:(NSString *)nibName document:(LCDocument *)initDocument inBrowser:(LCBrowser2Controller *)initBrowser;

#pragma mark "UI Elements"
- (IBAction) editClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;
- (IBAction) saveClicked:(id)sender;

#pragma mark "XML Method"
- (void) getDocument;
- (void) editDocument;
- (void) cancelEditDocument;
- (void) commitDocument;
- (void) deleteDocument;
- (void) performXmlAction:(NSString *)xmlName force:(BOOL)force;
- (void) XMLRequestFinished:(LCXMLRequest *)sender;

#pragma mark "UI Validation"
- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item;

#pragma mark "Properties"
@property (retain) LCDocument *document; 
@property (assign) LCBrowser2Controller *browser;
@property (assign) BOOL editing;
@property (assign) BOOL xmlOperationInProgress;
@property (copy) NSString *xmlAction;


@end
