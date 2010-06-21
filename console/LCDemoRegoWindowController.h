//
//  LCDemoRegoWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 2/12/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"
#import "LCXMLRequest.h"
#import "WebKit/WebView.h"

@interface LCDemoRegoWindowController : NSWindowController 
{
	/* Customer */
	LCCustomer *customer;
	
	/* User-Entered Fields */
	NSString *firstname;
	NSString *lastname;
	NSString *company;
	NSString *email;
	int licenseIndex;
	
	/* UI Elements */
	IBOutlet NSTabView *tabView;
	IBOutlet NSButton *cancelButton;
	IBOutlet NSButton *registerButton;
	IBOutlet NSTextField *firstnameTextField;
	IBOutlet NSTextField *lastnameTextField;
	IBOutlet NSTextField *companyTextField;
	IBOutlet NSTextField *emailTextField;
	IBOutlet NSTextField *usageTextField;
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet WebView *webView;
	
	/* URL Request -- Download key from Vince */
	LCActivity *activity;
	NSURLConnection *urlConn;
	NSMutableData *receivedData;
	NSString *curXMLElement;
	NSMutableString *curXMLString;
	NSString *licenseKey;
	
	/* XML Request -- To upload the key to core */
	LCXMLRequest *xmlReq;

	/* Flags */
	BOOL regoInProgress;
	NSString *regoMessage;
	NSString *progressString;
}

#pragma mark "Initialisation"
- (id) initForCustomer:(LCCustomer *)initCustomer;

#pragma mark "UI Actions"
- (IBAction) registerClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;
- (IBAction) privacyClicked:(id)sender;

#pragma mark "Send Registration"
- (void) sendRegistration;

#pragma mark "XML Parsing -- From Vince"
- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict;
- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string;
- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname;

#pragma mark "Upload to License Key -- To Core"
- (void) uploadLicenseKey;

#pragma mark "Accessors"
@property (nonatomic,retain) LCCustomer *customer;
@property (nonatomic,copy) NSString *firstname;
@property (nonatomic,copy) NSString *lastname;
@property (nonatomic,copy) NSString *company;
@property (nonatomic,copy) NSString *email;
@property (nonatomic, assign) int licenseIndex;
@property (nonatomic,copy) NSString *licenseKey;
@property (nonatomic, assign) BOOL regoInProgress;
@property (nonatomic,copy) NSString *regoMessage;
@property (nonatomic,copy) NSString *progressString;

@end
