//
//  LTActionTableViewController.m
//  Lithium
//
//  Created by James Wilson on 4/03/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTActionTableViewController.h"
#import "LTActionInfoCell.h"

@implementation LTActionTableViewController

- (id)initWithStyle:(UITableViewStyle)style {
    // Override initWithStyle: if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
    if (self = [super initWithStyle:style]) 
	{
		self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];

    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

#pragma mark Table view methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
	if (action.activationMode == 1)
	{ return 3; }
	else
	{ return 2; }
}


// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	if (section == 0)
	{
		/* Info Section */
		return 1;
	}
	else if (section == 1)
	{
		return 1;
	}
	else if (section == 2)
	{
		return 1;
	}
    return 0;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	if (indexPath.section == 0)
	{ return 174.0; }
	else
	{ return 48.0; }
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{    
	NSString *CellIdentifier = @"Default";
	if (indexPath.section == 0)
	{ CellIdentifier = @"Info"; }
	else if (indexPath.section > 0)
	{ CellIdentifier = @"Button"; }
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
	{
		if (indexPath.section == 0)
		{
			/* Create info cell */
			LTActionInfoCell *infoCell = [[[LTActionInfoCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
			cell = infoCell;
		}
		else if (indexPath.section > 0)
		{
			/* Create button cell */
			cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
		}
    }
    
    // Set up the cell...
	if (indexPath.section == 0)
	{
		/* Info */
		LTActionInfoCell *infoCell = (LTActionInfoCell *) cell;
		infoCell.action = action;
 	}
	else if (indexPath.section == 1)
	{
		/* Buttons */
		if (action.activationMode == 1)
		{ 
			if (action.rerun || action.runCount < 1)
			{ cell.textLabel.text = @"Execute Action Now"; }
			else
			{ cell.textLabel.text = @"Execute Action Again"; }
		}
		else
		{ cell.textLabel.text = @"Execute Action"; }
		cell.textLabel.font = [UIFont boldSystemFontOfSize:16.0];
		cell.textLabel.textAlignment = UITextAlignmentCenter;
	}
	else if (indexPath.section == 2)
	{
		/* Buttons */
		cell.textLabel.text = @"Cancel Pending Action";
		cell.textLabel.font = [UIFont boldSystemFontOfSize:16.0];
		cell.textLabel.textAlignment = UITextAlignmentCenter;
		if (action.runState == 1)
		{
			cell.hidden = NO;
		}
		else
		{ 
			cell.hidden = YES; 
		}
	}
	
    return cell;
}


- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	if (indexPath.section == 1 && indexPath.row == 0)
	{
		/* Execute */
		executeAlertView = [[UIAlertView alloc] initWithTitle:@"Confirm Action Execution" 
													  message:[NSString stringWithFormat:@"Action Script %@ will be executed immediately.", action.scriptFile]
													 delegate:self
											cancelButtonTitle:@"Cancel"
											otherButtonTitles:@"Execute", nil];
		[executeAlertView show];		
	}
	else if (indexPath.section == 2 && indexPath.row == 0)
	{
		/* Cancel */
		cancelAlertView = [[UIAlertView alloc] initWithTitle:@"Confirm Action Cancellation" 
													 message:[NSString stringWithFormat:@"Action will no longer run for this Incident only."]
													delegate:self
										   cancelButtonTitle:@"Cancel"
										   otherButtonTitles:@"Confirm", nil];
		[cancelAlertView show];		
	}
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

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
	if (alertView == executeAlertView && buttonIndex == 1)
	{
		/* Execute */
		[action execute];
		action.runCount++;
		[self.tableView reloadData];
	}
	else if (alertView == cancelAlertView && buttonIndex == 1)
	{
		/* Execute */
		[action cancel];
		action.runState = 0;
		[self.tableView reloadData];
		[[NSNotificationCenter defaultCenter] postNotificationName:@"ActionUpdated" object:action];
	}
	[self.tableView selectRowAtIndexPath:nil animated:NO scrollPosition:UITableViewScrollPositionNone];
}

- (void)dealloc {
    [super dealloc];
}

@synthesize action;
- (void) setAction:(LTAction *)value
{
	[action release];
	action = [value retain];
	
	self.title = @"Action";
}

@end

