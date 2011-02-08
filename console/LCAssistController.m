//
//  LCAssistController.m
//  Lithium Console
//
//  Created by James Wilson on 26/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCAssistController.h"

#import "LCDeviceEditController.h"
//#import "LCAssistXSPPCTask.h"
//#import "LCAssistXSIntelTask.h"
#import "LCAssistDeviceTask.h"
#import "LCSiteEditController.h"
#import "LCBrowser2Controller.h"
#import "LCDemoRegoWindowController.h"

@implementation LCAssistController

#pragma mark "Constructors"

- (LCAssistController *) initForCustomer:(LCCustomer *)initCustomer
{
	/* Init */
	[super initWithWindowNibName:@"SetupAssistantWindow"];
	self.customer = initCustomer;
	doNotShowAgain = [[NSUserDefaults standardUserDefaults] boolForKey:[NSString stringWithFormat:@"LCAssist_%@_doNotShowAgain", [customer name]]];
	devTaskDict = [[NSMutableDictionary dictionary] retain];
	devTasks = [[NSMutableArray array] retain];
	if (self.customer.children.count > 0)
	{ self.addToSiteSelection = [self.customer.children objectAtIndex:0]; }
	
	/* Create devTasks list */
	[self createDeviceTasks];
	
	/* Load/Setup NIB */
	[self window];
	NSRect curFrame = [[self window] frame];
	[[self window] setFrame:NSMakeRect (NSMinX(curFrame),NSMinY(curFrame),605,580) display:NO];
	[backView setImage:[NSImage imageNamed:@"slateback.png"]];
	
	/* Display */
	[self showWelcome];
	[[self window] makeKeyAndOrderFront:self];
	
	/* Check for unregistered demo */
	if (customer.licenseType == 6)
	{
		/* Deployment is an unregistered demo */
		LCDemoRegoWindowController *regoController = (LCDemoRegoWindowController *) [[LCDemoRegoWindowController alloc] initForCustomer:customer];
		[NSApp beginSheet:[regoController window]
		   modalForWindow:[self window]
			modalDelegate:self
		   didEndSelector:nil
			  contextInfo:nil];
	}

	return self;
}

#pragma mark "Create Device Tasks"

- (void) createDeviceTasks
{
	/* 
	 * Server/Workstation
	 */
	
	LCAssistDeviceTask *task;
	
	task = [LCAssistDeviceTask taskForVendor:@"xserve_g5"
								  deviceType:@"Xserve G5 (PPC)"
							  requiredFields:@"osx"
							  withController:self];
	[task setBlurb:@"Lithium can monitor Apple Xserves with G5 (PPC) processesors using Apple's proprietary Server Manager protocols.\n\nYou will need to specify the username and password of a user account with Administrator privileges. Lithium will use this account to authenticate when connecting to the server to gather monitoring data."];
	[task setThumbnail:[NSImage imageNamed:@"tn_xserve.png"]];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];

	task = [LCAssistDeviceTask taskForVendor:@"xserve_intel"
								  deviceType:@"Xserve Intel Xeon"
							  requiredFields:@"lom"
							  withController:self];
	[task setBlurb:@"Lithium can monitor Apple Xserves with Intel (Xeon) processesors using both Apple's Lights-Out Management and Server Manager protocols.\n\nYou will need to specify the Lights-Out Management username and password and there MUST be a USER ACCOUNT with Administrator privileges configured on the server with the same username and password."];
	[task setThumbnail:[NSImage imageNamed:@"tn_xserve.png"]];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];	
	
	task = [LCAssistDeviceTask taskForVendor:@"osx_server"
								  deviceType:@"Non-Xserve Mac OS X Server"
							  requiredFields:@"osx"
							  withController:self];
	[task setBlurb:@"Lithium can monitor Non-Xserve Macs with using Apple's proprietary Server Manager protocols.\n\nYou will need to specify the username and password of a user account with Administrator privileges. Lithium will use this account to authenticate when connecting to the server to gather monitoring data."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];
	
	task = [LCAssistDeviceTask taskForVendor:@"osx_client"
								  deviceType:@"Mac OS X Client"
							  requiredFields:@"osx"
							  withController:self];
	[task setBlurb:@"Lithium can monitor Xsan Client Macs running Mac OS X Client.\n\nYou will need to specify the username and password of a user account with Administrator privileges. Lithium will use this account to authenticate when connecting to the computer to gather monitoring data."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];
	
	task = [LCAssistDeviceTask taskForVendor:@"netsnmp"
								  deviceType:@"Linux/Unix Net-SNMP Agent"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor Linux or Unix server and computers running Mac OS X Client using SNMP.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];

	task = [LCAssistDeviceTask taskForVendor:@"windows"
								  deviceType:@"Windows"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor servers and workstations running Microsoft Windows using SNMP.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];	
	
	/*
	 * Storage 
	 */
	
	task = [LCAssistDeviceTask taskForVendor:@"activestorage"
								  deviceType:@"ActiveRAID"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor Active Storage ActiveRAID devices using SNMP.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];

	task = [LCAssistDeviceTask taskForVendor:@"brocadefcsw"
								  deviceType:@"Brocade Fibre Channel Switch"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor Brocade Fibre Channel Switches using SNMP.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];	

	task = [LCAssistDeviceTask taskForVendor:@"qlogic"
								  deviceType:@"Qlogic Fibre Channel Switch"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor Qlogic Fibre Channel Switches using SNMP.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];	
	
	task = [LCAssistDeviceTask taskForVendor:@"genericfcswitch"
								  deviceType:@"Generic Fibre Channel Switch"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor Generic Fibre Channel Switches that support the Fibre Alliance (FA-40) SNMP MIB.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];	

	task = [LCAssistDeviceTask taskForVendor:@"vtrak"
								  deviceType:@"Promise VTrak RAID"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor Promise VTrak RAID units using SNMP.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters (the default is 'public')."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];
	
	task = [LCAssistDeviceTask taskForVendor:@"filer"
								  deviceType:@"NetApp Filer"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor NetApp Filer storage units using SNMP.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];
	
	task = [LCAssistDeviceTask taskForVendor:@"infortrend"
								  deviceType:@"Infortrend RAID"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor Infortrend based RAID units using SNMP.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];	
	
	task = [LCAssistDeviceTask taskForVendor:@"xraid"
								  deviceType:@"Xserve RAID"
							  requiredFields:@"xraid"
							  withController:self];
	[task setBlurb:@"Lithium can monitor Apple Xserve RAID units using proprietary monitoring protocols.\n\nYou will need to specify the 'monitoring' password for the Xserve RAID unit (the default is 'public')."];
	[task setThumbnail:[NSImage imageNamed:@"tn_xraid.png"]];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];
	
	
	/*
	 * Network 
	 */
	
	task = [LCAssistDeviceTask taskForVendor:@"cisco"
								  deviceType:@"Cisco Router / Switch / Firewall"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor Cisco Routers, Switches and Firewalls running IOS or PIX OS using SNMP.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];

	task = [LCAssistDeviceTask taskForVendor:@"3comswitch"
								  deviceType:@"3com Managed Switch"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor 3com managed switches using SNMP.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];
	
	task = [LCAssistDeviceTask taskForVendor:@"hp"
								  deviceType:@"HP Procurve Switch"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor HP Procurve switches using SNMP.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];
	
	task = [LCAssistDeviceTask taskForVendor:@"apcups"
								  deviceType:@"APC UPS"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor APC UPS units equipped with a 'Web Card' or 'SNMP Card'.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[task setThumbnail:[NSImage imageNamed:@"tn_apcups.png"]];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];
	
	task = [LCAssistDeviceTask taskForVendor:@"foundry"
								  deviceType:@"Foundry ServerIron Load Balancer"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor Foundry ServerIron load balancers using SNMP.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];	
	
	task = [LCAssistDeviceTask taskForVendor:@"airport"
								  deviceType:@"Airport Basestation"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor Apple Airport base stations using SNMP.\n\nFor 802.11g (Dome) units you will need to supply the management password for the device as the SNMP community string. For 802.11n (Flat) units you will need to specify the SNMP community string set via the Airport Admin utility."];
	[task setThumbnail:[NSImage imageNamed:@"tn_airport.png"]];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];
	
	/*
	 * Generic
	 */
	
	task = [LCAssistDeviceTask taskForVendor:@"snmpbasic"
								  deviceType:@"Generic SNMP (Network Device)"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium provides Generic SNMP monitoring that should provide basic monitoring coverage for the majority of network devices.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];
	
	task = [LCAssistDeviceTask taskForVendor:@"snmpadv"
								  deviceType:@"Generic SNMP (Server/Workstation)"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium provides Generic SNMP monitoring that should provide basic monitoring coverage for the majority of server/workstation style devices.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];
	
	task = [LCAssistDeviceTask taskForVendor:@"printer"
								  deviceType:@"Printer (SNMP Enabled)"
							  requiredFields:@"snmp"
							  withController:self];
	[task setBlurb:@"Lithium can monitor SNMP-enabled Printers using SNMP.\n\nYou will need to specify a read-only SNMP community string or SNMPv3 parameters."];
	[devTaskDict setObject:task forKey:[task vendor]];
	[devTasks addObject:task];	
}

#pragma mark "Navigation Actions"

- (IBAction) nextClicked:(id)sender
{
	int curIndex = [tabView indexOfTabViewItem:[tabView selectedTabViewItem]];
	
	if (curIndex == TAB_WELCOME)
	{ [self showDeviceTypes]; }
	else if (curIndex == TAB_SITEADD)
	{ [self exitFromSiteAdd]; }
	else if (curIndex == TAB_DEVTYPE)
	{ [self showFirstDeviceAdd]; }
	else if (curIndex == TAB_DEVADD)
	{ [self showNextDeviceAdd]; }
	else if (curIndex == TAB_DEVREV)
	{ [self showFinished]; }
	else if (curIndex == TAB_FINISHED)
	{
		[[self window] close];
		if ([[LCBrowser2Controller activeControllers] count] < 1)
		{ [[[LCBrowser2Controller alloc] initWithEntity:customer] autorelease]; }
	}
}

- (IBAction) backClicked:(id)sender
{
	int curIndex = [tabView indexOfTabViewItem:[tabView selectedTabViewItem]];

	if (curIndex == TAB_SITEADD)
	{ [self exitFromSiteAdd]; }
	else if (curIndex == TAB_DEVTYPE)
	{ [self showWelcome]; }
	else if (curIndex == TAB_DEVADD)
	{ [self showPrevDeviceAdd]; }
	else if (curIndex == TAB_DEVREV)
	{ [self showLastDeviceAdd]; }
	else if (curIndex == TAB_FINISHED)
	{ [self showDeviceReview]; }
}

- (IBAction) cancelCkicked:(id)sender
{
	[[self window] close];
}

- (IBAction) addMoreDevicesClicked:(id)sender
{
	if (!addToSiteSelection) return;
	LCDeviceEditController *controller;
	controller = [[LCDeviceEditController alloc] initForNewDeviceAtSite:addToSiteSelection usingModule:[currentDeviceTask vendor]];
	[NSApp beginSheet:[controller window]
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (IBAction) addMoreSitesClicked:(id)sender
{
	[self showSiteAdd];
}

- (IBAction) addSiteClicked:(id)sender
{
	LCSiteEditController *controller;
	controller = [[LCSiteEditController alloc] initForNewSiteAtCustomer:customer];
	[NSApp beginSheet:[controller window]
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

#pragma mark "View Management Methods"

- (void) showWelcome
{
	[tabView selectTabViewItemAtIndex:0];
}

- (void) showSiteAdd
{
	indexBeforeSiteAdd = [tabView indexOfTabViewItem:[tabView selectedTabViewItem]];
	
	[tabView selectTabViewItemAtIndex:1];	
}

- (void) exitFromSiteAdd
{
	[tabView selectTabViewItemAtIndex:indexBeforeSiteAdd];
}

- (void) showDeviceTypes
{
	[tabView selectTabViewItemAtIndex:2];
}

- (void) showFirstDeviceAdd
{
	[tabView selectTabViewItemAtIndex:3];
	
	if (enabledTasks) [enabledTasks release];
	enabledTasks = [[NSMutableArray arrayWithArray:devTasks] retain];
	[enabledTasks filterUsingPredicate:[NSPredicate predicateWithFormat:@"enabled = YES"]];
	
	if ([enabledTasks count] > 0)
	{
		self.currentDeviceTask = nil;
		[self showNextDeviceAdd];
	}
	else
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"No device types were selected."
										 defaultButton:@"OK" 
									   alternateButton:nil 
										   otherButton:nil
							 informativeTextWithFormat:@"Please select atleast one device type."];
		[alert setIcon:[NSImage imageNamed:@"stop_48.tif"]];
		[alert beginSheetModalForWindow:[self window]
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
		[self showDeviceTypes];
	}
}

- (void) showNextDeviceAdd
{	
	if (currentDeviceTask)
	{
		int curIndex = [enabledTasks indexOfObject:currentDeviceTask];
		if (curIndex == ([enabledTasks count] - 1))
		{
			/* Last one */
			[self showDeviceReview];
		}
		else
		{
			/* Move to next */
			self.currentDeviceTask = [enabledTasks objectAtIndex:curIndex+1];
		}
	}
	else
	{
		/* First */
		self.currentDeviceTask = [enabledTasks objectAtIndex:0];
	}
}

- (void) showPrevDeviceAdd
{
	int curIndex = [enabledTasks indexOfObject:currentDeviceTask];
	if (curIndex == 0)
	{
		/* First */
		[self showDeviceTypes];
	}
	else
	{
		/* Move back */
		self.currentDeviceTask = [enabledTasks objectAtIndex:curIndex-1];
	}
}

- (void) showLastDeviceAdd
{
	[tabView selectTabViewItemAtIndex:3];
	
	if ([enabledTasks count] > 0)
	{
		self.currentDeviceTask = [enabledTasks objectAtIndex:[enabledTasks count]-1];
	}
	else
	{
		[self showDeviceTypes];
	}	
}

- (void) showDeviceReview
{
	[tabView selectTabViewItemAtIndex:4];
}

- (void) showFinished
{
	[tabView selectTabViewItemAtIndex:5];
	[nextButton setTitle:@"Close"];
	[cancelButton setHidden:YES];
	[backButton setHidden:YES];
	[addSitesButton setHidden:YES];
}

#pragma mark "Accessors"

- (LCCustomer *) customer
{ return customer; }

- (void) setCustomer:(LCCustomer *)newCustomer
{
	[customer release];
	customer = [newCustomer retain];
}

- (LCEntity *) selectedSite
{
	return addToSiteSelection;
}

@synthesize doNotShowAgain;
- (void) setDoNotShowAgain:(BOOL)flag
{ 
	doNotShowAgain = flag; 
	[[NSUserDefaults standardUserDefaults] setBool:flag 
											forKey:[NSString stringWithFormat:@"LCAssist_%@_doNotShowAgain", [customer name]]];
}

@synthesize devTaskDict;
@synthesize devTasks;
@synthesize enabledTasks;

@synthesize indexBeforeSiteAdd;
@synthesize currentDeviceTask;

@synthesize addToSiteSelection;
- (void) setAddToSiteSelection:(LCSite *)value
{ addToSiteSelection = value; }

@end
