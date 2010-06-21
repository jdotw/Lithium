//
//  LCImportDocument.h
//  Lithium Console
//
//  Created by James Wilson on 26/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"
#import "LCXMLRequest.h"
#import "LCBrowser2Controller.h"

@interface LCImportDocument : NSDocument
{
	/* Document */
	LCDocument *document;

	/* Related Objects */
	LCCustomer *customer;
	LCCustomerList *customerList;

	/* XML Operations */
	LCXMLRequest *createRequest;
	BOOL importInProgress;
	NSString *importStatusString;
	BOOL importSucceeded;
	
	/* UI Elements */
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet NSWindow *sheetWindow;
	LCBrowser2Controller *browser;
}

#pragma mark "XML Methods"
- (void) import;

#pragma mark "UI Actions"
- (IBAction) importClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;

#pragma mark "Properties"
@property (nonatomic,retain) LCCustomer *customer;
@property (nonatomic,retain) LCDocument *document;
@property (nonatomic, assign) BOOL importInProgress;
@property (nonatomic,copy) NSString *importStatusString;
@property (nonatomic,retain) LCBrowser2Controller *browser;
@property (readonly) LCCustomerList *customerList;

@end
