//
//  LTDeviceEditModuleListTableViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 21/12/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTDeviceEditModuleListTableViewController.h"
#import "LTEntity.h"
#import "LTCustomer.h"
#import "LTDeviceEditTableViewController.h"

@implementation LTDeviceEditModuleListTableViewController


#pragma mark -
#pragma mark Initialization

- (id)initWithCustomer:(LTCustomer *)initCustomer deviceEditTVC:(LTDeviceEditTableViewController *)initEditTVC
{
    self = [super initWithStyle:UITableViewStyleGrouped];
    if (self) 
	{
		customer = [initCustomer retain];
		editTVC = [initEditTVC retain];
		customModuleKeys = [[customer.customModules allKeys] copy];
    }
    return self;
}

#pragma mark -
#pragma mark Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
	return 5;
}

- (NSInteger)customModuleSectionInTableView:(UITableView *)tableView
{
	return [self numberOfSectionsInTableView:tableView]-1;
}

- (NSString *) tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
	switch (section) {
		case 0:
			return @"Server or Workstation";
		case 1:
			return @"Storage";
		case 2:
			return @"Network Device";
		case 3:
			return @"Generic";
		case 4:
			return @"Custom";
		default:
			return nil;
	}
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    // Return the number of rows in the section.
	switch (section) {
		case 0:
			return 6;
		case 1:
			return 6;
		case 2:
			return 10;
		case 3:
			return 4;
		case 4:
			return customModuleKeys.count;
		default:
			return 0;
	}
}

- (NSDictionary *) moduleDictionaryForRowAtIndexPath:(NSIndexPath *)indexPath
{
	NSString *vendorModuleName = nil;
	NSString *vendorModuleDesc = nil;
	switch (indexPath.section) {
		case 0:
			/* Server or Workstation */
			switch (indexPath.row) {
				case 0:
					vendorModuleName = @"xserve_intel";
					vendorModuleDesc = @"Xserve (Intel)";
					break;
				case 1:
					vendorModuleName = @"xserve_g5";
					vendorModuleDesc = @"Xserve (G5)";
					break;
				case 2:
					vendorModuleName = @"osx_server";
					vendorModuleDesc = @"Non-Xserve Mac OS X Server";
					break;
				case 3:
					vendorModuleName = @"osx_client";
					vendorModuleDesc = @"Mac OS X Client";
					break;
				case 4:
					vendorModuleName = @"netsnmp";
					vendorModuleDesc = @"Linux/UNIX with Net-SNMP Daemon";
					break;
				case 5:
					vendorModuleName = @"windows";
					vendorModuleDesc = @"Windows";
					break;
				default:
					break;
			}
			break;
		case 1:
			/* Storage */
			switch (indexPath.row) {
				case 0:
					vendorModuleName = @"vtrak";
					vendorModuleDesc = @"Promise VTrak";
					break;
				case 1:
					vendorModuleName = @"xraid";
					vendorModuleDesc = @"Xserve RAID";
					break;
				case 2:
					vendorModuleName = @"activestorage";
					vendorModuleDesc = @"Active Storage";
					break;
				case 3:
					vendorModuleName = @"infortrend";
					vendorModuleDesc = @"Infortrend Controller";
					break;
				case 4:
					vendorModuleName = @"filer";
					vendorModuleDesc = @"NetApp Filer";
					break;
				case 5:
					vendorModuleName = @"qlogic";
					vendorModuleDesc = @"Qlogic Fibre Channel Switch";
					break;
				default:
					break;
			}
			
			break;
		case 2:
			/* Network Device */
			switch (indexPath.row) {
				case 0:
					vendorModuleName = @"3com";
					vendorModuleDesc = @"3Com Switch";
					break;
				case 1:
					vendorModuleName = @"alliedtelesyn";
					vendorModuleDesc = @"Allied Telesyn Switch";
					break;
				case 2:
					vendorModuleName = @"cisco";
					vendorModuleDesc = @"Cisco Router/Switch/Firewall";
					break;
				case 3:
					vendorModuleName = @"foundry";
					vendorModuleDesc = @"Foundry Load Balancer";
					break;
				case 4:
					vendorModuleName = @"hp";
					vendorModuleDesc = @"HP Switch";
					break;
				case 5:
					vendorModuleName = @"airport";
					vendorModuleDesc = @"Airport Access Point";
					break;
				case 6:
					vendorModuleName = @"apcups";
					vendorModuleDesc = @"APC UPS";
					break;
				case 7:
					vendorModuleName = @"mgeups";
					vendorModuleDesc = @"MGE UPS";
					break;
				case 8:
					vendorModuleName = @"netbotz";
					vendorModuleDesc = @"NetBotz Environmental Monitor";
					break;
				case 9:
					vendorModuleName = @"liebert";
					vendorModuleDesc = @"Liebert Airconditioner";
					break;
				default:
					break;
			}
			
			break;
		case 3:
			/* Generic */
			switch (indexPath.row) {
				case 0:
					vendorModuleName = @"snmpbasic";
					vendorModuleDesc = @"Basic SNMP";
					break;
				case 1:
					vendorModuleName = @"snmpadv";
					vendorModuleDesc = @"Extended SNMP";
					break;
				case 2:
					vendorModuleName = @"printer";
					vendorModuleDesc = @"Printer";
					break;
				case 3:
					vendorModuleName = @"icmp";
					vendorModuleDesc = @"ICMP Ping Only";
					break;
				default:
					break;
			}
			
			break;
		case 4:
			/* Custom */
			vendorModuleName = [customModuleKeys objectAtIndex:indexPath.row];
			vendorModuleDesc = [customer.customModules objectForKey:vendorModuleName];
			NSLog (@"vendorModuleName=%@ editTVC.devVendorModule=%@", vendorModuleName, editTVC.devVendorModule);			
			break;
		default:
			break;
	}
	
	NSMutableDictionary *moduleDict = [NSMutableDictionary dictionary];
	if (vendorModuleName) [moduleDict setObject:vendorModuleName forKey:@"name"];
	if (vendorModuleDesc) [moduleDict setObject:vendorModuleDesc forKey:@"desc"];
	return moduleDict;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{    
    static NSString *StandardCellIdentifier = @"Standard";
    static NSString *CustomCellIdentifier = @"Custom";
	
	NSString *CellIdentifier;
	if (indexPath.section == [self customModuleSectionInTableView:tableView]) CellIdentifier = CustomCellIdentifier;
	else CellIdentifier = StandardCellIdentifier;
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
	{
		UITableViewCellStyle style = UITableViewCellStyleDefault;
		if (indexPath.section == [self customModuleSectionInTableView:tableView]) style = UITableViewCellStyleSubtitle;
        cell = [[[UITableViewCell alloc] initWithStyle:style reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Configure the cell...
	NSDictionary *moduleDict = [self moduleDictionaryForRowAtIndexPath:indexPath];
	cell.textLabel.text = [moduleDict objectForKey:@"desc"];
	cell.detailTextLabel.text = [moduleDict objectForKey:@"name"];
	if ([editTVC.devVendorModule isEqualToString:[moduleDict objectForKey:@"name"]]) 
	{ cell.accessoryType = UITableViewCellAccessoryCheckmark; }
	else 
	{ cell.accessoryType = UITableViewCellAccessoryNone; }
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	editTVC.devVendorModule = [[self moduleDictionaryForRowAtIndexPath:indexPath] objectForKey:@"name"];
	[self.tableView reloadData];
}


#pragma mark -
#pragma mark Memory management

- (void)dealloc {
	[customer release];
	[editTVC release];
	[customModuleKeys release];
    [super dealloc];
}


@end

