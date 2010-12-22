//
//  LTDeviceEditTableViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 21/12/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTDeviceEditTableViewController.h"
#import "LTEntity.h"
#import "LTDeviceEditModuleListTableViewController.h"
#import "LTDeviceEditProtocolListTableViewController.h"
#import "LTDeviceEditSNMPVersionListTableViewController.h"
#import "LTDeviceEditRefreshIntervalListTableViewController.h"
#import "LTDeviceEditSNMPAuthenticationListTableViewController.h"
#import "LTDeviceEditSNMPPrivacyListTableViewController.h"
#import "LTCustomer.h"
#import "LTCoreDeployment.h"

@implementation LTDeviceEditTableViewController

@synthesize device, site;
@synthesize devSNMPVersion, devSNMPAuthMethod, devSNMPPrivacyMethod, devVendorModule;
@synthesize devRefreshInterval, devProtocol;

#define kLastUsedVendorKey @"LTDeviceEditTableViewControllerLastUsedVendorModule"
#define kLastUsedRefreshInterval @"LTDeviceEditTableViewControllerLastUsedRefreshInterval"

#pragma mark -
#pragma mark Initialization

- (id)initWithDeviceToEdit:(LTEntity *)initDevice
{
    self = [self initWithStyle:UITableViewStyleGrouped];
    if (self) 
	{
		self.device = initDevice;
		self.site = self.device.parent;
		
		/* Setup Device Properties */
		devDesc.text = self.device.desc;
		devIPAddress.text = self.device.deviceIpAddress;
		devLOMIPAddress.text = self.device.lomIpAddress;
		self.devSNMPVersion = self.device.snmpVersion;
		devSNMPCommunity.text = self.device.snmpCommunity;
		self.devSNMPAuthMethod = self.device.snmpAuthMethod;
		devSNMPAuthPassword.text = self.device.snmpAuthPassword;
		self.devSNMPPrivacyMethod = self.device.snmpPrivacyMethod;
		devSNMPPrivacyPassword.text = self.device.snmpPrivacyPassword;
		devUsername.text = self.device.deviceUsername;
		devPassword.text = self.device.devicePassword;
		devLOMUsername.text = self.device.lomUsername;
		devLOMPassword.text = self.device.lomPassword;
		self.devVendorModule = self.device.vendorModule;
		self.devRefreshInterval = self.device.refreshInterval;
		self.devProtocol = self.device.deviceProtocol;
		devUseICMP.on = self.device.useICMP;
		devUseProcessList.on = self.device.useProcessList;
		devUseLOM.on = self.device.useLOM;
    }
    return self;
}

- (id)initForNewDeviceAtSite:(LTEntity *)initSite
{	
    self = [self initWithStyle:UITableViewStyleGrouped];
    if (self) 
	{
		self.site = initSite;
		devVendorModule = [[NSUserDefaults standardUserDefaults] objectForKey:kLastUsedVendorKey] ? : @"osx_server";
		devSNMPVersion = 2;
		devRefreshInterval = 60;
		if ([[NSUserDefaults standardUserDefaults] objectForKey:kLastUsedRefreshInterval]) 
		{ devRefreshInterval = [[NSUserDefaults standardUserDefaults] integerForKey:kLastUsedRefreshInterval]; }
		devProtocol = 0;
    }
    return self;
}

- (UITextField *) propertyTextField
{
	UITextField *returnTextField = [[UITextField alloc] initWithFrame:CGRectMake(0., 0., 200.0, 23.)];
	returnTextField.textAlignment = UITextAlignmentRight;
	returnTextField.borderStyle = UITextBorderStyleNone;
    returnTextField.textColor = [UIColor colorWithRed:0.22 green:0.33 blue:0.53 alpha:1.0];
	returnTextField.font = [UIFont systemFontOfSize:17.0];
    returnTextField.placeholder;
    returnTextField.backgroundColor = [UIColor clearColor];
	returnTextField.autocorrectionType = UITextAutocorrectionTypeNo;	// no auto correction support
	returnTextField.delegate = self;
	
	returnTextField.keyboardType = UIKeyboardTypeDefault;
	returnTextField.returnKeyType = UIReturnKeyDefault;
	
	returnTextField.clearButtonMode = UITextFieldViewModeWhileEditing;	// has a clear 'x' button to the right
	return returnTextField;
}

- (UISwitch *) propertySwitch
{
	UISwitch *switchCtl = [[UISwitch alloc] initWithFrame:CGRectMake(0.0, 0.0, 94.0, 27.0)];
	switchCtl.backgroundColor = [UIColor clearColor];
	return switchCtl;
}

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:UITableViewStyleGrouped];
    if (self) 
	{
		/* Create Labels for properties */
		devDesc = [self propertyTextField];
		devDesc.placeholder = @"Description";
		devIPAddress = [self propertyTextField];
		devIPAddress.placeholder = @"IP Address";
		devLOMPassword = [self propertyTextField];
		devLOMPassword.placeholder = @"LOM IP Address";
		devSNMPCommunity = [self propertyTextField];
		devSNMPCommunity.placeholder = @"SNMP Community";
		devSNMPAuthPassword = [self propertyTextField];
		devSNMPAuthPassword.secureTextEntry = YES;
		devSNMPAuthPassword.placeholder = @"Password";
		devSNMPPrivacyPassword = [self propertyTextField];
		devSNMPPrivacyPassword.secureTextEntry = YES;
		devSNMPPrivacyPassword.placeholder = @"Password";
		devUsername = [self propertyTextField];
		devUsername.placeholder = @"Username";
		devPassword = [self propertyTextField];
		devPassword.placeholder = @"Password";
		devPassword.secureTextEntry = YES;
		devLOMUsername = [self propertyTextField];
		devLOMUsername.placeholder = @"LOM Username";
		devLOMPassword = [self propertyTextField];
		devLOMPassword.placeholder = @"LOM Password";
		devLOMPassword.secureTextEntry = YES;
		devUseICMP = [self propertySwitch];
		devUseLOM = [self propertySwitch];
		devUseProcessList = [self propertySwitch];		
    }
    return self;	
}

#pragma mark -
#pragma mark View lifecycle

- (void)viewDidLoad 
{
    [super viewDidLoad];

	self.navigationItem.leftBarButtonItem = [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCancel target:self action:@selector(cancelTouched:)] autorelease];
	self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemSave target:self action:@selector(saveTouched:)] autorelease];
	
	if (self.device) self.navigationItem.title = [NSString stringWithFormat:@"Edit Device %@", self.device.desc];
	else self.navigationItem.title = [NSString stringWithFormat:@"Add Device at %@", self.site.desc];
}

- (void)viewWillAppear:(BOOL)animated {
	
    [super viewWillAppear:animated];
	[self.tableView reloadData];
}

/*
- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
}
*/

//- (void)viewWillDisappear:(BOOL)animated 
//{
//    [super viewWillDisappear:animated];
//}

/*
- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
}
*/

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation 
{
	return YES;
}


#pragma mark -
#pragma mark Table view data source

#define TYPE_UNKNOWN 0

/* Section 0 */
#define TYPE_BASIC 1

/* Section 1 */
#define TYPE_FEATURE_SELECT 7

/* Section 2/3 */
#define TYPE_PROTOCOL_SELECT 2
#define TYPE_SNMP 4
#define TYPE_SERVERMGRD 5
#define TYPE_PASSWORDONLY 6

/* Section 3/4 */
#define TYPE_REFRESH 8

- (BOOL) isDeviceRunningOSX
{
	if ([self.devVendorModule hasPrefix:@"xserve"] || [self.devVendorModule hasPrefix:@"osx"])
	{ return YES; }
	else 
	{ return NO; }
}

- (BOOL) isDeviceICMPOnly
{
	if ([self.devVendorModule isEqualToString:@"icmp"]) 
	{ return YES; }
	else 
	{ return NO; }
}

- (BOOL) isDevicePasswordOnly
{
	if ([self.devVendorModule isEqualToString:@"xraid"]) 
	{ return YES; }
	else 
	{ return NO; }
}

- (BOOL) isDeviceServer
{
	if ([self.devVendorModule isEqualToString:@"netsnmp"] || 
		[self.devVendorModule isEqualToString:@"osx_server"] || 
		[self.devVendorModule isEqualToString:@"snmpadv"] || 
		[self.devVendorModule isEqualToString:@"windows"] ||
		[self.devVendorModule isEqualToString:@"xserve"])
	{ return YES; }
	else 
	{ return NO; }	
}

- (BOOL) isDeviceUsingSNMP
{
	if ([self isDeviceRunningOSX])
	{
		/* Check protocol flag */
		if (devProtocol == 1) 
		{ return NO; }
		else 
		{ return YES; }
	}
	else 
	{
		/* Assume Yes */
		return YES;
	}
}

- (NSInteger) deviceSNMPVersion
{
	return devSNMPVersion;
}

- (NSInteger)typeForSection:(NSInteger)section
{
	/* Section 0 */
	if (section == 0) 
	{
		return TYPE_BASIC;
	}
	
	/* Section 1 (Feature, or straight to Refresh) */
	else if (section == 1 && [self isDeviceICMPOnly])
	{
		return TYPE_REFRESH;
	}
	else if (section == 1) 
	{
		return TYPE_FEATURE_SELECT;
	}
	
	/* Section 2 (Protocol Select or SNMP Setup) */
	else if (section == 2 && [self isDeviceRunningOSX])
	{
		/* Device is OSX, show protocol selection section */
		return TYPE_PROTOCOL_SELECT;
	}
	else if (section == 2 && [self isDevicePasswordOnly])
	{
		/* Device requires a password only (Xserve RAID) */
		return TYPE_PASSWORDONLY;
	}
	else if (section == 2)
	{
		/* Otherwise, section 2 is always SNMP */
		return TYPE_SNMP;
	}
	
	/* Section 3 (SNMP/Server MgrdD Setup or Refresh) */
	else if (section == 3 && [self isDeviceRunningOSX] && [self isDeviceUsingSNMP])
	{
		/* OSX and SNMP */
		return TYPE_SNMP;
	}
	else if (section == 3 && [self isDeviceRunningOSX])
	{
		/* OSX and Servermgrd */
		return TYPE_SERVERMGRD;
	}
	else if (section == 3)
	{
		/* Otherwise, section 3 is always Refresh */
		return TYPE_REFRESH;
	}
	
	/* Section 4 (Refresh for OSX) */
	else if (section == 4)
	{
		return TYPE_REFRESH;
	}	
	
	/* Default */
	else return 0;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
    // Return the number of sections.
	if ([self isDeviceICMPOnly])
	{
		/* Basic + Refresh */
		return 2;
	}
	else if ([self isDeviceRunningOSX])
	{
		/* Basic + Feature + Protocol + SNMP/ServerMgr + Refresh */
		return 5;
	}
	else
	{
		/* Basic + Feature + SNMP/Password + Refresh */
		return 4;
	}
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
    // Return the number of rows in the section.
	if ([self typeForSection:section] == TYPE_BASIC)
	{
		/* Basic device info (Type, Desc, IP Address) */
		return 3;
	}
	else if ([self typeForSection:section] == TYPE_FEATURE_SELECT)
	{
		/* Use ICMP + Process List (If Supported) */
		if ([self isDeviceServer]) return 2;
		else return 1;
	}
	else if ([self typeForSection:section] == TYPE_PROTOCOL_SELECT)
	{
		/* Single Row */
		return 1;
	}
	else if ([self typeForSection:section] == TYPE_SNMP)
	{
		/* SNMP Config (version dependent) */
		if ([self deviceSNMPVersion] == 3)
		{
			/* Version + Usermame + Auth + Auth Pass + Privacy + Privacy Pass */
			return 6;
		}
		else 
		{
			/* Version + Community */
			return 2;
		}
	}
	else if ([self typeForSection:section] == TYPE_SERVERMGRD)
	{
		/* Username + Password */
		return 2;
	}
	else if ([self typeForSection:section] == TYPE_PASSWORDONLY)
	{
		/* Password */
		return 1;
	}
	else if ([self typeForSection:section] == TYPE_REFRESH)
	{
		/* Refresh Interval */
		return 1;
	}
	else
	{
		return 0;
	}
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
	{
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:CellIdentifier] autorelease];
    }
	cell.textLabel.text = nil;
	cell.detailTextLabel.text = nil;
	cell.accessoryType = UITableViewCellAccessoryNone;
	cell.accessoryView = nil;
	
    // Configure the cell...
	if ([self typeForSection:indexPath.section] == TYPE_BASIC)
	{
		switch (indexPath.row)
		{
			case 0:
				cell.textLabel.text = @"Device Type";
				cell.detailTextLabel.text = [LTDeviceEditModuleListTableViewController descriptionForModuleName:self.devVendorModule];
				cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
				break;
			case 1:
				cell.textLabel.text = @"Description";
				cell.accessoryView = devDesc;
				break;
			case 2:
				cell.textLabel.text = @"IP Address";
				cell.accessoryView = devIPAddress;
				break;
		}
	}
	else if ([self typeForSection:indexPath.section] == TYPE_PROTOCOL_SELECT)
	{
		cell.textLabel.text = @"Monitoring Protocol";
		if (self.devProtocol == 1)
		{ cell.detailTextLabel.text = @"ServerManager"; }
		else
		{ cell.detailTextLabel.text = @"SNMP"; }
		cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
	}
	else if ([self typeForSection:indexPath.section] == TYPE_FEATURE_SELECT)
	{
		switch (indexPath.row) 
		{
			case 0:
				cell.textLabel.text = @"Use ICMP Ping Check";
				cell.accessoryView = devUseICMP;
				break;
			case 1:
				cell.textLabel.text = @"Monitor Process List";
				cell.accessoryView = devUseProcessList;
				break;
			case 3:
				cell.textLabel.text = @"Use Lights Out Management";
				cell.accessoryView = devUseLOM;
				break;
			default:
				break;
		}
	}
	else if ([self typeForSection:indexPath.section] == TYPE_SNMP)
	{
		if (indexPath.row == 0)
		{
			cell.textLabel.text = @"SNMP Version";
			if (devSNMPVersion == 2) cell.detailTextLabel.text = @"2c";
			else cell.detailTextLabel.text = [NSString stringWithFormat:@"%i", devSNMPVersion];
			cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
		}
		if ([self deviceSNMPVersion] == 3)
		{
			/* SNMP V3 handling */
			switch (indexPath.row)
			{
				case 1:
					cell.textLabel.text = @"Username";
					cell.accessoryView = devUsername;
					break;
				case 2:
					cell.textLabel.text = @"Authentication Method";
					cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
					if (devSNMPAuthMethod == 1) cell.detailTextLabel.text = @"SHA";
					else if (devSNMPAuthMethod == 2) cell.detailTextLabel.text = @"MD5";
					else cell.detailTextLabel.text = @"None";
					break;
				case 3:
					cell.textLabel.text = @"Authentication Password";
					cell.accessoryView = devSNMPAuthPassword;
					break;
				case 4:
					cell.textLabel.text = @"Privacy Method";
					cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
					if (devSNMPPrivacyMethod == 1) cell.detailTextLabel.text = @"DES";
					else cell.detailTextLabel.text = @"None";
					break;
				case 5:
					cell.textLabel.text = @"Privacy Password";
					cell.accessoryView = devSNMPPrivacyPassword;
					break;
			}
		}
		else 
		{
			/* SNMP v1/v2c Handling */
			switch (indexPath.row) {
				case 1:
					cell.textLabel.text = @"Community String";
					cell.accessoryView = devSNMPCommunity;
					break;
				default:
					break;
			}
		}
	}
	else if ([self typeForSection:indexPath.section] == TYPE_SERVERMGRD)
	{
		switch (indexPath.row) {
			case 0:
				cell.textLabel.text = @"Admin Username";
				cell.accessoryView = devUsername;
				break;
			case 1:
				cell.textLabel.text = @"Password";
				cell.accessoryView = devPassword;
				break;
			default:
				break;
		}
	}
	else if ([self typeForSection:indexPath.section] == TYPE_PASSWORDONLY)
	{
		switch (indexPath.row) {
			case 0:
				cell.textLabel.text = @"Password";
				cell.accessoryView = devPassword;
				break;
			default:
				break;
		}
	}
	else if ([self typeForSection:indexPath.section] == TYPE_REFRESH)
	{
		cell.textLabel.text = @"Refresh Interval";
		cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
		switch (devRefreshInterval) 
		{
			case 30:
				cell.detailTextLabel.text = @"30 Seconds";
				break;
			case 60:
				cell.detailTextLabel.text = @"60 Seconds";
				break;
			case 120:
				cell.detailTextLabel.text = @"2 Minutes";
				break;
			case 300:
				cell.detailTextLabel.text = @"5 Minutes";
				break;
			case 600:
				cell.detailTextLabel.text = @"10 Minutes";
				break;
			case (60 * 30):
				cell.detailTextLabel.text = @"30 Minutes";
				break;
			case (60 * 60):
				cell.detailTextLabel.text = @"1 Hour";
				break;
			default:
				cell.detailTextLabel.text = [NSString stringWithFormat:@"%i Seconds", devRefreshInterval];
				break;
		}
	}
	
	if (cell.accessoryView) cell.selectionStyle = UITableViewCellSelectionStyleNone;
    
    return cell;
}


/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/


/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source.
        [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationFade];
    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view.
    }   
}
*/


/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath {
}
*/


/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/


#pragma mark -
#pragma mark Table view delegate

- (NSIndexPath *) tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	[editingTextField resignFirstResponder];	
	return indexPath;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	if ([self typeForSection:indexPath.section] == TYPE_BASIC && indexPath.row == 0)
	{
		/* Selected Device Module Type */
		LTDeviceEditModuleListTableViewController *vc = [[LTDeviceEditModuleListTableViewController alloc] initWithCustomer:self.device.customer
																											  deviceEditTVC:self];
		[self.navigationController pushViewController:vc animated:YES];
		[vc release];
	}
	else if ([self typeForSection:indexPath.section] == TYPE_PROTOCOL_SELECT)
	{
		LTDeviceEditProtocolListTableViewController *vc = [[LTDeviceEditProtocolListTableViewController alloc] initWithEditTableViewController:self];
		[self.navigationController pushViewController:vc animated:YES];
		[vc release];
	}
	else if ([self typeForSection:indexPath.section] == TYPE_SNMP && indexPath.row == 0)
	{
		LTDeviceEditSNMPVersionListTableViewController *vc = [[LTDeviceEditSNMPVersionListTableViewController alloc] initWithEditTableViewController:self];
		[self.navigationController pushViewController:vc animated:YES];
		[vc release];
	}
	else if ([self typeForSection:indexPath.section] == TYPE_SNMP && devSNMPVersion == 3 && indexPath.row == 2)
	{
		LTDeviceEditSNMPAuthenticationListTableViewController *vc = [[LTDeviceEditSNMPAuthenticationListTableViewController alloc] initWithEditTableViewController:self];
		[self.navigationController pushViewController:vc animated:YES];
		[vc release];
	}	
	else if ([self typeForSection:indexPath.section] == TYPE_SNMP && devSNMPVersion == 3 && indexPath.row == 4)
	{
		LTDeviceEditSNMPPrivacyListTableViewController *vc = [[LTDeviceEditSNMPPrivacyListTableViewController alloc] initWithEditTableViewController:self];
		[self.navigationController pushViewController:vc animated:YES];
		[vc release];
	}	
	else if ([self typeForSection:indexPath.section] == TYPE_REFRESH)
	{
		LTDeviceEditRefreshIntervalListTableViewController *vc = [[LTDeviceEditRefreshIntervalListTableViewController alloc] initWithEditTableViewController:self];
		[self.navigationController pushViewController:vc animated:YES];
		[vc release];
	}
}

#pragma mark -
#pragma mark Text Field Delegate

- (void) textFieldDidBeginEditing:(UITextField *)textField
{
	editingTextField = textField;
}

- (void) textFieldDidEndEditing:(UITextField *)textField
{
	editingTextField = nil;	
}

- (BOOL) textFieldShouldEndEditing:(UITextField *)textField
{
	return YES;
}

#pragma mark -
#pragma mark Actions

- (void) saveTouched:(id)sender
{
	/* Check the deployment is reachable */
	if (!self.site.coreDeployment.reachable)
	{
		UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Deployment is Unreachable"
														 message:@"The Lithium Core deployment is currently unreachable, please try again later"
														delegate:nil
											   cancelButtonTitle:@"Dismiss"
											   otherButtonTitles:nil] autorelease];
		[alert show];
		return;
	}
	
	/* Update last-used NSUserDefaults */
	[[NSUserDefaults standardUserDefaults] setInteger:devRefreshInterval forKey:kLastUsedRefreshInterval];
	[[NSUserDefaults standardUserDefaults] setObject:devVendorModule forKey:kLastUsedVendorKey];
	[[NSUserDefaults standardUserDefaults] synchronize];
	
	/* Create place-holder device if needed */
	if (!self.device)
	{
		/* This is a new device, create a new live entity (we set the UUID/name)
		 * and add it to the site. 
		 */
		
		/* Create device */
		self.device = [[LTEntity new] autorelease];
		CFUUIDRef uuidRef = CFUUIDCreate(nil);
		self.device.name = (NSString *)CFUUIDCreateString(nil, uuidRef);
		CFRelease(uuidRef);			
		self.device.desc = devDesc.text;
		self.device.parent = self.site;		// Do not put in site's child list 
		self.device.customer = self.site.customer;
		self.device.coreDeployment = self.site.coreDeployment;
		[self.site.children addObject:self.device];
		[self.site.childDict setObject:self.device forKey:self.device.name];
	}

	/* Create XML */
	NSMutableString *xml = [NSMutableString stringWithFormat:@"<?xml version=\"1.0\" encoding=\"UTF-8\"?><device>"];
	[xml appendFormat:@"<name>%@</name>", self.device.name];
	[xml appendFormat:@"<desc>%@</desc>", devDesc.text];
	[xml appendFormat:@"<ip>%@</ip>", devIPAddress.text];
	[xml appendFormat:@"<lom_ip>%@</lom_ip>", devLOMIPAddress.text ? : @""];
	[xml appendFormat:@"<snmpversion>%i</snmpversion>", devSNMPVersion];
	[xml appendFormat:@"<snmpauthmethod>%i</snmpauthmethod>", devSNMPAuthMethod];
	[xml appendFormat:@"<snmpprivenc>%i</snmpprivenc>", devSNMPPrivacyMethod];
	[xml appendFormat:@"<snmpcomm>%@</snmpcomm>", devSNMPCommunity.text ? : @""];
	if (![devSNMPAuthPassword.text isEqualToString:@"********"])
	{ [xml appendFormat:@"<snmpauthpassword>%@</snmpauthpassword>", devSNMPAuthPassword.text ? : @""]; }
	if (![devSNMPPrivacyPassword.text isEqualToString:@"********"])
	{ [xml appendFormat:@"<snmpprivpassword>%@</snmpprivpassword>", devSNMPPrivacyPassword.text ? : @""]; }
	[xml appendFormat:@"<username>%@</username>", devUsername.text];
	if (![devPassword.text isEqualToString:@"********"])
	{ [xml appendFormat:@"<password>%@</password>", devPassword.text ? : @""]; }
	[xml appendFormat:@"<lom_username>%@</lom_username>", devLOMUsername.text];
	if (![devLOMPassword.text isEqualToString:@"********"])
	{ [xml appendFormat:@"<lom_password>%@</lom_password>", devLOMPassword.text ? : @""]; }
	[xml appendFormat:@"<vendor>%@</vendor>", devVendorModule];
	[xml appendFormat:@"<refresh_interval>%i</refresh_interval>", devRefreshInterval];
	[xml appendFormat:@"<protocol>%i</protocol>", devProtocol];
	[xml appendFormat:@"<icmp>%i</icmp>", devUseICMP.on ? 1 : 0];
	[xml appendFormat:@"<swrun>%i</swrun>", devUseProcessList.on ? 1 : 0];
	[xml appendFormat:@"<lom>%i</lom>", devUseLOM.on ? 1 : 0];
	if (self.site) [xml appendFormat:@"<site_name>%@</site_name>", self.site.name];
	[xml appendFormat:@"</device>"];
	
	/* Post the update via the device */
	[self.device postXmlToResource:self.device.customer.resourceAddress
					 entityAddress:self.device.entityAddress
						   xmlName:@"device_update"
							   xml:xml];
	
	/* Refresh the customer. The sequential processing of event
	 * should ensure that this new/updated device is part of the 
	 * update received from the customer.
	 */
	[self.device.customer refresh];
	
	/* Dismiss */
	[self.navigationController dismissModalViewControllerAnimated:YES];	
}

- (void) cancelTouched:(id)sender
{
	[self.navigationController dismissModalViewControllerAnimated:YES];
}

#pragma mark -
#pragma mark Memory management

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Relinquish ownership any cached data, images, etc. that aren't in use.
}

- (void)viewDidUnload {
    // Relinquish ownership of anything that can be recreated in viewDidLoad or on demand.
    // For example: self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}


@end

