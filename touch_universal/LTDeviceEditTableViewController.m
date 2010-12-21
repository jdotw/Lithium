//
//  LTDeviceEditTableViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 21/12/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTDeviceEditTableViewController.h"
#import "LTEntity.h"

@implementation LTDeviceEditTableViewController

@synthesize device, site;
@synthesize devSNMPVersion, devSNMPAuthMethod, devSNMPPrivacyMethod, devVendorModule;
@synthesize devRefreshInterval, devProtocol;

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
	
	returnTextField.keyboardType = UIKeyboardTypeDefault;
	returnTextField.returnKeyType = UIReturnKeyDefault;
	
	returnTextField.clearButtonMode = UITextFieldViewModeWhileEditing;	// has a clear 'x' button to the right
	return returnTextField;
}

- (UISwitch *) propertySwitch
{
	UISwitch *switchCtl = [[[UISwitch alloc] initWithFrame:CGRectMake(0.0, 0.0, 94.0, 27.0)] autorelease];
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
		devIPAddress = [self propertyTextField];
		devLOMPassword = [self propertyTextField];
		devSNMPCommunity = [self propertyTextField];
		devSNMPAuthPassword = [self propertyTextField];
		devSNMPPrivacyPassword = [self propertyTextField];
		devUsername = [self propertyTextField];
		devPassword = [self propertyTextField];
		devLOMUsername = [self propertyTextField];
		devLOMPassword = [self propertyTextField];
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
/*
- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
}
*/
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
	
    // Configure the cell...
	if ([self typeForSection:indexPath.section] == TYPE_BASIC)
	{
		switch (indexPath.row)
		{
			case 0:
				cell.textLabel.text = @"Device Type";
				cell.detailTextLabel.text = self.devVendorModule;
				NSLog (@"Detail color = %@", cell.detailTextLabel.textColor);
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
					cell.textLabel.text = @"Authentication";
					cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
					break;
				case 3:
					cell.textLabel.text = @"Authentication Password";
					cell.accessoryView = devSNMPAuthPassword;
					break;
				case 4:
					cell.textLabel.text = @"Privacy";
					cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
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
		cell.detailTextLabel.text = [NSString stringWithFormat:@"%i", devRefreshInterval];
		cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
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

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    // Navigation logic may go here. Create and push another view controller.
    /*
    <#DetailViewController#> *detailViewController = [[<#DetailViewController#> alloc] initWithNibName:@"<#Nib name#>" bundle:nil];
     // ...
     // Pass the selected object to the new view controller.
    [self.navigationController pushViewController:detailViewController animated:YES];
    [detailViewController release];
    */
}

#pragma mark -
#pragma mark Actions

- (void) saveTouched:(id)sender
{
	
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

