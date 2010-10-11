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

/*
- (id)initWithStyle:(UITableViewStyle)style {
    // Override initWithStyle: if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
    if (self = [super initWithStyle:style]) {
    }
    return self;
}
*/

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Overriden to allow any orientation.
    return YES;
}

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

- (void) coreDeploymentArrayUpdated:(NSNotification *)notification
{
	[[self tableView] reloadData];
}

/*
- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}
*/
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

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

#pragma mark Table view methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
    return 1;
}


// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	if (section == 0)
	{
		return [appDelegate.coreDeployments count] + 1;
	}
    else return 0;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
	if (section == 0)
	{ return @"Lithium Core Deployments"; }
	else
	{ return nil; }
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Set up the cell...
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	if ([indexPath section] == 0)
	{
		if ([indexPath row] < appDelegate.coreDeployments.count)
		{ 
			LTCoreDeployment *core = [appDelegate.coreDeployments objectAtIndex:[indexPath row]];
			cell.textLabel.text = core.ipAddress;
			UISwitch *switchCtl = [self createSwitchControl];
			switchCtl.on = core.enabled;
			cell.accessoryView = switchCtl;
		}
		else
		{ 
			cell.textLabel.text = @"Add Lithium Core Deployment...";
			cell.accessoryView = [self createContactAddButton];
		}
	}

    return cell;
}

- (UIButton *) createContactAddButton
{
	// create a UIButton (UIButtonTypeContactAdd)
	UIButton *contactAddButtonType = [UIButton buttonWithType:UIButtonTypeContactAdd];
	contactAddButtonType.frame = CGRectMake(0.0, 0.0, 25.0, 25.0);
	[contactAddButtonType setTitle:@"Detail Disclosure" forState:UIControlStateNormal];
	contactAddButtonType.backgroundColor = [UIColor clearColor];
	[contactAddButtonType addTarget:self action:@selector(addNewCoreTouched:) forControlEvents:UIControlEventTouchUpInside];
	return contactAddButtonType;
}

- (UISwitch *) createSwitchControl
{
	CGRect frame = CGRectMake(0.0, 0.0, 94.0, 27.0);
	UISwitch *switchCtl = [[[UISwitch alloc] initWithFrame:frame] autorelease];
	[switchCtl addTarget:self action:@selector(coreSwitched:) forControlEvents:UIControlEventValueChanged];
	switchCtl.backgroundColor = [UIColor clearColor];
	return switchCtl;
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
	
	[appDelegate saveCoreDeployments];	
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

- (void)dealloc {
    [super dealloc];
}


@end

