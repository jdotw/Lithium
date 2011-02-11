//
//  LTTableViewController.m
//  Lithium
//
//  Created by James Wilson on 2/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTTableViewController.h"
#import "AppDelegate.h"

#import "LTTableViewSectionHeaderView.h"
#import "LTEntityRefreshProgressViewCell.h"

@implementation LTTableViewController

#pragma mark -
#pragma mark View Delegates

- (void)viewWillAppear:(BOOL)animated 
{
    [super viewWillAppear:animated];
	isVisible = YES;
}

- (void)viewWillDisappear:(BOOL)animated 
{
	[super viewWillDisappear:animated];
	isVisible = NO;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    return YES;
}

- (void) viewDidUnload
{
    _refreshHeaderView=nil;

}

#pragma mark -
#pragma mark Memory Management

- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];
}

- (void)dealloc 
{
    _refreshHeaderView = nil;
    [super dealloc];
}

#pragma mark -
#pragma mark Pull-to-Refresh

@synthesize pullToRefresh=_pullToRefresh;
- (void) setPullToRefresh:(BOOL)value
{
    _pullToRefresh = value;
    
    if (_pullToRefresh)
    {
        if (_refreshHeaderView == nil) 
        {
            EGORefreshTableHeaderView *view = [[EGORefreshTableHeaderView alloc] initWithFrame:CGRectMake(0.0f, 0.0f - self.tableView.bounds.size.height, self.view.frame.size.width, self.tableView.bounds.size.height)];
            view.delegate = self;
            [self.tableView addSubview:view];
            _refreshHeaderView = view;
            [view release];
            
        }
        
        //  update the last update date
        [_refreshHeaderView refreshLastUpdatedDate];
    }
    else
    {
        if (_refreshHeaderView)
        {
            [_refreshHeaderView removeFromSuperview];
            _refreshHeaderView = nil;
        }
    }
}

#pragma mark UIScrollViewDelegate Methods

- (void)scrollViewDidScroll:(UIScrollView *)scrollView
{		
	[_refreshHeaderView egoRefreshScrollViewDidScroll:scrollView];
}

- (void)scrollViewDidEndDragging:(UIScrollView *)scrollView willDecelerate:(BOOL)decelerate
{	
	[_refreshHeaderView egoRefreshScrollViewDidEndDragging:scrollView];	
}


#pragma mark -
#pragma mark EGORefreshTableHeaderDelegate Methods

- (void)egoRefreshTableHeaderDidTriggerRefresh:(EGORefreshTableHeaderView*)view
{	
	[self forceRefresh];
    if (![self refreshInProgress])
    {
        /* The refresh didnt start */
        _reloading = NO;
        [_refreshHeaderView performSelector:@selector(egoRefreshScrollViewDataSourceDidFinishedLoading:) 
                                 withObject:self.tableView 
                                 afterDelay:0.0];
    }
}

- (BOOL)egoRefreshTableHeaderDataSourceIsLoading:(EGORefreshTableHeaderView*)view
{	
	return _reloading; // should return if data source model is reloading
}

- (NSDate*)egoRefreshTableHeaderDataSourceLastUpdated:(EGORefreshTableHeaderView*)view
{	
    [NSException raise:@"egoRefreshTableHeaderDataSourceLastUpdated" 
                format:@"Subclass %@ must implement egoRefreshTableHeaderDataSourceLastUpdated:", self];
	return nil;
}

#pragma mark -
#pragma mark Table view methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}


// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return 0;
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Set up the cell...
	
    return cell;
}

- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath
{
	/* Clear text field backgrounds */
	if ([[cell class] isSubclassOfClass:[LTEntityRefreshProgressViewCell class]])
	{
		LTEntityRefreshProgressViewCell *progressCell = (LTEntityRefreshProgressViewCell *) cell;
		progressCell.backgroundColor = [UIColor colorWithWhite:0.0 alpha:0.5];
		progressCell.opaque = NO;
		progressCell.progressLabel.backgroundColor = [UIColor clearColor];
		progressCell.progressView.backgroundColor = [UIColor clearColor];
	}
	
	cell.textLabel.backgroundColor = [UIColor clearColor];
}


- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    // Navigation logic may go here. Create and push another view controller.
	// AnotherViewController *anotherViewController = [[AnotherViewController alloc] initWithNibName:@"AnotherView" bundle:nil];
	// [self.navigationController pushViewController:anotherViewController];
	// [anotherViewController release];
}

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
{
	if ([self respondsToSelector:@selector(tableView:titleForHeaderInSection:)] && [self tableView:tableView titleForHeaderInSection:section])
	{ 
		LTTableViewSectionHeaderView *view = [[[LTTableViewSectionHeaderView alloc] initWithFrame:CGRectZero] autorelease];
		view.titleLabel.text = [self tableView:tableView titleForHeaderInSection:section];
		return view; 
	}
	else 
	{ return nil; }
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

#pragma mark -
#pragma mark Subclass methods

- (void) refresh
{
	// Subclass should over-ride this
    if (self.pullToRefresh)
    {
        [NSException raise:@"refresh" format:@"refresh must be overridden by subclass %@", self];
    }
}

- (void) forceRefresh
{
	// Subclass should over-ride this
    if (self.pullToRefresh)
    {
        [NSException raise:@"forceRefresh" format:@"forceRefresh must be overridden by subclass %@", self];
    }    
}

- (BOOL) refreshInProgress
{
    if (self.pullToRefresh)
    {
        [NSException raise:@"refreshInProgress" format:@"refreshInProgress must be overridden by subclass %@", self];
    }
    return NO;
}

#pragma mark -
#pragma mark Properties

@synthesize isVisible;
- (BOOL) isVisible
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	if (!appDelegate.isActive) return NO;
	else return isVisible;
}

@end

