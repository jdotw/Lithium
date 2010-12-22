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
#pragma mark Class Methods

static NSMutableDictionary *staticModuleDescDict = nil;

+ (NSString *) descriptionForModuleName:(NSString *)name
{
	if (!staticModuleDescDict)
	{
		staticModuleDescDict = [[NSMutableDictionary dictionary] retain];
		[staticModuleDescDict setObject:@"Xserve (Intel)" forKey:@"xserve_intel"];
		[staticModuleDescDict setObject:@"Xserve (G5)" forKey:@"xserve_g5"];
		[staticModuleDescDict setObject:@"Non-Xserve Mac OS X Server" forKey:@"osx_server"];
		[staticModuleDescDict setObject:@"Mac OS X Client" forKey:@"osx_client"];
		[staticModuleDescDict setObject:@"Linux/UNIX with Net-SNMP Daemon" forKey:@"netsnmp"];
		[staticModuleDescDict setObject:@"Windows" forKey:@"windows"];
		[staticModuleDescDict setObject:@"Promise VTrak" forKey:@"vtrak"];
		[staticModuleDescDict setObject:@"Xserve RAID" forKey:@"xraid"];
		[staticModuleDescDict setObject:@"Active Storage" forKey:@"activestorage"];
		[staticModuleDescDict setObject:@"Infortrend Controller" forKey:@"infortrend"];
		[staticModuleDescDict setObject:@"NetApp Filer" forKey:@"filer"];
		[staticModuleDescDict setObject:@"Qlogic Fibre Channel Switch" forKey:@"qlogic"];
		[staticModuleDescDict setObject:@"3Com Switch" forKey:@"3com"];
		[staticModuleDescDict setObject:@"Allied Telesyn Switch" forKey:@"alliedtelesyn"];
		[staticModuleDescDict setObject:@"Cisco Router/Switch/Firewall" forKey:@"cisco"];
		[staticModuleDescDict setObject:@"Foundry Load Balancer" forKey:@"foundry"];
		[staticModuleDescDict setObject:@"HP Switch" forKey:@"hp"];
		[staticModuleDescDict setObject:@"Airport Access Point" forKey:@"airport"];
		[staticModuleDescDict setObject:@"APC UPS" forKey:@"apcups"];
		[staticModuleDescDict setObject:@"MGE UPS" forKey:@"mgeups"];
		[staticModuleDescDict setObject:@"NetBotz Environmental Monitor" forKey:@"netbotz"];
		[staticModuleDescDict setObject:@"Liebert Airconditioner" forKey:@"liebert"];
		[staticModuleDescDict setObject:@"Basic SNMP" forKey:@"snmpbasic"];
		[staticModuleDescDict setObject:@"Extended SNMP" forKey:@"snmpadv"];
		[staticModuleDescDict setObject:@"Printer" forKey:@"printer"];
		[staticModuleDescDict setObject:@"ICMP Ping Only" forKey:@"icmp"];
	}
	if ([staticModuleDescDict objectForKey:name]) return [staticModuleDescDict objectForKey:name];
	else return name;
}
		

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
    }    return self;
}

- (void) viewDidLoad
{
	[super viewDidLoad];
	self.navigationItem.title = @"Select Device Module";
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
	switch (indexPath.section) {
		case 0:
			/* Server or Workstation */
			switch (indexPath.row) {
				case 0:
					vendorModuleName = @"xserve_intel";
					break;
				case 1:
					vendorModuleName = @"xserve_g5";
					break;
				case 2:
					vendorModuleName = @"osx_server";
					break;
				case 3:
					vendorModuleName = @"osx_client";
					break;
				case 4:
					vendorModuleName = @"netsnmp";
					break;
				case 5:
					vendorModuleName = @"windows";
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
					break;
				case 1:
					vendorModuleName = @"xraid";
					break;
				case 2:
					vendorModuleName = @"activestorage";
					break;
				case 3:
					vendorModuleName = @"infortrend";
					break;
				case 4:
					vendorModuleName = @"filer";
					break;
				case 5:
					vendorModuleName = @"qlogic";
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
					break;
				case 1:
					vendorModuleName = @"alliedtelesyn";
					break;
				case 2:
					vendorModuleName = @"cisco";
					break;
				case 3:
					vendorModuleName = @"foundry";
					break;
				case 4:
					vendorModuleName = @"hp";
					break;
				case 5:
					vendorModuleName = @"airport";
					break;
				case 6:
					vendorModuleName = @"apcups";
					break;
				case 7:
					vendorModuleName = @"mgeups";
					break;
				case 8:
					vendorModuleName = @"netbotz";
					break;
				case 9:
					vendorModuleName = @"liebert";
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
					break;
				case 1:
					vendorModuleName = @"snmpadv";
					break;
				case 2:
					vendorModuleName = @"printer";
					break;
				case 3:
					vendorModuleName = @"icmp";
					break;
				default:
					break;
			}
			
			break;
		case 4:
			/* Custom */
			vendorModuleName = [customModuleKeys objectAtIndex:indexPath.row];
			NSLog (@"vendorModuleName=%@ editTVC.devVendorModule=%@", vendorModuleName, editTVC.devVendorModule);			
			break;
		default:
			break;
	}
	
	NSMutableDictionary *moduleDict = [NSMutableDictionary dictionary];
	[moduleDict setObject:vendorModuleName 
				   forKey:@"name"];
	[moduleDict setObject:[LTDeviceEditModuleListTableViewController descriptionForModuleName:vendorModuleName] 
				   forKey:@"desc"];
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

