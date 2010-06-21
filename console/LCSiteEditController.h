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
@property (nonatomic,retain) NSWindow *windowForSheet;

#pragma mark "Validation"
- (BOOL) validateInput;

#pragma mark "Device Removal Methods"
- (IBAction) removeSiteCancelClicked:(id)sender;

#pragma mark "XML Operation Methods"
- (LCXMLRequest *) performUpdate;
- (LCXMLRequest *) performRemove;

#pragma mark "Site Property Methods"
@property (nonatomic, copy) NSString *name;
@property (nonatomic, copy) NSString *desc;
@property (nonatomic, copy) NSString *addressLine1;
@property (nonatomic, copy) NSString *addressLine2;
@property (nonatomic, copy) NSString *addressLine3;
@property (nonatomic, copy) NSString *suburb;
@property (nonatomic, copy) NSString *state;
@property (nonatomic, copy) NSString *postcode;
@property (nonatomic, copy) NSString *country;
@property (nonatomic, assign) float longitude;
@property (nonatomic, assign) float latitude;

#pragma mark "Property Methods"
@property (nonatomic, retain) LCCustomer *customer;
@property (nonatomic, retain) LCSite *siteEntity;
- (void) updateLiveSite:(LCSite *)site;
- (NSMutableDictionary *) siteProperties;
@property (nonatomic, retain) NSString *statusString;
@property (nonatomic, retain) NSImage *statusIcon;
@property (nonatomic, assign) BOOL xmlOperationInProgress;

@end
