//
//  LTAuthenticationTableViewController.m
//  Lithium
//
//  Created by James Wilson on 30/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTAuthenticationTableViewController.h"

#import "AppDelegate.h"

@interface LTAuthenticationTableViewController (private)

- (void) promptForAuth;

@end

@implementation LTAuthenticationTableViewController

- (id)initWithStyle:(UITableViewStyle)style 
{
    // Override initWithStyle: if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
    if (self = [super initWithStyle:style]) 
	{
    }
    return self;
}

- (void)viewDidLoad 
{
    [super viewDidLoad];

	self.navigationItem.leftBarButtonItem = [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCancel 
																						   target:self action:@selector(cancel:)] autorelease];
	self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc] initWithTitle:@"Log In" 
																			   style:UIBarButtonItemStyleDone 
																			  target:self 
																			  action:@selector(login:)] autorelease];
	self.navigationItem.rightBarButtonItem.title = @"Log In";
	self.navigationItem.prompt = @"Log In to Lithium Core";
	self.navigationController.navigationBar.tintColor = [UIColor colorWithWhite:120.0/255.0 alpha:1.0];
	usernameTextField = [self createTextField];
	usernameTextField.placeholder = @"username";
	usernameTextField.autocapitalizationType = UITextAutocapitalizationTypeNone;
	passwordTextField = [self createTextField];
	passwordTextField.placeholder = @"password";
	usernameTextField.autocapitalizationType = UITextAutocapitalizationTypeNone;
	passwordTextField.secureTextEntry = YES;
	
	rememberSwitch = [self createSwitchControl];
	rememberSwitch.on = YES;
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

- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

#pragma mark Table view methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
    return 2;
}


// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	if (section == 0) return 2;
    else return 1;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
	return nil;
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Set up the cell...
	if ([indexPath section] == 0)
	{
		if ([indexPath row] == 0)
		{
			/* Username */
			cell.textLabel.text = @"Username";
			cell.accessoryView = usernameTextField;
		}
		else
		{
			/* Password */
			cell.textLabel.text = @"Password";
			cell.accessoryView = passwordTextField;
		}
	}
	else if ([indexPath section] == 1)
	{
		cell.textLabel.text = @"Remember Password";
		cell.accessoryView = rememberSwitch;
	}
	
	cell.selectionStyle = UITableViewCellSelectionStyleNone;

    return cell;
}

- (UITextField *)createTextField
{
	CGRect frame = CGRectMake(0.0, 0.0, 280, 23);
	UITextField *returnTextField = [[UITextField alloc] initWithFrame:frame];
    
	returnTextField.borderStyle = UITextBorderStyleNone;
    returnTextField.textColor = [UIColor blackColor];
	returnTextField.font = [UIFont systemFontOfSize:17.0];
    returnTextField.placeholder = @"<enter text>";
    returnTextField.backgroundColor = [UIColor clearColor];
	returnTextField.autocorrectionType = UITextAutocorrectionTypeNo;	// no auto correction support
	
	returnTextField.keyboardType = UIKeyboardTypeDefault;
	returnTextField.returnKeyType = UIReturnKeyDefault;
	
	returnTextField.clearButtonMode = UITextFieldViewModeWhileEditing;	// has a clear 'x' button to the right
	
	return returnTextField;
}

- (UISwitch *) createSwitchControl
{
	CGRect frame = CGRectMake(0.0, 0.0, 94.0, 27.0);
	UISwitch *switchCtl = [[[UISwitch alloc] initWithFrame:frame] autorelease];
//	[switchCtl addTarget:self action:@selector(rememberSwitched:) forControlEvents:UIControlEventValueChanged];
	switchCtl.backgroundColor = [UIColor clearColor];
	return switchCtl;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
    // Navigation logic may go here. Create and push another view controller.
	// AnotherViewController *anotherViewController = [[AnotherViewController alloc] initWithNibName:@"AnotherView" bundle:nil];
	// [self.navigationController pushViewController:anotherViewController];
	// [anotherViewController release];
}


/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/


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


/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

- (void) login:(id)sender
{
	NSMutableArray *completed = [NSMutableArray array];
	for (NSMutableDictionary *connDict in connections)
	{
		LTEntity *customer = [connDict objectForKey:@"customer"];
		if (customer != curCustomer)
		{ continue; }
		
		if (rememberSwitch.on)
		{
			[[NSUserDefaults standardUserDefaults] setObject:usernameTextField.text forKey:[NSString stringWithFormat:@"Username-%@-%@", customer.ipAddress, customer.name]];
			[[NSUserDefaults standardUserDefaults] setObject:passwordTextField.text forKey:[NSString stringWithFormat:@"Password-%@-%@", customer.ipAddress, customer.name]];	
			[[NSUserDefaults standardUserDefaults] synchronize];
		}
		
		NSURLAuthenticationChallenge *challenge = [connDict objectForKey:@"challenge"];
		NSURLCredential *cred = [NSURLCredential credentialWithUser:usernameTextField.text
														   password:passwordTextField.text
														persistence:NSURLCredentialPersistenceForSession];
		
		[[challenge sender] useCredential:cred forAuthenticationChallenge:challenge];
		
		[completed addObject:connDict];
	}
	[connections removeObjectsInArray:completed];
	
	[self.navigationController dismissModalViewControllerAnimated:YES];
	
	if ([connections count] > 0)
	{
		[self performNextRequest];
	}
	else
	{
		[connections release];
		connections = nil;
		authRequestInProgress = NO;
	}
}

- (void) cancel:(id)sender
{
	NSMutableArray *completed = [NSMutableArray array];
	for (NSMutableDictionary *connDict in connections)
	{
		LTEntity *entity = [connDict objectForKey:@"entity"];
		LTEntity *customer = [connDict objectForKey:@"customer"];
		if (customer != curCustomer)
		{ continue; }
		
		[self forgetCredentialsForCustomer:customer];
		
		NSURLConnection *connection = [connDict objectForKey:@"connection"];
		[connection cancel];
		[entity connection:connection didFailWithError:nil];
		
		[completed addObject:connDict];
	}
	[connections removeObjectsInArray:completed];

	[self.navigationController dismissModalViewControllerAnimated:YES];
	
	if ([connections count] > 0)
	{
		[self performNextRequest];
	}
	else
	{
		[connections release];
		connections = nil;
		authRequestInProgress = NO;
	}
	
}

- (void) forgetCredentialsForCustomer:(LTEntity *)forgetCustomer
{
	[[NSUserDefaults standardUserDefaults] removeObjectForKey:[NSString stringWithFormat:@"Username-%@-%@", forgetCustomer.ipAddress, forgetCustomer.name]];
	[[NSUserDefaults standardUserDefaults] removeObjectForKey:[NSString stringWithFormat:@"Password-%@-%@", forgetCustomer.ipAddress, forgetCustomer.name]];	
}

- (void) requestAuthForCustomer:(LTEntity *)customer 
						 entity:(LTEntity *)entity 
					  challenge:(NSURLAuthenticationChallenge *)challenge 
					 connection:(NSURLConnection *)connection 
		   previousFailureCount:(int)previousFailureCount
{
	if (!connections)
	{
		connections = [[NSMutableArray array] retain];
	}
	
	NSMutableDictionary *connDict = [NSMutableDictionary dictionary];
	[connDict setObject:customer forKey:@"customer"];
	[connDict setObject:entity forKey:@"entity"];
	[connDict setObject:challenge forKey:@"challenge"];
	[connDict setObject:connection forKey:@"connection"];
	[connDict setObject:[NSNumber numberWithInt:previousFailureCount] forKey:@"previousFailureCount"];
	[connections addObject:connDict];
	
	if (!authRequestInProgress)
	{
		[self performNextRequest];
	}
}

- (void) performNextRequest
{
	NSMutableDictionary *connDict = [connections objectAtIndex:0];
	LTEntity *customer = [connDict objectForKey:@"customer"];
	int previousFailureCount = [[connDict objectForKey:@"previousFailureCount"] intValue];

	curCustomer = (LTCustomer *) customer;
	
	authRequestInProgress = YES;
	if (customer.desc) self.title = customer.desc;
	else self.title = customer.name;
	
	/* Check for saved credential */
	NSString *remUsername = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithFormat:@"Username-%@-%@", curCustomer.ipAddress, curCustomer.name]];
	NSString *remPassword = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithFormat:@"Password-%@-%@", curCustomer.ipAddress, curCustomer.name]];	
#ifdef DEMO
	if ([curCustomer.name isEqualToString:@"shopkeep"]) 
	{
		remUsername = @"admin";
		remPassword = @"lithium";
	}
#endif
	if ([remUsername length] > 0 && [remPassword length] > 0 && previousFailureCount == 0)
	{
		/* User remembered credentials */
		NSURLAuthenticationChallenge *challenge = [connDict objectForKey:@"challenge"];
		NSURLCredential *cred = [NSURLCredential credentialWithUser:remUsername
														   password:remPassword
														persistence:NSURLCredentialPersistenceForSession];
		
		[connections removeObjectAtIndex:0];

		[[challenge sender] useCredential:cred forAuthenticationChallenge:challenge];
		
		if ([connections count] > 0)
		{ 
			[self performNextRequest]; 
		}
		else
		{ 
			authRequestInProgress = NO;
			[connections release];
			connections = nil;
		}
	}
	else
	{
		/* Prompt for Auth */
		[self promptForAuth];
	}
}

- (void) promptForAuth
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	if ([appDelegate.tabBarController modalViewController])
	{ 
		[NSTimer scheduledTimerWithTimeInterval:0.5
										 target:self
									   selector:@selector(promptForAuth)
									   userInfo:nil
										repeats:NO];
	}
	else
	{
		UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:self];
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 30200
			navController.modalPresentationStyle = UIModalPresentationFormSheet;
#endif
		[appDelegate.tabBarController presentModalViewController:navController animated:YES];
		[navController autorelease];	
	}
}

- (void)dealloc 
{
	[connections release];
	[usernameTextField release];
	[passwordTextField release];
	
    [super dealloc];
}

@end

