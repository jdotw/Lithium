//
//  LTDeviceEditSNMPVersionListTableViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 22/12/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTDeviceEditSNMPVersionListTableViewController.h"
#import "LTDeviceEditTableViewController.h"


@implementation LTDeviceEditSNMPVersionListTableViewController

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
	self.navigationItem.title = @"Select SNMP Version";
}

- (void)viewWillAppear:(BOOL)animated 
{
    [super viewWillAppear:animated];
	[self.tableView reloadData];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation 
{
	return YES;
}
#pragma mark -
#pragma mark Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
    return 1;
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
    return 3;
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
			cell.textLabel.text = @"Version 1";
			break;
		case 1:
			cell.textLabel.text = @"Version 2c";
			break;
		case 2:
			cell.textLabel.text = @"Version 3";
			break;
		default:
			break;
	}
	if (editTVC.devSNMPVersion == (indexPath.row+1)) cell.accessoryType = UITableViewCellAccessoryCheckmark;
	else cell.accessoryType = UITableViewCellAccessoryNone;
    
    return cell;
}

- (NSString *) tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section
{
	if (section == 0)
	{
		return @"Version 2c is recommended.";
	}
	else 
	{
		return nil;
	}
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	editTVC.devSNMPVersion = indexPath.row+1;
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

