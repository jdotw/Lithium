//
//  LCDeviceEditController.m
//  Lithium Console
//
//  Created by James Wilson on 31/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#import "LCName.h"
#import "LCDeviceEditController.h"
#import "LCDevice.h"
#import "LCContainer.h"

@implementation LCDeviceEditController

#pragma mark "Constructors - Multiple Device Add"

- (void) resizeSheetForMultipleAdd
{
	NSRect curFrame = [[self window] frame];
	curFrame.size.height = curFrame.size.height + 150.0;
	[[self window] setFrame:curFrame display:NO];
}

- (LCDeviceEditController *) initForNewDevicesAtSite:(LCSite *)initSite usingTemplates:(NSArray *)initTemplates
{
	/* Check for preferred module */
	NSString *preferredModule = nil;
	if (initTemplates.count > 0)
	{
		LCDeviceEditTemplate *template = [initTemplates objectAtIndex:0];
		preferredModule = template.preferredModule;
	}
	return [self initForNewDevicesAtSite:initSite usingTemplates:initTemplates usingModule:preferredModule];
}

- (LCDeviceEditController *) initForNewDevicesAtSite:(LCSite *)initSite usingTemplates:(NSArray *)initTemplates usingModule:(NSString *)initVendorModule
{
	self = [self initForNewDeviceAtSite:initSite usingModule:initVendorModule];
	if (!self) return nil;
	
	/* Use Templates */
	if (initTemplates.count <= 1)
	{
		/* Use single-device template */
		self.mode = ADD_MODE_SINGLE;
		if (initTemplates.count == 1)
		{ self.singleTemplate = [initTemplates objectAtIndex:0]; }
	}
	else
	{
		/* Use multiple-device template */
		self.mode = ADD_MODE_MULTIPLE;
		self.multipleTemplates = initTemplates;
	}
	
	/* Update first responder */
	if (self.credentialsSelectedTab == 0)
	{
		/* SNMP */
		if (self.snmpVersion == 1)
		{ [[self window] makeFirstResponder:snmpCommunityTextField]; }
		else
		{ [[self window] makeFirstResponder:snmpUsernameTextField]; }
	}
	else if (self.credentialsSelectedTab == 1)
	{
		/* Username/Password */
		[[self window] makeFirstResponder:usernameTextField];
	}
	else if (self.credentialsSelectedTab == 2)
	{
		/* Xserve RAID (Password) */
		[[self window] makeFirstResponder:raidPasswordTextField];
	}
	
	[self resizeSheetForMultipleAdd];
	
	return self;
}

#pragma mark "Constructors - Import"

- (LCDeviceEditController *) initForImportFromCSVAtSite:(LCSite *)initSite windowForSheet:(NSWindow *)initWindow
{
	[self init];
	
	/* Setup Controller */
	self.siteEntity = initSite;
	[addButton setTitle:@"Add"];
	[self buildModuleMenuWithSelection:[[NSUserDefaults standardUserDefaults] stringForKey:@"LCDeviceEditControllerLastUsedModule"]];
	self.windowForSheet = initWindow;
	
	/* Display File Open Sheet */
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	[panel setAccessoryView:csvOpenAccessoryView];
	[panel beginSheetForDirectory:nil
							 file:nil
				   modalForWindow:windowForSheet
					modalDelegate:self
				   didEndSelector:@selector(csvOpenPanelDidEnd:returnCode:contextInfo:)
					  contextInfo:nil];
	
	return self;
}

- (LCDeviceEditController *) initForImportFromARDAtSite:(LCSite *)initSite windowForSheet:(NSWindow *)initWindow
{
	/* Try opening plist */
	NSData *data = [NSData dataWithContentsOfFile:[@"~/Library/Preferences/com.apple.RemoteDesktop.plist" stringByExpandingTildeInPath]];
	NSDictionary *ardProperties = (NSDictionary *)[NSPropertyListSerialization
													 propertyListFromData:data
													 mutabilityOption:0
													 format:nil
													 errorDescription:nil];	
	
	/* Create templates */
	NSMutableArray *templates = [NSMutableArray array];
	for (NSDictionary *machineDict in [ardProperties objectForKey:@"ComputerDatabase"])
	{
		LCDeviceEditTemplate *template = [[LCDeviceEditTemplate new] autorelease];
		template.desc = [machineDict objectForKey:@"name"];
		template.ipAddress = [machineDict objectForKey:@"networkAddress"];
		[templates addObject:template];
	}

	return [self initForNewDevicesAtSite:initSite usingTemplates:templates usingModule:@"osx_server"];
}

#pragma mark "Constructors - Single Device Add"

- (LCDeviceEditController *) initForNewDeviceAtSite:(LCSite *)initSite
{
	[self init];

	[self setSiteEntity:initSite];
	[addButton setTitle:@"Add"];

	[self buildModuleMenuWithSelection:[[NSUserDefaults standardUserDefaults] stringForKey:@"LCDeviceEditControllerLastUsedModule"]];
	
	return self;
}

- (LCDeviceEditController *) initForNewDeviceAtSite:(LCSite *)initSite usingModule:(NSString *)initVendorModule
{
	[self initForNewDeviceAtSite:initSite];

	if (initVendorModule)
	{ [self buildModuleMenuWithSelection:initVendorModule]; }
	
	return self;
}

#pragma mark "Constructor - Single Device Edit"

- (LCDeviceEditController *) initWithDeviceToEdit:(LCDevice *)initDevice
{
	[self init];
	
	[self setDeviceEntity:initDevice];
	[self setSiteEntity:[initDevice site]];
	[self setConfirmPassword:[self password]];
	[self setLomConfirmPassword:[self lomPassword]];
	if (refreshInterval != 30 && refreshInterval != 60 && refreshInterval != 120 
		&& refreshInterval != 300 && refreshInterval != 600)
	{ self.refreshInterval = 300; }
	[addButton setTitle:@"Save"];

	[self buildModuleMenuWithSelection:[initDevice vendor]];
	
	return self;
}

#pragma mark "Constructor - Single Device Duplicate"

- (LCDeviceEditController *) initWithDeviceToDuplicate:(LCDevice *)initDevice
{
	[self init];
	
	self.singleTemplate.desc = initDevice.desc;
	self.singleTemplate.ipAddress = initDevice.ipAddress;
	self.singleTemplate.lomIpAddress = initDevice.lomIpAddress;
	
	[initDevice copyXmlPropertiesToObject:self];
	[self setSiteEntity:[initDevice site]];
	[self setPassword:nil];
	[self setConfirmPassword:[self password]];
	[self setLomPassword:nil];
	[self setLomConfirmPassword:[self lomPassword]];
	if (refreshInterval != 30 && refreshInterval != 60 && refreshInterval != 120 
		&& refreshInterval != 300 && refreshInterval != 600)
	{ self.refreshInterval = 300; }
	[addButton setTitle:@"Add"];
	
	/*
	 * Perform duplication of properties 
	 */
	
	/* Increment numbers in name/desc */
	if (self.singleTemplate.desc) self.singleTemplate.desc = [self incrementNumberInString:self.singleTemplate.desc];
	
	/* Increment IP */
	NSString *ip = self.singleTemplate.ipAddress;
	NSArray *hostComponents = [ip componentsSeparatedByString:@"."];
	struct in_addr addr;
	if (inet_aton([ip cStringUsingEncoding:NSUTF8StringEncoding], &addr) == 1 && [hostComponents count] == 4)
	{
		/* IP Address */
		NSString *lastOctet = [self incrementNumberInString:[hostComponents objectAtIndex:3]];
		self.singleTemplate.ipAddress = [NSString stringWithFormat:@"%@.%@.%@.%@",
										 [hostComponents objectAtIndex:0], [hostComponents objectAtIndex:1],
										 [hostComponents objectAtIndex:2], lastOctet];
	}
	else if ([hostComponents count] > 0)
	{
		/* Hostname */
		NSString *originalHostname = [hostComponents objectAtIndex:0];
		NSString *hostName = [self incrementNumberInString:[hostComponents objectAtIndex:0]];
		NSString *replacementIP = nil;
		if ([hostComponents count] > 1)
		{
			/* Multi-part hostname */
			NSMutableString *replacement = [NSMutableString stringWithString:ip];
			[replacement replaceCharactersInRange:NSMakeRange(0,[originalHostname length]) 
									   withString:hostName];
			replacementIP = replacement;
		}
		else
		{
			/* Single-part */
			replacementIP = hostName;
		}
		if (replacementIP)
		{ self.singleTemplate.ipAddress = replacementIP; }
	}

	[self buildModuleMenuWithSelection:[initDevice vendor]];
	
	return self;
}

#pragma mark "Constructor Single Device Remove"

- (LCDeviceEditController *) initWithDeviceToRemove:(LCDevice *)initDevice windowForSheet:(NSWindow *)initWindowForSheet
{
	[self init];
	
	self.siteEntity = initDevice.site;
	self.deviceEntity = initDevice;
	self.windowForSheet = initWindowForSheet;
	
	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm Device Delete"
									 defaultButton:@"Delete" 
								   alternateButton:@"Cancel" 
									   otherButton:nil 
						 informativeTextWithFormat:@"This action can not be undone."];
	[alert beginSheetModalForWindow:initWindowForSheet
					  modalDelegate:self
					 didEndSelector:@selector(removeDeviceSheetEnded:returnCode:contextInfo:)
						contextInfo:nil];

	return self;
}

#pragma mark "Constructor for Device Move"

- (LCDeviceEditController *) initWithDevicesToMove:(NSArray *)initDevices toSite:(LCSite *)initSite windowForSheet:(NSWindow *)initWindowForSheet
{
	[self init];
	
	self.siteEntity = initSite;
	self.devices = initDevices;
	self.windowForSheet = initWindowForSheet;
	
	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm Device Move"
									 defaultButton:@"Move" 
								   alternateButton:@"Cancel" 
									   otherButton:nil 
						 informativeTextWithFormat:@"Moving a device will cause the monitoring process for that device to be restarted. Monitoring data will be temporarily unavailable while monitoring restarts."];
	[alert beginSheetModalForWindow:initWindowForSheet
					  modalDelegate:self
					 didEndSelector:@selector(moveDeviceSheetEnded:returnCode:contextInfo:)
						contextInfo:nil];
	
	return self;
}

#pragma mark "Constructor for Device Marking"

- (LCDeviceEditController *) initWithDevice:(LCDevice *)initDevice toMarkAs:(int)initMark
{
	[self init];
	
	self.siteEntity = initDevice.site;
	self.deviceEntity = initDevice;
	self.mark = initMark;

	/* Mark the device */
	
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"device"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"name" stringValue:[deviceEntity name]]];
	[rootnode addChild:[NSXMLNode elementWithName:@"site_name" stringValue:[siteEntity name]]];
	[rootnode addChild:[NSXMLNode elementWithName:@"mark" stringValue:[NSString stringWithFormat:@"%i", mark]]];
	
	/* Create and perform request */
	markXMLReq = [[LCXMLRequest requestWithCriteria:[siteEntity customer]
										   resource:[[siteEntity customer] resourceAddress]
											 entity:[[siteEntity customer] entityAddress]
											xmlname:@"device_mark"
											 refsec:0
											 xmlout:xmldoc] retain];		
	[markXMLReq setDelegate:self];
	[markXMLReq setPriority:XMLREQ_PRIO_HIGH];
	[markXMLReq performAsyncRequest];
	
	[self setXmlOperationInProgress:YES];
	
	return self;	
}

#pragma mark "Constructors"

- (LCDeviceEditController *) init
{
	/* Super-class init */
	[super initWithWindowNibName:@"DeviceEditWindow"];
	
	/* Create single-add template */
	LCDeviceEditTemplate *template = [LCDeviceEditTemplate new];	
	self.singleTemplate = template;
	[template autorelease];
	
	/* Create properties */
	templateXmlRequests = [[NSMutableDictionary dictionary] retain];

	/* Set defaults */
	[self setSnmpVersion:1];
	[self setRefreshInterval:60];
	
	/* Load NIB */
	[self window];
	
	return self;
}

- (void) dealloc
{
	if (siteEntity)
	{ [[NSNotificationCenter defaultCenter] removeObserver:self name:@"LCVendorListRefreshFinished" object:[[siteEntity customer] vendorList]]; }
	[deviceEntity release];
	[siteEntity release];
	[singleTemplate release];
	[multipleTemplates release];
	[templateXmlRequests release];
	[super dealloc];
}

#pragma mark "String Utilities"

- (NSString *) incrementNumberInString:(NSString *)originalString
{
	/* Increment right-most number in name */
	const char *str = [originalString cStringUsingEncoding:NSUTF8StringEncoding];
	size_t len = strlen(str);
	int i;
	BOOL numHit = NO;
	int numEnd = len-1;
	int numStart = 0;
	for (i=len-1; i >= 0; i--)
	{
		/* Move backward through string */
		if (!numHit && isdigit(str[i]))
		{
			/* Hit our first num */
			numEnd = i;
			numHit = YES;
		}
		else if (numHit && !isdigit(str[i]))
		{
			/* End of our number hit */
			numStart = i+1;
			break;
		}					
	}
	if (numHit)
	{
		/* Number was found between numStart and numEnd */
		char *num_str = malloc ((numEnd - numStart)+2);
		memset (num_str, 0, ((numEnd - numStart)+2));
		strncpy (num_str, str + numStart, (numEnd - numStart)+1);
		
		/* Increment */
		int num = atoi (num_str);
		num++;
		
		/* Replace in string */
		NSString *incremenetedNumber = [NSString stringWithFormat:@"%i", num];
		NSMutableString *paddedNumber = [NSMutableString string];
		if (strlen(num_str) > [incremenetedNumber length])
		{
			int i;
			int paddingLength = strlen(num_str) - [incremenetedNumber length];
			for (i=0; i < paddingLength; i++)
			{ [paddedNumber appendString:@"0"]; }
		}
		[paddedNumber appendString:incremenetedNumber];
		NSMutableString *replacement = [NSMutableString stringWithString:originalString];
		[replacement replaceCharactersInRange:NSMakeRange(numStart, (numEnd - numStart)+1) 
								   withString:paddedNumber];
		
		/* Set it */
		return replacement;
	}
	else
	{
		/* No num, append "2" */
		return [NSString stringWithFormat:@"%@2", originalString];
	}
}

#pragma mark "KVO and Notification Methods"

- (void) vendorListRefreshFinished:(NSNotification *)note
{
	/* Rebuild menu with same selection */
	[self buildModuleMenuWithSelection:[deviceEntity vendor]];
}

#pragma mark "General Accessors"

@synthesize deviceEntity;
- (void) setDeviceEntity:(LCDevice *)value
{
	/* Set value */
	[deviceEntity release];
	deviceEntity = value;
	
	/* Update template and local variables */
	singleTemplate.name = deviceEntity.name;
	singleTemplate.desc = deviceEntity.desc;
	singleTemplate.ipAddress = deviceEntity.ipAddress;
	singleTemplate.lomIpAddress = deviceEntity.lomIpAddress;
	[deviceEntity copyXmlPropertiesToObject:self];
}
@synthesize devices;

@synthesize siteEntity;
- (void) setSiteEntity:(LCSite *)value
{
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"LCVendorListRefreshFinished" object:[[siteEntity customer] vendorList]];
	
	[siteEntity release];
	siteEntity = value;
	
	[[[siteEntity customer] vendorList] refreshWithPriority:XMLREQ_PRIO_HIGH];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(vendorListRefreshFinished:)
												 name:@"LCVendorListRefreshFinished" 
											   object:[[siteEntity customer] vendorList]];
}

#pragma mark "UI Variable Accessors"

@synthesize mode;
@synthesize mark;
@synthesize credentialsSelectedTab;
@synthesize snmpSelectedTab;
@synthesize optionsSelectedTab;
@synthesize windowForSheet;
@synthesize willUseSnmp;

#pragma mark "Device Property Methods"

@synthesize singleTemplate;
@synthesize multipleTemplates;
@synthesize snmpCommunity;
@synthesize snmpVersion;
- (void) setSnmpVersion:(int)value
{
	snmpVersion = value;
	if (value == 1 || value == 2)
	{ self.snmpSelectedTab = 0; }
	else
	{ self.snmpSelectedTab = 1; }
}
@synthesize snmpAuthMethod;
@synthesize snmpPrivacyEncryption;
@synthesize snmpPrivPassword;
@synthesize snmpAuthPassword;

@synthesize username;
@synthesize password;
- (void) setPassword:(NSString *)string
{ 
	[password release];
	password = [string copy];
	if ([password isEqualToString:confirmPassword]) self.passwordsMatch = YES;
	else self.passwordsMatch = NO;	
}
@synthesize confirmPassword;
- (void) setConfirmPassword:(NSString *)string
{
	[confirmPassword release];
	confirmPassword = [string copy];
	if ([confirmPassword isEqualToString:password]) self.passwordsMatch = YES;
	else self.passwordsMatch = NO;	
}
@synthesize passwordsMatch;

@synthesize lomUsername;
@synthesize lomPassword;
- (void) setLomPassword:(NSString *)string
{ 
	[lomPassword release];
	lomPassword = [string copy];
	if ([lomPassword isEqualToString:lomConfirmPassword]) self.lomPasswordsMatch = YES;
	else self.lomPasswordsMatch = NO;	
}
@synthesize lomConfirmPassword;
- (void) setLomConfirmPassword:(NSString *)string
{
	[lomConfirmPassword release];
	lomConfirmPassword = [string copy];
	if ([lomConfirmPassword isEqualToString:lomPassword]) self.lomPasswordsMatch = YES;
	else self.lomPasswordsMatch = NO;	
}
@synthesize lomPasswordsMatch;

@synthesize profile;
@synthesize vendor;

@synthesize refreshInterval;
@synthesize osxProtocol;
- (void) setOsxProtocol:(int)value
{
	osxProtocol = value;
	if (osxProtocol == 0)
	{
		/* SNMP */
		self.willUseSnmp = YES;
		self.credentialsSelectedTab = 0;
	}
	else
	{
		/* Servermgrd */
		self.willUseSnmp = NO;		
		self.credentialsSelectedTab = 1;
	}
	
	[[NSUserDefaults standardUserDefaults] setInteger:osxProtocol forKey:@"LCDeviceEditControllerOSXClientProtocol"];
	[[NSUserDefaults standardUserDefaults] setInteger:osxProtocol forKey:@"LCDeviceEditControllerOSXServerProtocol"];
	[[NSUserDefaults standardUserDefaults] synchronize];
}
@synthesize useIcmp;
@synthesize useLithiumSnmpExtensions;
@synthesize monitorProcessList;
@synthesize useNagios;
@synthesize useLom;
@synthesize isXsanClient;

- (NSMutableDictionary *) devicePropertiesUsingTemplate:(LCDeviceEditTemplate *)template
{
	/* Combines all the properties that may be required
	 * for a add/test/edit/remove operation into 
	 * a NSMutableDictionary
	 */
	
	LCDevice *device;
	
	if (deviceEntity)
	{
		device = deviceEntity;
	}
	else
	{
		device = [[[LCDevice alloc] init] autorelease];
		device.name = template.name;
		device.desc = template.desc;
		device.ipAddress = template.ipAddress;
		device.lomIpAddress = template.lomIpAddress;
		
		[device copyXmlPropertiesFromObject:self];
	}
	
	NSMutableDictionary *deviceProperties = [NSMutableDictionary dictionaryWithDictionary:[device xmlPropertiesDictionary]];
	if (siteEntity.name)
	{ [deviceProperties setObject:siteEntity.name forKey:@"site_name"]; }
	
	return deviceProperties;
}

- (void) updateLiveDeviceEntity:(LCDevice *)device usingTemplate:(LCDeviceEditTemplate *)template
{
	/* Updated the live (real) LCDevice entity with
	 * the the update controllers local values.
	 * 
	 * This is done before an update/submit operation
	 */
	
	/* Handle template properties */
	device.desc = template.desc;
	device.ipAddress = template.ipAddress;
	device.lomIpAddress = template.lomIpAddress;
	
	/* Handle everything else */
	[device copyXmlPropertiesFromObject:self];
}

- (void) setValue:(id)value forUndefinedKey:(NSString *)key
{
	
}

#pragma mark "Module Methods"

- (void) buildModuleMenuWithSelection:(NSString *)defaultSelection
{
	NSMenuItem *item;
	
	/* Clear Existing */
	while ([[serverModuleMenu itemArray] count] > 0)
	{ [serverModuleMenu removeItemAtIndex:0]; }
	while ([[storageModuleMenu itemArray] count] > 0)
	{ [storageModuleMenu removeItemAtIndex:0]; }
	while ([[networkModuleMenu itemArray] count] > 0)
	{ [networkModuleMenu removeItemAtIndex:0]; }
	while ([[genericModuleMenu itemArray] count] > 0)
	{ [genericModuleMenu removeItemAtIndex:0]; }
	while ([[customModuleMenu itemArray] count] > 0)
	{ [customModuleMenu removeItemAtIndex:0]; }

	/* 
	 * Built-Ins 
	 */
	
	/* Server */

	if (![[siteEntity customer] limitedLicense])
	{
		item = [[NSMenuItem alloc] initWithTitle:@"Xserve Intel" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
		[item setRepresentedObject:@"xserve_intel"];
		[serverModuleMenu insertItem:item atIndex:[[serverModuleMenu itemArray] count]];
		if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
		[item autorelease];

		item = [[NSMenuItem alloc] initWithTitle:@"Xserve G5" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
		[item setRepresentedObject:@"xserve_g5"];
		[serverModuleMenu insertItem:item atIndex:[[serverModuleMenu itemArray] count]];
		if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
		[item autorelease];
	}

	item = [[NSMenuItem alloc] initWithTitle:@"Non-Xserve Mac OS X Server" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"osx_server"];
	[serverModuleMenu insertItem:item atIndex:[[serverModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	item = [[NSMenuItem alloc] initWithTitle:@"Mac OS X Client" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"osx_client"];
	[serverModuleMenu insertItem:item atIndex:[[serverModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	[serverModuleMenu insertItem:[NSMenuItem separatorItem] atIndex:[[serverModuleMenu itemArray] count]];

	item = [[NSMenuItem alloc] initWithTitle:@"Linux/UNIX with Net-SNMP Daemon" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"netsnmp"];
	[serverModuleMenu insertItem:item atIndex:[[serverModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	[serverModuleMenu insertItem:[NSMenuItem separatorItem] atIndex:[[serverModuleMenu itemArray] count]];

	item = [[NSMenuItem alloc] initWithTitle:@"Windows" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"windows"];
	[serverModuleMenu insertItem:item atIndex:[[serverModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	/* Storage */
	item = [[NSMenuItem alloc] initWithTitle:@"Promse VTrak" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"vtrak"];
	[storageModuleMenu insertItem:item atIndex:[[storageModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	item = [[NSMenuItem alloc] initWithTitle:@"Xserve RAID" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"xraid"];
	[storageModuleMenu insertItem:item atIndex:[[storageModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];
	
	item = [[NSMenuItem alloc] initWithTitle:@"Active Storage XRAID" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"activestorage"];
	[storageModuleMenu insertItem:item atIndex:[[storageModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];
	

	[storageModuleMenu insertItem:[NSMenuItem separatorItem] atIndex:[[storageModuleMenu itemArray] count]];

	item = [[NSMenuItem alloc] initWithTitle:@"InforTrend Controller" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"infortrend"];
	[storageModuleMenu insertItem:item atIndex:[[storageModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	item = [[NSMenuItem alloc] initWithTitle:@"NetApp Filer" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"filer"];
	[storageModuleMenu insertItem:item atIndex:[[storageModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	[storageModuleMenu insertItem:[NSMenuItem separatorItem] atIndex:[[storageModuleMenu itemArray] count]];
	
	item = [[NSMenuItem alloc] initWithTitle:@"Qlogic Fibre Channel Switch" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"qlogic"];
	[storageModuleMenu insertItem:item atIndex:[[storageModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];
	
	/* Network */

	item = [[NSMenuItem alloc] initWithTitle:@"3Com Switch" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"3com"];
	[networkModuleMenu insertItem:item atIndex:[[networkModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	item = [[NSMenuItem alloc] initWithTitle:@"Allied Telesyn Switch" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"alliedtelesyn"];
	[networkModuleMenu insertItem:item atIndex:[[networkModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	item = [[NSMenuItem alloc] initWithTitle:@"Cisco Router/Switch/Firewall" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"cisco"];
	[networkModuleMenu insertItem:item atIndex:[[networkModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	item = [[NSMenuItem alloc] initWithTitle:@"Foundry Load Balancer" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"foundry"];
	[networkModuleMenu insertItem:item atIndex:[[networkModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	item = [[NSMenuItem alloc] initWithTitle:@"HP Switch" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"hp"];
	[networkModuleMenu insertItem:item atIndex:[[networkModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	[networkModuleMenu insertItem:[NSMenuItem separatorItem] atIndex:[[networkModuleMenu itemArray] count]];
	item = [[NSMenuItem alloc] initWithTitle:@"Airport Access Point" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"airport"];
	[networkModuleMenu insertItem:item atIndex:[[networkModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	[networkModuleMenu insertItem:[NSMenuItem separatorItem] atIndex:[[networkModuleMenu itemArray] count]];

	item = [[NSMenuItem alloc] initWithTitle:@"APC UPS" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"apcups"];
	[networkModuleMenu insertItem:item atIndex:[[networkModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	item = [[NSMenuItem alloc] initWithTitle:@"MGE UPS" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"mgeups"];
	[networkModuleMenu insertItem:item atIndex:[[networkModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	item = [[NSMenuItem alloc] initWithTitle:@"NetBotz Environment Monitor" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"netbotz"];
	[networkModuleMenu insertItem:item atIndex:[[networkModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	[networkModuleMenu insertItem:[NSMenuItem separatorItem] atIndex:[[networkModuleMenu itemArray] count]];
	
	item = [[NSMenuItem alloc] initWithTitle:@"Liebert Airconditioner" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"liebert"];
	[networkModuleMenu insertItem:item atIndex:[[networkModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	/* Generic */
	item = [[NSMenuItem alloc] initWithTitle:@"Basic SNMP" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"snmpbasic"];
	[genericModuleMenu insertItem:item atIndex:[[genericModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	item = [[NSMenuItem alloc] initWithTitle:@"Extended SNMP" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"snmpadv"];
	[genericModuleMenu insertItem:item atIndex:[[genericModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	[genericModuleMenu insertItem:[NSMenuItem separatorItem] atIndex:[[genericModuleMenu itemArray] count]];

	item = [[NSMenuItem alloc] initWithTitle:@"Printer" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"printer"];
	[genericModuleMenu insertItem:item atIndex:[[genericModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];

	[genericModuleMenu insertItem:[NSMenuItem separatorItem] atIndex:[[genericModuleMenu itemArray] count]];
	
	item = [[NSMenuItem alloc] initWithTitle:@"ICMP Ping Only" action:@selector(moduleMenuClicked:) keyEquivalent:@""];
	[item setRepresentedObject:@"icmp"];
	[genericModuleMenu insertItem:item atIndex:[[genericModuleMenu itemArray] count]];
	if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
	[item autorelease];	
	
	/* Custom */
	for (LCVendor *module in [[siteEntity.customer vendorList] vendors])
	{
		if ([[module name] hasSuffix:@"xml"])
		{
			item = [[NSMenuItem alloc] initWithTitle:[module desc] action:@selector(moduleMenuClicked:) keyEquivalent:@""];
			[item setRepresentedObject:[module name]];
			[customModuleMenu insertItem:item atIndex:[[customModuleMenu itemArray] count]];
			if ([[item representedObject] isEqualToString:defaultSelection]) [self moduleMenuClicked:item];
			[item autorelease];
		}
	}
}

- (IBAction) moduleMenuClicked:(id)sender
{
	/* Get Old Selection */
	NSMenuItem *oldSelectionItem = nil;
	if ([[[modulePopUpButton lastItem] title] length] > 0)
	{ oldSelectionItem = [modulePopUpButton lastItem]; }

	/* Set selection */
	NSMenuItem *clickedItem = sender;
	[modulePopUpButton setTitle:[clickedItem title]];
	self.selectedModuleType = [clickedItem representedObject];
	
	/* Clear old selection */
	if (oldSelectionItem && ![[oldSelectionItem title] isEqualToString:[clickedItem title]]) 
	{ [modulePopUpButton removeItemAtIndex:[modulePopUpButton indexOfItem:oldSelectionItem]]; }	
	
	/* Setup Vendor/Credentials/Options */
	NSString *moduleType = [clickedItem representedObject];
	self.vendor = moduleType;
	if ([moduleType hasPrefix:@"xserve"] || [moduleType isEqualToString:@"osx_server"])
	{
		/* OS X Server */
		if (!self.deviceEntity) self.osxProtocol = [[NSUserDefaults standardUserDefaults] integerForKey:@"LCDeviceEditControllerOSXServerProtocol"];
		if ([moduleType isEqualToString:@"xserve_intel"])
		{ 
			[useLomButton setHidden:NO]; 
			self.credentialsSelectedTab = self.osxProtocol;
		}
		else
		{ 
			[useLomButton setHidden:YES]; 
			self.credentialsSelectedTab = self.osxProtocol;
		}
		self.optionsSelectedTab = 0;
	}
	else if ([moduleType isEqualToString:@"osx_client"])
	{
		/* OS X Client */
		self.optionsSelectedTab = 1;
		if (!self.deviceEntity) self.osxProtocol = [[NSUserDefaults standardUserDefaults] integerForKey:@"LCDeviceEditControllerOSXClientProtocol"];
		self.credentialsSelectedTab = self.osxProtocol;
	}		
	else if ([moduleType isEqualToString:@"windows"] && [moduleType isEqualToString:@"netsnmp"])
	{
		/* SNMP Server */
		self.credentialsSelectedTab = 0;
		self.optionsSelectedTab = 2;
	}
	else if ([moduleType isEqualToString:@"xraid"])
	{
		/* Xserve RAID */
		self.credentialsSelectedTab = 2;
		self.optionsSelectedTab = 3;
	}
	else if ([moduleType isEqualToString:@"icmp"])
	{
		/* ICMP */
		self.credentialsSelectedTab = 3;
		self.optionsSelectedTab = 4;
	}
	else
	{
		/* Other SNMP */
		self.credentialsSelectedTab = 0;
		self.optionsSelectedTab = 3;
	}
	
	[[NSUserDefaults standardUserDefaults] setValue:moduleType forKey:@"LCDeviceEditControllerLastUsedModule"];
	[[NSUserDefaults standardUserDefaults] synchronize];
}

@synthesize selectedModuleType;

#pragma mark "UI Actions"

- (IBAction) addClicked:(id)sender
{
	/* Validate */
	if (![self validateInput])
	{ return; }
	
	/* Set status */
	[self setStatusIcon:nil];
	
	/* Check if we're updating an existing devie */
	if (deviceEntity)
	{
		/* Update the live device */
		[self updateLiveDeviceEntity:deviceEntity usingTemplate:singleTemplate];
	}

	/* Perform */
	testMode = NO;
	[self performAddUpdateOrTest];	
}

- (IBAction) testClicked:(id)sender
{
	/* Validate */
	if (![self validateInput])
	{ return; }

	/* Set status */
	[self setStatusIcon:nil];
	
	/* Perform */
	testMode = YES;
	[self performAddUpdateOrTest];
}

- (void) performAddUpdateOrTest
{
	/* Reset error flag */	
	errorEncountered = NO;
	
	/* Clear old dict */
	[templateXmlRequests removeAllObjects];
	
	/* Add device(s) */
	if (self.mode == ADD_MODE_SINGLE)
	{
		/* Single-add */
		self.singleTemplate.xmlReq = [self performUpdateUsingProperties:[self devicePropertiesUsingTemplate:singleTemplate]];
		self.singleTemplate.xmlOperationInProgress = YES;
		self.xmlOperationInProgress = YES;
		[templateXmlRequests setObject:self.singleTemplate forKey:[self.singleTemplate.xmlReq description]];
	}
	else if (self.mode == ADD_MODE_MULTIPLE)
	{
		/* Multiple-add */
		for (LCDeviceEditTemplate *template in multipleTemplates)
		{
			/* Perform add/update/test using template */
			if (template.enabled)
			{
				template.xmlReq = [self performUpdateUsingProperties:[self devicePropertiesUsingTemplate:template]];
				template.xmlOperationInProgress = YES;
				[templateXmlRequests setObject:template forKey:[template.xmlReq description]];
			}
		}
		if ([templateXmlRequests count] > 0)
		{ self.xmlOperationInProgress = YES; }
	}

	/* Update status string */
	if (self.xmlOperationInProgress)
	{
		if (testMode)
		{ 
			self.statusString = @"Testing device settings...";
		}
		else
		{
			if (deviceEntity) self.statusString = @"Updating device...";
			else self.statusString = @"Adding device...";
		}
	}
}

- (IBAction) cancelClicked:(id)sender
{
	[NSApp endSheet:[self window]];
	[[self window] close];
}

- (IBAction) refreshVendorList:(id)sender
{
	[[[siteEntity customer] vendorList] refreshWithPriority:XMLREQ_PRIO_HIGH];
}

- (IBAction) importSelectAllClicked:(id)sender
{
	for (LCDeviceEditTemplate *template in multipleTemplates)
	{ template.enabled = YES; }
}

- (IBAction) importSelectNoneClicked:(id)sender
{
	for (LCDeviceEditTemplate *template in multipleTemplates)
	{ template.enabled = NO; }	
}

- (IBAction) importToggleAllClicked:(id)sender
{
	for (LCDeviceEditTemplate *template in multipleTemplates)
	{ template.enabled = template.enabled ? NO : YES; }		
}

#pragma mark "Status Accessors"

@synthesize statusString;
@synthesize statusIcon;
@synthesize xmlOperationInProgress;

#pragma mark "Validation"

- (BOOL) validateInput
{
	/* 
	 * Add-Method dependent checking 
	 */
	
	if (self.mode == ADD_MODE_SINGLE)
	{
		/* Single-add */
		if ([self.singleTemplate.desc length] < 1)
		{ 
			[self setStatusIcon:[NSImage imageNamed:@"stop_16.tif"]];
			[self setStatusString:@"Error: A description is required."]; 
			return NO; 
		}
		if ([self.singleTemplate.name length] < 1)
		{ 
			[self setStatusIcon:[NSImage imageNamed:@"stop_16.tif"]];
			[self setStatusString:@"Error: A device name is required."]; 
			return NO; 
		}
		if ([self.singleTemplate.ipAddress length] < 1)
		{ 
			[self setStatusIcon:[NSImage imageNamed:@"stop_16.tif"]];
			[self setStatusString:@"Error: An IP address is required."]; 
			return NO; 
		}
	}
	else if (self.mode == ADD_MODE_MULTIPLE)
	{
		/* DEBUG FIX Must add validation */
	}
	
	/*
	 * Credential dependent checking
	 */
	if (credentialsSelectedTab == 1 || credentialsSelectedTab == 2)
	{
		/* Username/Pass */
		if (![[self password] isEqualToString:[self confirmPassword]])
		{ 
			[self setStatusIcon:[NSImage imageNamed:@"stop_16.tif"]];
			if (credentialsSelectedTab == 3)
			{ [self setStatusString:@"Error: Admin Passwords do not match."];  }
			else
			{ [self setStatusString:@"Error: Passwords do not match."];  }				
			return NO; 
		}
		
		/* LOM User/Pass */
		if (credentialsSelectedTab == 3)
		{
			if (![[self lomPassword] isEqualToString:[self lomConfirmPassword]])
			{ 
				[self setStatusIcon:[NSImage imageNamed:@"stop_16.tif"]];
				[self setStatusString:@"Error: LOM Passwords do not match."]; 
				return NO; 
			}			
		}
	}
	
	return YES;
}

#pragma mark "Device Removal Methods"

- (void) removeDeviceSheetEnded:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		/* Remove device */
		[[alert window] orderOut:self];
		[NSApp beginSheet:removeProgressSheet
		   modalForWindow:windowForSheet 
			modalDelegate:self 
		   didEndSelector:nil 
			  contextInfo:nil];
		[self performRemove];
	}
}

- (IBAction) removeDeviceCancelClicked:(id)sender
{
	[NSApp endSheet:removeProgressSheet];
	[removeProgressSheet close];
	[removeXMLReq cancel];
	[removeXMLReq release];
}

#pragma mark "Device Move Methods"

- (void) moveDeviceSheetEnded:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		/* Remove device */
		[[alert window] orderOut:self];
		[NSApp beginSheet:moveProgressSheet
		   modalForWindow:windowForSheet 
			modalDelegate:self 
		   didEndSelector:nil 
			  contextInfo:nil];
		[self performMove];
	}
}

- (IBAction) moveDeviceCancelClicked:(id)sender
{
	[NSApp endSheet:moveProgressSheet];
	[moveProgressSheet close];
	[moveXMLReq cancel];
	[moveXMLReq release];
}

#pragma mark "Import Methods"

- (void) csvOpenPanelDidEnd:(NSOpenPanel *)panel returnCode:(int)returnCode  contextInfo:(void  *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		/* Open file */
		NSString *string = [NSString stringWithContentsOfFile:[panel filename] encoding:NSUTF8StringEncoding error:nil];
		
		/* Parse lines */
		unsigned length = [string length];
		NSUInteger paraStart = 0, paraEnd = 0, contentsEnd = 0;
		NSMutableArray *lines = [NSMutableArray array];
		NSRange currentRange;
		while (paraEnd < length) {
			[string getParagraphStart:&paraStart end:&paraEnd
						  contentsEnd:&contentsEnd forRange:NSMakeRange(paraEnd, 0)];
			currentRange = NSMakeRange(paraStart, contentsEnd - paraStart);
			[lines addObject:[string substringWithRange:currentRange]];
		}
		
		/* Create template */
		NSMutableArray *templates = [NSMutableArray array];
		for (NSString *line in lines)
		{
			NSArray *components = [line componentsSeparatedByString:@","];
			LCDeviceEditTemplate *template = [[LCDeviceEditTemplate new] autorelease];
			if (components.count > 0) template.desc = [components objectAtIndex:0];
			if (components.count > 1) template.ipAddress = [components objectAtIndex:1];
			if (components.count > 2) template.lomIpAddress = [components objectAtIndex:2];
			if ([template.desc length] > 0 && [template.ipAddress length] > 0)
			{ [templates addObject:template]; }
			else
			{ NSLog (@"WARNING: Skipped invalid line '%@' in CSV file %@", line, [panel filename]); }
		}
		
		/* Setup controller */
		if (templates.count <= 1)
		{
			self.mode = ADD_MODE_SINGLE;
			if (templates.count == 1)
			{ self.singleTemplate = [templates objectAtIndex:0]; }
		}
		else
		{
			/* Use multiple-device template */
			self.mode = ADD_MODE_MULTIPLE;
			self.multipleTemplates = templates;
			[self resizeSheetForMultipleAdd];
		}
		
		/* Update first responder */
		if (self.credentialsSelectedTab == 0)
		{
			/* SNMP */
			if (self.snmpVersion == 1)
			{ [[self window] makeFirstResponder:snmpCommunityTextField]; }
			else
			{ [[self window] makeFirstResponder:snmpUsernameTextField]; }
		}
		else if (self.credentialsSelectedTab == 1)
		{
			/* Username/Password */
			[[self window] makeFirstResponder:usernameTextField];
		}
		else if (self.credentialsSelectedTab == 2)
		{
			/* Xserve RAID (Password) */
			[[self window] makeFirstResponder:raidPasswordTextField];
		}
		
		/* Open edit sheet */
		[panel orderOut:self];
		[NSApp beginSheet:[self window]
		   modalForWindow:windowForSheet
			modalDelegate:self
		   didEndSelector:nil
			  contextInfo:nil];
	}
}

#pragma mark "XML Operation Methods"

- (LCXMLRequest *) performUpdateUsingProperties:(NSMutableDictionary *)properties
{
	/* Update the device */
	NSXMLDocument *xmldoc;
	NSXMLElement *rootnode;
	
	/* Create XML */
	rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"device"];
	xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	NSEnumerator *keys = [properties keyEnumerator];
	NSString *key;
	while (key=[keys nextObject])
	{ 
		/* Skip default password */
		if ([key isEqualToString:@"password"] && [[properties objectForKey:key] isEqualToString:@"********"]) continue;
		if ([key isEqualToString:@"lom_password"] && [[properties objectForKey:key] isEqualToString:@"********"]) continue;
		
		/* Add to XML */
		[rootnode addChild:[NSXMLNode elementWithName:key stringValue:[[properties objectForKey:key] description]]];
	}
	
	/* Create and perform request */
	LCXMLRequest *req;
	if (testMode)
	{
		req = [[LCXMLRequest requestWithCriteria:[siteEntity customer]
												 resource:[[siteEntity customer] resourceAddress]
												   entity:[[siteEntity customer] entityAddress]
												  xmlname:@"device_test"
												   refsec:0
												   xmlout:xmldoc] retain];
	}
	else
	{
		req = [[LCXMLRequest requestWithCriteria:[siteEntity customer]
												 resource:[[siteEntity customer] resourceAddress]
												   entity:[[siteEntity customer] entityAddress]
												  xmlname:@"device_update"
												   refsec:0
												   xmlout:xmldoc] retain];		
	}
	[req setDelegate:self];
	[req setXMLDelegate:self];
	[req setPriority:XMLREQ_PRIO_HIGH];
	[req performAsyncRequest];
	
	return req;
}

- (LCXMLRequest *) performRemove
{
	/* Update the device */
	NSXMLDocument *xmldoc;
	NSXMLElement *rootnode;
	
	/* Create XML */
	rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"device"];
	xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"name" stringValue:[deviceEntity name]]];
	[rootnode addChild:[NSXMLNode elementWithName:@"site_name" stringValue:[siteEntity name]]];
	
	/* Create and perform request */
	removeXMLReq = [[LCXMLRequest requestWithCriteria:[siteEntity customer]
									resource:[[siteEntity customer] resourceAddress]
									  entity:[[siteEntity customer] entityAddress]
									 xmlname:@"device_remove"
									  refsec:0
									  xmlout:xmldoc] retain];		
	[removeXMLReq setDelegate:self];
	[removeXMLReq setPriority:XMLREQ_PRIO_HIGH];
	[removeXMLReq performAsyncRequest];
	
	[self setXmlOperationInProgress:YES];
	
	return removeXMLReq;
}

- (LCXMLRequest *) performMove
{
	/* Update the device */
	NSXMLDocument *xmldoc;
	NSXMLElement *rootnode;
	
	/* Create XML */
	rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"device_move"];
	xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	for (LCDevice *device in devices)
	{
		NSXMLElement *deviceNode = [NSXMLNode elementWithName:@"device"];
		[deviceNode addChild:[NSXMLNode elementWithName:@"dev_entaddr" stringValue:[[device entityAddress] addressString]]];
		[deviceNode addChild:[NSXMLNode elementWithName:@"dest_site_entaddr" stringValue:[[siteEntity entityAddress] addressString]]];
		[rootnode addChild:deviceNode];
	}
		
	/* Create and perform request */
	moveXMLReq = [[LCXMLRequest requestWithCriteria:[siteEntity customer]
											 resource:[[siteEntity customer] resourceAddress]
											   entity:[[siteEntity customer] entityAddress]
											  xmlname:@"device_move"
											   refsec:0
											   xmlout:xmldoc] retain];		
	[moveXMLReq setDelegate:self];
	[moveXMLReq setPriority:XMLREQ_PRIO_HIGH];
	[moveXMLReq performAsyncRequest];
	
	[self setXmlOperationInProgress:YES];
	
	return moveXMLReq;
}

#pragma mark "XML Request Delegate Methods"

- (void) XMLRequestPreParse:(LCXMLRequest *)sender
{
	xmlProperties = [[NSMutableDictionary dictionary] retain]; 
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Check to see if this was a remove */
	if (sender == removeXMLReq)
	{
		/* Hide progress sheet and remove device */
		[self setXmlOperationInProgress:NO];
		[NSApp endSheet:removeProgressSheet];
		[removeProgressSheet close];
		[siteEntity removeChild:deviceEntity];
		[siteEntity.customer highPriorityRefresh];
		[self autorelease];
		[removeXMLReq release];
		removeXMLReq = nil;
		return;
	}

	/* Check to see if this was a move */
	if (sender == moveXMLReq)
	{
		/* Hide progress sheet */
		[self setXmlOperationInProgress:NO];
		[NSApp endSheet:moveProgressSheet];
		[moveProgressSheet close];
		
		/* Refresh customer to ensure tree is updated */
		[siteEntity.customer highPriorityRefresh];
		[[siteEntity.customer groupTree] highPriorityRefresh];
		
		/* Cleanup */
		[self autorelease];
		[moveXMLReq release];
		moveXMLReq = nil;
		return;
	}	
	
	/* Check to see if this was a mark */
	if (sender == markXMLReq)
	{
		/* Remove device entities */
		NSArray *childrenToRemove = [deviceEntity.children copy];
		for (LCContainer *container in childrenToRemove)
		{
			[deviceEntity removeChild:container];
		}
		deviceEntity.opState = self.mark;
		deviceEntity.mark = self.mark;
		
		/* Refresh customer */
		[siteEntity.customer highPriorityRefresh];
		
		/* Cleanup */
		[self autorelease];
		[markXMLReq release];
		markXMLReq = nil;
		return;	
	}
	
	/*
	 * Process the XML Received 
	 */

	/* Get the template */
	LCDeviceEditTemplate *template = [templateXmlRequests objectForKey:[sender description]];
	[templateXmlRequests removeObjectForKey:[sender description]];
	template.xmlOperationInProgress = NO;
	template.xmlReq = nil;

	/* Check result */
	if ([sender success] && xmlProperties && [xmlProperties count] > 0)
	{
		
		/* Check if this is a live operation or a test */
		if (!testMode)
		{
			/* Check for entity */
			if (deviceEntity)
			{
				/* This was an update operation, update existing entity */
				for (NSString *key in xmlProperties)
				{ [deviceEntity setXmlValue:[xmlProperties objectForKey:key] forKey:key]; }
				[deviceEntity highPriorityRefresh];
			}
			else
			{
				/* Create entity using received properties */
				if (xmlProperties)
				{
					LCDevice *entity = (LCDevice *) [[LCDevice alloc] initWithType:3
																			  name:[xmlProperties objectForKey:@"name"]
																			  desc:[xmlProperties objectForKey:@"desc"]
															 resourceAddressString:[xmlProperties objectForKey:@"resadr"]];
					for (NSString *key in xmlProperties)
					{
						[entity setXmlValue:[xmlProperties objectForKey:key] forKey:key]; 
					}
					[entity setParent:siteEntity];
					[entity setPresenceConfirmed:YES];
					[siteEntity insertObject:entity inChildrenAtIndex:[[siteEntity children] count]];
					[entity highPriorityRefresh];
					[entity autorelease];
				}
			}			
		}
		else
		{
			/* Check test result */
			if ([[xmlProperties objectForKey:@"result"] intValue] == 1)
			{
				/* Test Successful!! */
				template.resultIcon = [NSImage imageNamed:@"ok_16.tif"];				
			}
			else
			{
				/* Test Failed */
				template.resultIcon = [NSImage imageNamed:@"stop_16.tif"];				
				errorEncountered = YES;
			}
		}
	}
	else
	{
		/* Error occurred, bad data received */
		template.resultIcon = [NSImage imageNamed:@"stop_16.tif"];				
		errorEncountered = YES;		
	}
	
	/*
	 * Perform operation-specific tasks
	 */

	/* Check operation status */
	if ([templateXmlRequests count] == 0)
	{ 
		/* All Finished */
		[self setXmlOperationInProgress:NO];
		if (errorEncountered)
		{
			if (testMode)
			{ 
				if (self.mode == ADD_MODE_SINGLE) [self setStatusString:@"No response from device."]; 
				else if (self.mode == ADD_MODE_MULTIPLE) [self setStatusString:@"One or more devices did not respond."]; 
			}
			else
			{
				if (deviceEntity) 
				{
					[self setStatusString:@"Failed to update device"];
				}
				else
				{
					if (self.mode == ADD_MODE_SINGLE) [self setStatusString:@"Failed to add device."];
					else if (self.mode == ADD_MODE_MULTIPLE) [self setStatusString:@"Failed to add one or more devices."];					
				}
			}
			[self setStatusIcon:[NSImage imageNamed:@"stop_16.tif"]];
		}
		else
		{
			if (testMode)
			{ 
				if (self.mode == ADD_MODE_SINGLE) [self setStatusString:@"Device responded."]; 
				else if (self.mode == ADD_MODE_MULTIPLE) [self setStatusString:@"All devices responded."]; 
			}
			else
			{
				if (deviceEntity)
				{
					[self setStatusString:@"Device updated successfully"];
				}
				else 
				{
					if (self.mode == ADD_MODE_SINGLE) [self setStatusString:@"Device added."]; 
					else if (self.mode == ADD_MODE_MULTIPLE) [self setStatusString:@"All devices added."]; 
				}
				[NSTimer scheduledTimerWithTimeInterval:0.0 
												 target:self 
											   selector:@selector(closeSheetTimerCallback:) 
											   userInfo:nil 
												repeats:NO];
			}
			[self setStatusIcon:[NSImage imageNamed:@"ok_16.tif"]];
		}
	}

	/* Cleanup */
	[xmlProperties release];
	xmlProperties = nil;
	[sender release];
}

#pragma mark "XML Parser Delegate Methods"

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	/* Retain element */
	xmlElement = [element retain];
	
	/* Release previous string */
	if (xmlString)
	{
		[xmlString release];
		xmlString = nil;
	}
}

- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	/* Create new string or append string to existing */
	if (xmlElement)
	{
		if (xmlString) { [xmlString appendString:string]; }
		else { xmlString = [[NSMutableString stringWithString:string] retain]; }
	}
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Update properties */
	if (xmlElement && xmlString)
	{ 
		[xmlProperties setObject:xmlString forKey:xmlElement]; 
		[xmlString release];
		xmlString = nil;
	}
	
	/* Release current element */
	if (xmlElement)
	{
		[xmlElement release];
		xmlElement = nil;
	}
}

#pragma mark "Close Sheet Timer Callback"

- (void) closeSheetTimerCallback:(NSTimer *)timer
{
	[NSApp endSheet:[self window]];
	[[self window] close];
}	

@end
