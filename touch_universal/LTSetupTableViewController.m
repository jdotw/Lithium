//
//  LTSetupTableViewController.m
//  Lithium
//
//  Created by James Wilson on 29/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTSetupTableViewController.h"

#import "AppDelegate.h"
#import "LTCoreDeployment.h"
#import "LTCoreEditTableViewController.h"

@implementation LTSetupTableViewController

#pragma mark -
#pragma mark Control Creation

- (UIButton *) createButtonControl:(UIButtonType)type
{
	// create a UIButton (UIButtonTypeContactAdd)
	UIButton *button = [UIButton buttonWithType:type];
	button.frame = CGRectMake(0.0, 0.0, 25.0, 25.0);
	[button setTitle:@"Detail Disclosure" forState:UIControlStateNormal];
	button.backgroundColor = [UIColor clearColor];
	return button;
}

- (UISwitch *) createSwitchControl
{
	CGRect frame = CGRectMake(0.0, 0.0, 94.0, 27.0);
	UISwitch *switchCtl = [[[UISwitch alloc] initWithFrame:frame] autorelease];
	switchCtl.backgroundColor = [UIColor clearColor];
	return switchCtl;
}

#pragma mark View Delegates

- (void)viewDidLoad 
{
    [super viewDidLoad];
	
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(coreDeploymentArrayUpdated:)
												 name:@"CoreDeploymentAdded" 
											   object:appDelegate];	

    self.navigationItem.rightBarButtonItem = self.editButtonItem;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation 
{
	return YES;
}

#pragma mark Core List

- (void) coreDeploymentArrayUpdated:(NSNotification *)notification
{
	[[self tableView] reloadData];
}

#pragma mark -
#pragma mark Table view methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
    return 2;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
	switch (section) {
		case 0:
			return @"Lithium Core Deployments";
		case 1:
			return @"Device List";
		default:
			return nil;
			break;
	}
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	switch (section) {
		case 0:
			/* Lithium Core Deployments */
			return [appDelegate.coreDeployments count] + 1;
		case 1:
			/* Device List */
			return 1;
		default:
			return 0;
	}
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Set up the cell...
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	if (indexPath.section == 0)
	{
		/* Lithium Core Deployments */
		if ([indexPath row] < appDelegate.coreDeployments.count)
		{ 
			LTCoreDeployment *core = [appDelegate.coreDeployments objectAtIndex:[indexPath row]];
			cell.textLabel.text = core.ipAddress;
			UISwitch *switchCtl = [self createSwitchControl];
			switchCtl.on = core.enabled;
			[switchCtl addTarget:self action:@selector(coreSwitched:) forControlEvents:UIControlEventValueChanged];
			cell.accessoryView = switchCtl;
		}
		else
		{ 
			cell.textLabel.text = @"Add Lithium Core Deployment...";
			UIButton *addButton = [self createButtonControl:UIButtonTypeContactAdd];
			[addButton addTarget:self action:@selector(addNewCoreTouched:) forControlEvents:UIControlEventTouchUpInside];
			cell.accessoryView = addButton;
		}
	}
	else if (indexPath.section == 1)
	{
		/* Device List Settings */
		cell.textLabel.text = @"Group by Location";
		UISwitch *switchCtl = [self createSwitchControl];
		switchCtl.on = [[NSUserDefaults standardUserDefaults] boolForKey:kDeviceListGroupByLocation];
		[switchCtl addTarget:self action:@selector(deviceListGroupingSwitched:) forControlEvents:UIControlEventValueChanged];
		cell.accessoryView = switchCtl;
	}

    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	if ([indexPath section] == 0)
	{
		if ([indexPath row] < appDelegate.coreDeployments.count)
		{
			/* Edit Existing Core */
		}
		else
		{
			/* Add New */
			[self addNewCoreTouched:self];
		}
	}
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath 
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	if ([indexPath section] == 0)
	{
		if ([indexPath row] < appDelegate.coreDeployments.count)
		{ return YES; }
		else
		{ return NO; }
	}
	else
	{ return NO; }
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath 
{ 
    if (editingStyle == UITableViewCellEditingStyleDelete) 
	{
        // Delete the row from the data source
		AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
		LTCoreDeployment *core = [appDelegate.coreDeployments objectAtIndex:[indexPath row]];
		[appDelegate removeCore:core];
        [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:YES];		
    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) 
	{
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }
}

- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return NO;
}

#pragma mark -
#pragma mark UI Actions

- (IBAction) addNewCoreTouched:(id)sender
{
	LTCoreEditTableViewController *controller = [[LTCoreEditTableViewController alloc] initWithStyle:UITableViewStyleGrouped];
	UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:controller];
	[self.navigationController presentModalViewController:navController animated:YES];
	[controller release];
	[navController release];	
}

- (IBAction) coreSwitched:(id)sender
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	/* FIX Doesnt Do Anything! */
	[appDelegate saveCoreDeployments];	
}

- (IBAction) deviceListGroupingSwitched:(UISwitch *)switchCtl
{
	[[NSUserDefaults standardUserDefaults] setBool:switchCtl.on forKey:kDeviceListGroupByLocation];
	[[NSUserDefaults standardUserDefaults] synchronize];
	[[NSNotificationCenter defaultCenter] postNotificationName:kDeviceListGroupByLocation object:self];
}

#pragma mark -
#pragma mark Memory Management

- (void)didReceiveMemoryWarning 
{
	[super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
	// Release anything that's not essential, such as cached data
}

- (void)dealloc {
    [super dealloc];
}


@end

