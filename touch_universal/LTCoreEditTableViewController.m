//
//  LTCoreEditTableViewController.m
//  Lithium
//
//  Created by James Wilson on 30/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTCoreEditTableViewController.h"
#import "AppDelegate.h"
#import "LTCoreDeployment.h"
#import "LTPushRegistrationRequest.h"

@implementation LTCoreEditTableViewController

/*
- (id)initWithStyle:(UITableViewStyle)style {
    // Override initWithStyle: if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
    if (self = [super initWithStyle:style]) {
    }
    return self;
}
*/

- (id) initWithCoreToEdit:(LTCoreDeployment *)initCore
{
	self = [super initWithStyle:UITableViewStyleGrouped];
	if (!self) return nil;
	
	self.editCore = initCore;
		
	return self;
}

- (void)viewDidLoad 
{
	[super viewDidLoad];

	self.title = @"Lithium Setup";
	self.navigationItem.leftBarButtonItem = [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCancel 
																							target:self action:@selector(cancel:)] autorelease];
	self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemSave 
																							 target:self action:@selector(save:)] autorelease];
	 
	addressTextField = [self createTextField];
	addressTextField.placeholder = @"lithium.yourcompany.com";
	if (self.editCore)
	{
		addressTextField.text = self.editCore.ipAddress;
	}
	
	descTextField = [self createTextField];
	descTextField.placeholder = @"Lithium";
	if (self.editCore)
	{
		descTextField.text = self.editCore.desc;
	}
	
	CGRect frame = CGRectMake(0.0, 0.0, 94.0, 27.0);
	useSSLSwitch = 	[[[UISwitch alloc] initWithFrame:frame] autorelease];
	useSSLSwitch.backgroundColor = [UIColor clearColor];
	
	self.tableView.allowsSelection = YES;
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

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

#pragma mark Table view methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
	if (self.editCore)
	{
		return 3;
	}
	else
	{
		return 1;
	}
}


// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	if (section == 0)
	{
		return 3;
	}
	else if (section == 1)
	{
		return 1; 
	}
	else if (section == 2)
	{
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
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Set up the cell...
	if ([indexPath section] == 0)
	{
		if (indexPath.row == 0)
		{
			cell.textLabel.text = @"Address";
			cell.accessoryView = addressTextField;
		}
		else if (indexPath.row == 1)
		{
			cell.textLabel.text = @"Description";
			cell.accessoryView = descTextField;
		}		
		else if (indexPath.row == 2)
		{
			cell.textLabel.text = @"Use SSL";
			cell.accessoryView = useSSLSwitch;
		}
		cell.selectionStyle = UITableViewCellSelectionStyleNone;
	}
	else if (indexPath.section == 1)
	{
		cell.textLabel.textAlignment = UITextAlignmentCenter;
		cell.textLabel.text = @"Forget Username and Password";
		cell.selectionStyle = UITableViewCellSelectionStyleBlue;
	}
	else if (indexPath.section == 2)
	{
		cell.textLabel.textAlignment = UITextAlignmentCenter;
		cell.textLabel.text = @"Reset Badge and Notifications";
		cell.selectionStyle = UITableViewCellSelectionStyleBlue;
	}
	
    return cell;
}

- (UITextField *)createTextField
{
	CGRect frame = CGRectMake(0.0, 0.0, 160, 23);
	UITextField *returnTextField = [[[UITextField alloc] initWithFrame:frame] autorelease];
    
	returnTextField.borderStyle = UITextBorderStyleNone;
    returnTextField.textColor = [UIColor blackColor];
	returnTextField.font = [UIFont systemFontOfSize:17.0];
    returnTextField.placeholder = @"<enter text>";
    returnTextField.backgroundColor = [UIColor clearColor];
	returnTextField.autocorrectionType = UITextAutocorrectionTypeNo;	// no auto correction support
	returnTextField.autocapitalizationType = UITextAutocapitalizationTypeNone;

	returnTextField.keyboardType = UIKeyboardTypeURL;
	returnTextField.returnKeyType = UIReturnKeyDefault;
	
	returnTextField.clearButtonMode = UITextFieldViewModeWhileEditing;	// has a clear 'x' button to the right
	
	return returnTextField;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	if (indexPath.section == 1 && indexPath.row == 0)
	{
		/* Clear auth credentials */
		for (LTCustomer *customer in editCore.children)
		{
			[[NSUserDefaults standardUserDefaults] removeObjectForKey:[NSString stringWithFormat:@"Username-%@-%@", customer.ipAddress, customer.name]];
			[[NSUserDefaults standardUserDefaults] removeObjectForKey:[NSString stringWithFormat:@"Password-%@-%@", customer.ipAddress, customer.name]];
		}
		
		UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Authentication Cleared"
														message:@"The saved authentication credentials have been forgotten. Next time you start Lithium Touch you will be prompted to login."
													   delegate:self
											  cancelButtonTitle:@"OK"
											 otherButtonTitles:nil] autorelease];
		[alert show];
		[tableView deselectRowAtIndexPath:indexPath animated:YES];
	}
	if (indexPath.section == 2 && indexPath.row == 0)
	{
		/* Reset badge count */
		if ([(AppDelegate *)[[UIApplication sharedApplication] delegate] pushToken])
		{
			[[[LTPushRegistrationRequest alloc] initWithCustomer:nil 
														  token:[(AppDelegate *)[[UIApplication sharedApplication] delegate] pushToken] 
										   receiveNotifications:NO] performReset];
			
		}
		UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Notifications Reset"
														message:@"The list Core Deployments that this device is bound to for Push Notifications has been reset. Next time you start Lithium Touch the deployment memberships will be re-established."
													   delegate:self
											  cancelButtonTitle:@"OK"
											  otherButtonTitles:nil] autorelease];
		[alert show];
		[tableView deselectRowAtIndexPath:indexPath animated:YES];
	}
}

- (NSIndexPath *) tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	if (indexPath.section == 1) return indexPath;
	else if (indexPath.section == 2) return indexPath;
	else return nil;
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the specified item to be editable.
    return NO;
}


/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:YES];
    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/


/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath {
}
*/


- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return NO;
}

- (IBAction) save:(id)sender
{
	[self.navigationController dismissModalViewControllerAnimated:YES];	

	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	LTCoreDeployment *core;
	if (editCore)
	{ core = editCore; }
	else
	{ core = [[LTCoreDeployment new] autorelease]; }
	core.ipAddress = [addressTextField text];
	core.enabled = YES;
	core.useSSL = useSSLSwitch.on;
	core.name = [addressTextField text];
	if ([[descTextField text] length] > 0)
	{
		core.desc = [descTextField text];
	}
	else
	{
		core.desc = [addressTextField text];
	}
	if (editCore)
	{ 
		[appDelegate saveCoreDeployments]; 
		[[NSNotificationCenter defaultCenter] postNotificationName:@"CoreDeploymentUpdated" object:core];
	}
	else
	{ [appDelegate addCore:core]; }
}

- (IBAction) cancel:(id)sender
{
	[self.navigationController dismissModalViewControllerAnimated:YES];
}

- (void)dealloc 
{
	[editCore release];
    [super dealloc];
}

@synthesize editCore;

@end

