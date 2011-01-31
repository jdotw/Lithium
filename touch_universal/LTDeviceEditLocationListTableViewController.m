//
//  LTDeviceEditLocationListTableViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 31/01/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTDeviceEditLocationListTableViewController.h"
#import "LTCustomer.h"
#import "LTDeviceEditTableViewController.h"

@implementation LTDeviceEditLocationListTableViewController

@synthesize customer;

#pragma mark -
#pragma mark Initialization

- (id)initWithCustomer:(LTCustomer *)initCustomer editTableViewController:(LTDeviceEditTableViewController *)initEditTVC
{
    // Override initWithStyle: if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
    self = [super initWithStyle:UITableViewStyleGrouped];
    if (self) 
	{
		self.customer = initCustomer;
		editTVC = [initEditTVC retain];
    }
    return self;
}

#pragma mark -
#pragma mark View lifecycle

- (void)viewDidLoad 
{
    [super viewDidLoad];
	self.title = @"Select Location";
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation 
{
	return YES;
}

#pragma mark -
#pragma mark Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
    // Return the number of sections.
    return 1;
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
    // Return the number of rows in the section.
    return self.customer.children.count;
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
	LTEntity *rowSite = [self.customer.children objectAtIndex:indexPath.row];
	cell.textLabel.text = rowSite.desc;
	if (rowSite == editTVC.site) cell.accessoryType = UITableViewCellAccessoryCheckmark;
	else cell.accessoryType = UITableViewCellAccessoryNone;
    
    return cell;
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath 
{
    // Return NO if you do not want the specified item to be editable.
    return NO;
}

#pragma mark -
#pragma mark Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	LTEntity *rowSite = [self.customer.children objectAtIndex:indexPath.row];
	editTVC.site = rowSite;
	[self.tableView reloadData];
	[self.navigationController popViewControllerAnimated:YES];
}

#pragma mark -
#pragma mark Memory management

- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];
}

- (void)viewDidUnload 
{
}

- (void)dealloc 
{
	[editTVC release];
    [super dealloc];
}

@end

