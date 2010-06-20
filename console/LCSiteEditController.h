//
//  LCSiteEditController.h
//  Lithium Console
//
//  Created by James Wilson on 27/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCSite.h"
#import "LCCustomer.h"

@interface LCSiteEditController : NSWindowController 
{
	/* Related Objects */
	LCCustomer *customer;
	LCSite *siteEntity;
	
	/* Site Properties */
	NSString *name;
	NSString *desc;
	NSString *addressLine1;
	NSString *addressLine2;
	NSString *addressLine3;
	NSString *suburb;
	NSString *state;
	NSString *postcode;
	NSString *country;
	float longitude;
	float latitude;
	
	/* UI Elements */
	NSButton *addButton;
	IBOutlet NSWindow *removeProgressSheet;

	/* Window for Sheet*/
	NSWindow *windowForSheet;

	/* XML Ops */
	BOOL xmlOperationInProgress;
	BOOL errorEncountered;
	NSString *statusString;
	NSImage *statusIcon;
	LCXMLRequest *xmlRequest;
	LCXMLRequest *removeXMLReq;
	NSMutableDictionary *xmlProperties;
	NSString *xmlElement;
	NSMutableString *xmlString;
}

#pragma mark "Constructors"
- (LCSiteEditController *) initForNewSiteAtCustomer:(LCCustomer *)initCustomer;
- (LCSiteEditController *) initWithSiteToEdit:(LCSite *)initSite;
- (LCSiteEditController *) initWithSiteToRemove:(LCEntity *)initSite windowForSheet:(NSWindow *)initWindowForSheet;
- (LCSiteEditController *) init;
- (void) dealloc;

#pragma mark "UI Actions"
- (IBAction) addClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;
- (void) closeSheet;

#pragma mark "UI Variable Accessors"
@property (retain) NSWindow *windowForSheet;

#pragma mark "Validation"
- (BOOL) validateInput;

#pragma mark "Device Removal Methods"
- (IBAction) removeSiteCancelClicked:(id)sender;

#pragma mark "XML Operation Methods"
- (LCXMLRequest *) performUpdate;
- (LCXMLRequest *) performRemove;

#pragma mark "Site Property Methods"
@property (copy) NSString *name;
@property (copy) NSString *desc;
@property (copy) NSString *addressLine1;
@property (copy) NSString *addressLine2;
@property (copy) NSString *addressLine3;
@property (copy) NSString *suburb;
@property (copy) NSString *state;
@property (copy) NSString *postcode;
@property (copy) NSString *country;
@property (assign) float longitude;
@property (assign) float latitude;

#pragma mark "Property Methods"
@property (retain) LCCustomer *customer;
@property (retain) LCSite *siteEntity;
- (void) updateLiveSite:(LCSite *)site;
- (NSMutableDictionary *) siteProperties;
@property (retain) NSString *statusString;
@property (retain) NSImage *statusIcon;
@property (assign) BOOL xmlOperationInProgress;

@end
