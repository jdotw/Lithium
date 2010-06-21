//
//  LCDeviceEditController.h
//  Lithium Console
//
//  Created by James Wilson on 31/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCSite.h"
#import "LCDevice.h"
#import "LCVendor.h"
#import "LCOutlineView.h"
#import "LCVendorList.h"
#import "LCBonjourBrowserList.h"
#import "LCIPRegistryNetworkList.h"
#import "LCDeviceEditTemplate.h"

#define ADD_MODE_SINGLE 0
#define ADD_MODE_MULTIPLE 1

@interface LCDeviceEditController : NSWindowController 
{
	/* Related objects */
	LCDevice *deviceEntity;
	LCSite *siteEntity;
	NSArray *devices;
	
	/* Settings */
	int mode;					/* ADD_MODE_SINGLE / ADD_MODE_MULTIPLE */
	int mark;					/* Marking to set */
	
	/* Device Properties */
	LCDeviceEditTemplate *singleTemplate;
	NSArray *multipleTemplates;
	NSString *snmpCommunity;
	int snmpVersion;
	int snmpAuthMethod;
	int snmpPrivacyEncryption;
	NSString *snmpPrivPassword;
	NSString *snmpAuthPassword;
	NSString *username;
	NSString *password;
	NSString *confirmPassword;
	BOOL passwordsMatch;
	NSString *lomUsername;
	NSString *lomPassword;
	NSString *lomConfirmPassword;
	BOOL lomPasswordsMatch;
	NSString *profile;
	NSString *vendor;
	int refreshInterval;
	int osxProtocol;			/* 1=SNMP, 2=ServerMgrd */
	BOOL useIcmp;
	BOOL useLithiumSnmpExtensions;
	BOOL useNagios;
	BOOL useLom;
	BOOL isXsanClient;
	BOOL monitorProcessList;
	
	/* Vendor Module */
	IBOutlet NSPopUpButton *modulePopUpButton;
	IBOutlet NSMenu *moduleMenu;
	IBOutlet NSMenu *serverModuleMenu;
	IBOutlet NSMenu *storageModuleMenu;
	IBOutlet NSMenu *networkModuleMenu;
	IBOutlet NSMenu *genericModuleMenu;
	IBOutlet NSMenu *customModuleMenu;
	NSString *selectedModuleType;
	
	/* UI Variables */
	int credentialsSelectedTab;
	int snmpSelectedTab;
	int optionsSelectedTab;
	BOOL willUseSnmp;
	
	/* UI Elements */
	IBOutlet NSButton *addButton;
	IBOutlet NSWindow *removeProgressSheet;
	IBOutlet NSTabView *optionTabView;
	IBOutlet NSButton *useLomButton;
	IBOutlet NSWindow *moveProgressSheet;
	IBOutlet NSTextField *snmpCommunityTextField;
	IBOutlet NSTextField *snmpUsernameTextField;
	IBOutlet NSTextField *usernameTextField;
	IBOutlet NSTextField *raidPasswordTextField;
	IBOutlet NSView *csvOpenAccessoryView;
	
	/* Window for Sheet*/
	NSWindow *windowForSheet;
	
	/* XML Ops */
	BOOL testMode;
	BOOL xmlOperationInProgress;
	BOOL errorEncountered;
	LCXMLRequest *removeXMLReq;
	LCXMLRequest *moveXMLReq;
	LCXMLRequest *markXMLReq;
	NSMutableDictionary *templateXmlRequests;
	
	/* XML Parser */
	NSMutableDictionary *xmlProperties;
	NSString *xmlElement;
	NSMutableString *xmlString;
	
	/* Status */
	NSString *statusString;
	NSImage *statusIcon;
}

#pragma mark "Constructors"
- (LCDeviceEditController *) initForNewDevicesAtSite:(LCSite *)initSite usingTemplates:(NSArray *)initTemplates;
- (LCDeviceEditController *) initForNewDevicesAtSite:(LCSite *)initSite usingTemplates:(NSArray *)initTemplates usingModule:(NSString *)initVendorModule;
- (LCDeviceEditController *) initForImportFromCSVAtSite:(LCSite *)initSite windowForSheet:(NSWindow *)initWindow;
- (LCDeviceEditController *) initForImportFromARDAtSite:(LCSite *)initSite windowForSheet:(NSWindow *)initWindow;
- (LCDeviceEditController *) initForNewDeviceAtSite:(LCSite *)initSite;
- (LCDeviceEditController *) initForNewDeviceAtSite:(LCSite *)initSite usingModule:(NSString *)initVendorModule;
- (LCDeviceEditController *) initWithDeviceToEdit:(LCDevice *)initDevice;
- (LCDeviceEditController *) initWithDeviceToDuplicate:(LCDevice *)initDevice;
- (LCDeviceEditController *) initWithDeviceToRemove:(LCDevice *)initDevice windowForSheet:(NSWindow *)initWindowForSheet;
- (LCDeviceEditController *) initWithDevicesToMove:(NSArray *)initDevices toSite:(LCSite *)initSite windowForSheet:(NSWindow *)initWindowForSheet;
- (LCDeviceEditController *) initWithDevice:(LCDevice *)initDevice toMarkAs:(int)initMark;
- (LCDeviceEditController *) init;
- (void) dealloc;



#pragma mark "String Utilities"
- (NSString *) incrementNumberInString:(NSString *)originalString;

#pragma mark "General Accessors"
@property (nonatomic, retain) LCDevice *deviceEntity;
@property (nonatomic, copy) NSArray *devices;
@property (nonatomic, retain) LCSite *siteEntity;

#pragma mark "UI Variable Accessors"
@property (nonatomic, assign) int mode;
@property (nonatomic, assign) int mark;
@property (nonatomic, assign) int credentialsSelectedTab;
@property (nonatomic, assign) int snmpSelectedTab;
@property (nonatomic, assign) int optionsSelectedTab;
@property (nonatomic, assign) BOOL willUseSnmp;
@property (nonatomic, retain) NSWindow *windowForSheet;

#pragma mark "UI Actions"
- (IBAction) addClicked:(id)sender;
- (IBAction) testClicked:(id)sender;
- (void) performAddUpdateOrTest;
- (IBAction) cancelClicked:(id)sender;
- (IBAction) refreshVendorList:(id)sender;
- (IBAction) importSelectAllClicked:(id)sender;
- (IBAction) importSelectNoneClicked:(id)sender;
- (IBAction) importToggleAllClicked:(id)sender;

#pragma mark "Template Properties"
@property (nonatomic, retain) LCDeviceEditTemplate *singleTemplate;
@property (nonatomic, copy) NSArray *multipleTemplates;

#pragma mark "Device Property Methods"
@property (nonatomic, copy) NSString *snmpCommunity;
@property (nonatomic, assign) int snmpVersion;
@property (nonatomic, assign) int snmpAuthMethod;
@property (nonatomic, assign) int snmpPrivacyEncryption;
@property (nonatomic, copy) NSString *snmpPrivPassword;
@property (nonatomic, copy) NSString *snmpAuthPassword;
@property (nonatomic, copy) NSString *username;
@property (nonatomic, copy) NSString *password;
@property (nonatomic, copy) NSString *confirmPassword;
@property (nonatomic, assign) BOOL passwordsMatch;
@property (nonatomic, copy) NSString *lomUsername;
@property (nonatomic, copy) NSString *lomPassword;
@property (nonatomic, copy) NSString *lomConfirmPassword;
@property (nonatomic, assign) BOOL lomPasswordsMatch;
@property (nonatomic, copy) NSString *profile;
@property (nonatomic, copy) NSString *vendor;
@property (nonatomic, assign) int refreshInterval;
@property (nonatomic, assign) int osxProtocol;
@property (nonatomic, assign) BOOL useIcmp;
@property (nonatomic, assign) BOOL useLithiumSnmpExtensions;
@property (nonatomic, assign) BOOL useNagios;
@property (nonatomic, assign) BOOL useLom;
@property (nonatomic, assign) BOOL isXsanClient;
@property (nonatomic, assign) BOOL monitorProcessList;
- (NSMutableDictionary *) devicePropertiesUsingTemplate:(LCDeviceEditTemplate *)template;
- (void) updateLiveDeviceEntity:(LCDevice *)device usingTemplate:(LCDeviceEditTemplate *)template;

#pragma mark "Module Methods"
- (void) buildModuleMenuWithSelection:(NSString *)defaultSelection;
- (IBAction) moduleMenuClicked:(id)sender;
@property (nonatomic,copy) NSString *selectedModuleType;

#pragma mark "UI Actions"
- (IBAction) addClicked:(id)sender;
- (IBAction) testClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;
- (IBAction) refreshVendorList:(id)sender;

#pragma mark "Status String Accessors"
@property (nonatomic,copy) NSString *statusString;
@property (nonatomic,copy) NSImage *statusIcon;
@property (nonatomic, assign) BOOL xmlOperationInProgress;

#pragma mark "Validation"
- (BOOL) validateInput;

#pragma mark "Device Removal Methods"
- (void) removeDeviceSheetEnded:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo;
- (IBAction) removeDeviceCancelClicked:(id)sender;

#pragma mark "Device Move Methods"
- (void) moveDeviceSheetEnded:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo;
- (IBAction) moveDeviceCancelClicked:(id)sender;

#pragma mark "XML Operation Methods"
- (LCXMLRequest *) performUpdateUsingProperties:(NSMutableDictionary *)properties;
- (LCXMLRequest *) performRemove;
- (LCXMLRequest *) performMove;

@end
