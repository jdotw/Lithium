//
//  LTDeviceEditSNMPPrivacyListTableViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 22/12/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTDeviceEditSNMPPrivacyListTableViewController.h"
#import "LTDeviceEditTableViewController.h"


@implementation LTDeviceEditSNMPPrivacyListTableViewController

#pragma mark -
#pragma mark Initialization

- (id)initWithEditTableViewController:(LTDeviceEditTableViewController *)initEditTVC
{
    self = [super initWithStyle:UITableViewStyleGrouped];
    if (self) 
	{
		editTVC = [initEditTVC retain];
    }
    return self;
}

#pragma mark -
#pragma mark View lifecycle

- (void) viewDidLoad
{
	[super viewDidLoad];
	self.navigationItem.title = @"Select SNMP Privacy Method";
}

- (void)viewWillAppear:(BOOL)animated 
{
    [super viewWillAppear:animated];
	[self.tableView reloadData];
}

#pragma mark -
#pragma mark Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
    return 1;
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
    return 2;
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Configure the cell...
	switch (indexPath.row) {
		case 0:
			cell.textLabel.text = @"None";
			break;
		case 1:
			cell.textLabel.text = @"DES";
			break;
		default:
			break;
	}
	if (editTVC.devSNMPPrivacyMethod == indexPath.row) 
	{ cell.accessoryType = UITableViewCellAccessoryCheckmark; }
	else cell.accessoryType = UITableViewCellAccessoryNone;
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	editTVC.devSNMPPrivacyMethod = indexPath.row;
	[self.tableView reloadData];
}

#pragma mark -
#pragma mark Memory management

- (void)dealloc 
{
	[editTVC release];
    [super dealloc];
}

@end

