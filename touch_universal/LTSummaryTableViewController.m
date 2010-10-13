//
//  LTSummaryTableViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 11/10/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTSummaryTableViewController.h"
#import "LTIconTableViewCell.h"
#import "LTIconView.h"

#define ICON_COUNT 1340

@implementation LTSummaryTableViewController


#pragma mark -
#pragma mark View lifecycle

- (void) awakeFromNib
{
	[super awakeFromNib];
}


/*
- (void)viewDidLoad {
    [super viewDidLoad];

    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
 
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
}
*/

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


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Override to allow orientations other than the default portrait orientation.
    return YES;
}


#pragma mark -
#pragma mark Table view data source

#define DATA_INCIDENTS 1
#define DATA_FAVORITES 2
#define DATA_DEVICES 3

- (int) dataTypeInSection:(NSInteger)section
{
	if (section == 0)
	{
		if ([incidents count] > 0) return DATA_INCIDENTS;
		else if ([favorites count] > 0) return DATA_FAVORITES;
		else return DATA_DEVICES;
	}
	else if (section == 1)
	{
		if ([favorites count] > 0) return DATA_FAVORITES;
		else return DATA_DEVICES;
	}
	else
	{
		return DATA_DEVICES;
	}
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
	int sections = 1;
	if ([incidents count] > 0) sections++;
	if ([favorites count] > 0) sections++;
    return sections;
}

- (NSString *) tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
	switch ([self dataTypeInSection:section]) {
		case DATA_INCIDENTS:
			return @"Incidents";
		case DATA_FAVORITES:
			return @"Favorites";
		case DATA_DEVICES:
			return @"All Devices";
		default:
			return nil;
	}
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	switch (section) {
		case 0:
			return 0;
		default:
			return (ICON_COUNT / [LTIconTableViewCell itemsPerRowAtHeight:self.rowHeight width:self.tableView.contentSize.width]);
	}
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{ 
    static NSString *CellIdentifier = @"Cell";
    
    LTIconTableViewCell *cell = (LTIconTableViewCell *) [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[LTIconTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Configure the cell...
	[cell removeAllSubviews];
	int i;
	for (i=0; i < [LTIconTableViewCell itemsPerRowAtHeight:self.rowHeight width:self.tableView.contentSize.width]; i++)
	{
		LTIconView *iconView = [[LTIconView alloc] initWithFrame:CGRectZero];
		[cell addSubview:iconView];
		[iconView release];
	}	
	[cell setNeedsLayout];
    
    return cell;
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
        [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationFade];
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


#pragma mark -
#pragma mark Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    // Navigation logic may go here. Create and push another view controller.
	/*
	 <#DetailViewController#> *detailViewController = [[<#DetailViewController#> alloc] initWithNibName:@"<#Nib name#>" bundle:nil];
     // ...
     // Pass the selected object to the new view controller.
	 [self.navigationController pushViewController:detailViewController animated:YES];
	 [detailViewController release];
	 */
}


#pragma mark -
#pragma mark Memory management

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Relinquish ownership any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
    // Relinquish ownership of anything that can be recreated in viewDidLoad or on demand.
    // For example: self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}


@end

