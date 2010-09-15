//
//  LTMetricValuesTableViewController.m
//  Lithium
//
//  Created by James Wilson on 21/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTMetricValuesTableViewController.h"
#import "LTMetricValue.h"

@implementation LTMetricValuesTableViewController

/*
- (id)initWithStyle:(UITableViewStyle)style {
    // Override initWithStyle: if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
    if (self = [super initWithStyle:style]) {
    }
    return self;
}
*/

- (void)viewDidLoad 
{
    [super viewDidLoad];
	self.title = @"Recent Values";

    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
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

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
	if (section == 0) return @"Recent Polled Values";
	else if (section == 1) return @"Recent Recorded Values";
	else return @"";
}

// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	if (section == 0)
	{ return metric.values.count; }
	else
	{ return historyList.values.count; }
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{    
    static NSString *CellIdentifier = @"Cell";
	
	LTMetricValue *value;
	if (indexPath.section == 0)
	{
		value = [metric.values objectAtIndex:indexPath.row];
	}
	else
	{
		value = [historyList.values objectAtIndex:indexPath.row];
	}
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
	{
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue2 reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Set up the cell...
	NSDateFormatter *dateFormatter = [[[NSDateFormatter alloc] init]  autorelease];
	[dateFormatter setDateFormat:@"MMM-d HH:mm:ss"];
	cell.textLabel.text = [NSString stringWithFormat:@"%@", [dateFormatter stringFromDate:value.timestamp]];
	if (indexPath.section == 0)
	{
		cell.detailTextLabel.text = value.stringValue;
	}
	else
	{
		NSString *valueString = [NSString stringWithFormat:@"%.2f", value.avgValue];
		if ([valueString isEqualToString:@"nan"]) 
		{ cell.detailTextLabel.text = [NSString stringWithFormat:@"---"]; }
		else
		{ cell.detailTextLabel.text = valueString; }
	}

    return cell;
}

- (NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	return nil;
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


- (void)dealloc 
{
	[metric release];
	[historyList release];
    [super dealloc];
}

- (void) historyListRefreshFinished:(NSNotification *)notification
{
	[[self tableView] reloadData];
}

@synthesize metric;
- (void) setMetric:(LTEntity *)value
{
	[metric release];
	metric = [value retain];
	
	historyList = [[LTMetricHistoryList alloc] init];
	historyList.metric = metric; 
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(historyListRefreshFinished:)
												 name:@"HistoryListRefreshFinished"
											   object:historyList];
	[historyList refresh];
	
	[[self tableView] reloadData];
}

@end

