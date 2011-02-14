//
//  LTDeviceEditRefreshIntervalListTableViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 22/12/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTDeviceEditRefreshIntervalListTableViewController.h"
#import "LTDeviceEditTableViewController.h"


@implementation LTDeviceEditRefreshIntervalListTableViewController

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
	self.navigationItem.title = @"Select Refresh Interval";
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
    return 7;
}

- (NSInteger) refreshIntervalForRowAtIndexPath:(NSIndexPath *)indexPath
{
	int interval = 30;
	switch (indexPath.row) {
		case 0:
			interval = 30;
			break;
		case 1:
			interval = 60;
			break;
		case 2:
			interval = 120;
			break;
		case 3:
			interval = 300;
			break;
		case 4:
			interval = 60 * 10;
			break;
		case 5:
			interval = 60 * 30;
			break;
		case 6:
			interval = 60 * 60;
			break;
		default:
			break;
	}	
	return interval;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Configure the cell...
	switch (indexPath.row) {
		case 0:
			cell.textLabel.text = @"30 Seconds";
			break;
		case 1:
			cell.textLabel.text = @"60 Seconds";
			break;
		case 2:
			cell.textLabel.text = @"2 Minutes";
			break;
		case 3:
			cell.textLabel.text = @"5 Minutes";
			break;
		case 4:
			cell.textLabel.text = @"10 Minutes";
			break;
		case 5:
			cell.textLabel.text = @"30 Minutes";
			break;
		case 6:
			cell.textLabel.text = @"1 Hour";
			break;
		default:
			break;
	}
	if (editTVC.devRefreshInterval == [self refreshIntervalForRowAtIndexPath:indexPath])
	{ cell.accessoryType = UITableViewCellAccessoryCheckmark; }
	else 
	{ cell.accessoryType = UITableViewCellAccessoryNone; }
    
    return cell;
}

- (NSString *) tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section
{
	if (section == 0)
	{
		return @"Longer refresh intervals also use longer timeout values.";
	}
	else 
	{
		return nil;
	}
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	editTVC.devRefreshInterval = [self refreshIntervalForRowAtIndexPath:indexPath];
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

