//
//  LTDeviceEditProtocolListTableViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 22/12/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTDeviceEditProtocolListTableViewController.h"
#import "LTDeviceEditTableViewController.h"

@implementation LTDeviceEditProtocolListTableViewController


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
	self.navigationItem.title = @"Select Monitoring Protocol";
}

- (void)viewWillAppear:(BOOL)animated 
{
    [super viewWillAppear:animated];
	[self.tableView reloadData];
}

#pragma mark -
#pragma mark Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    // Return the number of sections.
    return 1;
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    // Return the number of rows in the section.
    return 2;
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{    
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Configure the cell...
	switch (indexPath.row) {
		case 0:
			cell.textLabel.text = @"SNMP (Recommended)";
			break;
		case 1:
			cell.textLabel.text = @"ServerManager";
			break;
		default:
			break;
	}
	if (editTVC.devProtocol == indexPath.row) cell.accessoryType = UITableViewCellAccessoryCheckmark;
	else cell.accessoryType = UITableViewCellAccessoryNone;
    
    return cell;
}

#pragma mark -
#pragma mark Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	editTVC.devProtocol = indexPath.row;
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

