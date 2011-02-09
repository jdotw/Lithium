//
//  LTEntityTableViewController.m
//  Lithium
//
//  Created by James Wilson on 26/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTEntityTableViewController.h"
#import "LTEntityTableViewCell.h"
#import "AppDelegate.h"
#import "LTCoreDeployment.h"
#import "LTMetricTableViewController.h"
#import "LTTableViewCellBackground.h"
#import "LTTableViewCellSelectedBackground.h"
#import "LTTableViewCell.h"
#import "LTEntityRefreshProgressViewCell.h"
#import "LTCoreEditTableViewController.h"
#import "LTFavoritesTableViewController.h"
#import "LTDeviceViewController.h"
#import "LTTableView.h"
#import "LTDeviceEditTableViewController.h"
#import "AppDelegate_Pad.h"
#import "LTHardwareEntityTableViewCell.h"
#import "LTDeviceEntityTableViewCell.h"
#import "LTRackTableViewHeaderView.h"
#import "LTSubDeviceTableViewCellBackgroundView.h"

@interface LTEntityTableViewController (private)
- (void) coreDeploymentArrayUpdated:(NSNotification *)notification;
- (void) entityRefreshFinished:(NSNotification *)notification;
- (void) sortAndFilterChildren;
@end

@implementation LTEntityTableViewController

@synthesize externalNavigationController, drawAsRack=_drawAsRack;

- (BOOL) _groupDevicesByLocation
{
    /* Returns true if the list is currently grouping all
     * devics together by location
     */
    if (self.entity.type == ENT_CUSTOMER && [[NSUserDefaults standardUserDefaults] boolForKey:kDeviceListGroupByLocation])
    { return YES; }
    else
    { return NO; }
}

- (id)initWithEntity:(LTEntity *)initEntity
{
    /* THis initialization is only used when viewing 
     * actual entities and not for the root list 
     * of Lithium Core deployments which uses awake from NIB
     */
	self = [super initWithNibName:@"LTEntityTableViewController" bundle:nil];
	if (!self) return nil;
	
	self.entity = initEntity;
    
	return self;
}

- (void)viewDidLoad 
{
    [super viewDidLoad];
	
    /* Listening to *all* RefreshFinished notifications is intentional,
     * it ensures that we catch the refresh of root entity, child entities
     * and the root customer, all of which trigger a refresh 
     */
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(entityRefreshFinished:)
												 name:@"RefreshFinished" 
											   object:nil];
    

	if (entity)
	{
		if (entity.type >= 1 && entity.type < 3 && UI_USER_INTERFACE_IDIOM()==UIUserInterfaceIdiomPad) 
		{
			/* Entity is a Customer or Site, show an Add button to add a device
			 * only if the device is an iPad 
			 */
			self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd
																									target:self
																									action:@selector(addDeviceTouched:)] autorelease];
		}		
	}
	else
	{
#ifndef DEMO
		self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd
																								target:self 
																								action:@selector(addNewCoreTouched:)] autorelease];
		self.navigationItem.leftBarButtonItem = self.editButtonItem;
#endif
	}

	if (entity.type != 1 && entity.type != 2)
	{
		/* Hide search for non-customer view */
		self.tableView.tableHeaderView = nil;
	}
    	
    /* Add Refresh Timer */
	NSTimeInterval timerInterval;
	if (self.entity.device.refreshInterval < 15.0) timerInterval = 15.0;
	else timerInterval = (self.entity.refreshInterval * 0.5f);
	refreshTimer = [NSTimer scheduledTimerWithTimeInterval:timerInterval
													target:self
												  selector:@selector(refreshTimerFired:)
												  userInfo:nil
												   repeats:YES];
    
    /* iPad-Specific Table Setup */
    if (UI_USER_INTERFACE_IDIOM()==UIUserInterfaceIdiomPad)
    {
        if (entity.type >= ENT_DEVICE)
        {
            self.drawAsRack = NO;
            self.tableView.backgroundColor = [UIColor colorWithWhite:0.1 alpha:1.0];
        }
    }
}

- (void) viewDidUnload
{
	[super viewDidUnload];
}

- (void) awakeFromNib
{
	[super awakeFromNib];

	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];

	/* We are the root-level controller, observe core deployment and customer changes */
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(coreDeploymentArrayUpdated:)
												 name:@"CoreDeploymentAdded" object:appDelegate];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(coreDeploymentArrayUpdated:)
												 name:@"CoreDeploymentUpdated" object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(coreDeploymentArrayUpdated:)
												 name:@"CoreDeploymentRemoved" object:appDelegate];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(coreDeploymentReachabilityChanged:)
												 name:@"LTCoreDeploymentReachabilityChanged" object:nil];
	[self coreDeploymentArrayUpdated:nil];
	
	
	self.tableView.allowsSelectionDuringEditing = YES;
}

- (void)dealloc 
{
	AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
	[refreshTimer invalidate];
	[children release];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"RefreshFinished" object:nil];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"CoreDeploymentAdded" object:appDelegate];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"CoreDeploymentUpdated" object:nil];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"CoreDeploymentRemoved" object:appDelegate];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"LTCoreDeploymentReachabilityChanged" object:nil];
	if (entity) [[NSNotificationCenter defaultCenter] removeObserver:self name:@"LTEntityXmlStatusChanged" object:entity];
	
    [super dealloc];
}

#pragma mark -
#pragma mark "View Delegates"

- (CGSize) contentSizeForViewInPopover
{
	return CGSizeMake(320.0, 500.0);
}

- (void)viewWillAppear:(BOOL)animated 
{
    [super viewWillAppear:animated];
	if (!hasAppeared)
	{
		hasAppeared = YES;
	}
	[self refresh];
}

- (void)viewDidAppear:(BOOL)animated 
{
    [super viewDidAppear:animated];
}
		
- (void)viewWillDisappear:(BOOL)animated 
{
	[super viewWillDisappear:animated];
}


/*
- (void)viewDidDisappear:(BOOL)animated {
	[super viewDidDisappear:animated];
}
*/

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation 
{
    return YES;
}

- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

#pragma mark -
#pragma mark View Configuration

- (void)setDrawAsRack:(BOOL)value
{
    _drawAsRack = value;
    if (_drawAsRack)
    {
        self.tableView.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"RackBackTile.png"]];        
    }
}

#pragma mark -
#pragma mark Refresh Methods

- (void) refreshTimerFired:(NSTimer *)timer
{
	/* Chck if app s active/locked */
	if (self.isVisible)
	{
		[self refresh];
	}
}

- (void) refresh
{
	if (entity)
	{
		[entity refresh];
	}
	else
	{
		for (LTEntity *child in children)
		{
			[child refresh];
		}
	}
}

#pragma mark "Table view methods"

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
	if (tableView == self.searchDisplayController.searchResultsTableView)
	{
		return 1;
	}
	else
	{
		if (entity.type == ENT_CUSTOMER && [self _groupDevicesByLocation])
		{
			return children.count;
		}
		else 
		{
			return 1;
		}		
	}
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
	if (tableView == self.searchDisplayController.searchResultsTableView)
	{
		return nil;
	}
	else 
	{
		if (entity.type == ENT_CUSTOMER && [self _groupDevicesByLocation])
		{
			return [[children objectAtIndex:section] desc];
		}
		else 
		{
			return nil;
		}		
	}
}

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
{
    NSString *label = [self tableView:tableView titleForHeaderInSection:section];
    if (label)
    { 
        LTRackTableViewHeaderView *header = [[LTRackTableViewHeaderView alloc] initWithFrame:CGRectZero]; 
        header.textLabel.text = label;
        return header;
    }
    else
    { return nil; }
}

- (CGFloat) tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    return 28.0;    // Rack "tape" header view is always 28. high
}

// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	if (tableView == self.searchDisplayController.searchResultsTableView)
	{
		return searchFilteredItems.count;
	}
	else
	{
		if (entity)
		{
			if ([children count] == 0 && entity.refreshInProgress)
			{ return 1; }
			else if (entity.type == ENT_CUSTOMER && [self _groupDevicesByLocation])
			{
				return [[[children objectAtIndex:section] children] count];
			}
			else
			{ return [children count]; }
		}
		else
		{
			return [children count];
		}
	}
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	if (tableView != self.searchDisplayController.searchResultsTableView && [children count] == 0 && entity.refreshInProgress)
	{
		/* Refresh cell */
		return self.tableView.frame.size.height; 
	}
	else
	{ 
		/* Normal Cell */
		return 52.0; 
	}
}

- (LTEntity *) entityAtIndexPath:(NSIndexPath *)indexPath inTableView:(UITableView *)tableView
{
	if (tableView == self.searchDisplayController.searchResultsTableView)
	{
		return [searchFilteredItems objectAtIndex:indexPath.row];
	}
	else 
	{
		if (entity)
		{
			if (entity.type == ENT_CUSTOMER && [self _groupDevicesByLocation])
			{
                /* The children array will contain locations */
                if (indexPath.section < children.count)
                {
                    LTEntity *location = [children objectAtIndex:indexPath.section];
                    if (indexPath.row < location.children.count)
                    { return [location.children objectAtIndex:indexPath.row]; }
                    else
                    { return nil; }
                }
                else
                { return nil; }
			}
			else
			{
				if (indexPath.row < [children count])
				{ return [children objectAtIndex:indexPath.row]; }
				else 
				{ return nil; }
			}
		}
		else
		{
			return [children objectAtIndex:indexPath.row];
		}			
	}
}

- (NSString *) tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section
{
	if (self.entity.type == 4 && [self.entity.name isEqualToString:@"avail"] && self.entity.opState > 0)
	{
		/* This is the availability container and there is a problem */
		return @"\nThe device is not responding to one or more of the protocols that Lithium Core is using to gather monitoring data from the device.";
	}
	else return nil;
}

- (UIView *) tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section
{
	NSString *footerTitle = [self tableView:tableView titleForFooterInSection:section];
	if (footerTitle)
	{
		UILabel *label = [[[UILabel alloc] initWithFrame:CGRectZero] autorelease];
		label.text = footerTitle;
		label.numberOfLines = 0;
		label.font = [UIFont systemFontOfSize:12.0];
		label.backgroundColor = [UIColor clearColor];
		label.textColor = [UIColor grayColor];
		label.textAlignment = UITextAlignmentCenter;
		return label;
	}
	else
	{
		return nil;
	}
}

- (CGFloat) tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section
{
	NSString *footerTitle = [self tableView:tableView titleForFooterInSection:section];
	if (footerTitle)
	{
		CGSize stringSize = [footerTitle sizeWithFont:[UIFont systemFontOfSize:12.0] constrainedToSize:tableView.bounds.size lineBreakMode:UILineBreakModeWordWrap];
		return stringSize.height;
	}
	else
	{
		return 0.;
	}
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    LTEntity *displayEntity = [self entityAtIndexPath:indexPath inTableView:tableView];
	
    NSString *CellIdentifier;
	if (displayEntity)
	{ 
		if (displayEntity.type >= 3)
		{
			switch (displayEntity.type) {
				case 3:
					CellIdentifier = @"Device";
					break;
				default:
					CellIdentifier = @"Hardware";
					break;
			}
		}
		else CellIdentifier = @"Entity"; 
	}
	else 
	{ CellIdentifier = @"Refresh"; }
    
    LTEntityTableViewCell *cell = (LTEntityTableViewCell *) [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
	{
		if ([CellIdentifier isEqualToString:@"Entity"])
		{
			cell = [[[LTEntityTableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:CellIdentifier] autorelease];
		}
		else if ([CellIdentifier isEqualToString:@"Device"])
		{
			cell = [[[LTDeviceEntityTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
		}
		else if ([CellIdentifier isEqualToString:@"Hardware"])
		{
			cell = [[[LTHardwareEntityTableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:CellIdentifier] autorelease];
		}
		else if ([CellIdentifier isEqualToString:@"Refresh"])
		{
			cell = [[[LTEntityRefreshProgressViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
			cell.selectionStyle = UITableViewCellSelectionStyleNone;
		}
		if (displayEntity)
		{
			if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad && displayEntity.type == 3)
			{ cell.accessoryType = UITableViewCellAccessoryNone; }
			else
			{ cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator; }
		}
    }
    
    // Set up the cell...
	if (displayEntity)
	{
		cell.textLabel.text = displayEntity.desc;
		cell.textLabel.font = [UIFont boldSystemFontOfSize:16.0];
		cell.entity = displayEntity;
        cell.drawAsRack = self.drawAsRack;
		if (displayEntity.type == 0)
		{
			/* Displaying a deployment, use the detailtextLabel (subtitle)
			 * to show the deployment status 
			 */
			LTCoreDeployment *deployment = (LTCoreDeployment *) displayEntity;
			if (!deployment.reachable)
			{
				cell.detailTextLabel.text = @"Deployment is unreachable";
			}
			else if (!deployment.enabled)
			{ 
				cell.detailTextLabel.text = @"Deployment is disabled";
			}
			else if (deployment.discovered)
			{
				cell.detailTextLabel.text = @"Discovered deployment";
			}
			else
			{ 
				cell.detailTextLabel.text = nil;
			}				
		}
        
        /* iPad Specific Cell Setup */
        if (UI_USER_INTERFACE_IDIOM()==UIUserInterfaceIdiomPad)
        {
            if (entity.type >= ENT_DEVICE)
            {
                cell.backgroundView = [[LTSubDeviceTableViewCellBackgroundView alloc] initWithFrame:CGRectZero];
                cell.textLabel.font = [UIFont boldSystemFontOfSize:14.0];
                cell.textLabel.shadowColor = [UIColor blackColor];
                cell.textLabel.shadowOffset = CGSizeMake(0.0, -1.0);
                cell.textLabel.minimumFontSize = 12.0;
                cell.textLabel.adjustsFontSizeToFitWidth = YES;
                cell.detailTextLabel.font = [UIFont systemFontOfSize:14.0];
                cell.detailTextLabel.shadowColor = [UIColor blackColor];
                cell.detailTextLabel.shadowOffset = CGSizeMake(0.0, -1.0);
                cell.detailTextLabel.minimumFontSize = 12.0;
                cell.detailTextLabel.adjustsFontSizeToFitWidth = YES;
            }
        }
	}
	else
	{
		/* Refresh info */
		LTEntityRefreshProgressViewCell *progressCell = (LTEntityRefreshProgressViewCell *) cell;
		if (![entity.coreDeployment enabled])
		{
			[progressCell.progressView stopAnimating];
			progressCell.progressLabel.text = @"Deployment is disabled.";
		}
		else if (![entity.coreDeployment reachable])
		{
			[progressCell.progressView stopAnimating];
			progressCell.progressLabel.text = @"Deployment is unreachable.";
		}
		else
		{
			[progressCell.progressView startAnimating];
			if (entity.xmlStatus) progressCell.progressLabel.text = entity.xmlStatus;
			else progressCell.progressLabel.text = @"Downloading Monitoring Data...";
		}
	}
		
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	if (!entity && self.tableView.editing)
	{
		/* Edit the selected core deployment */
		LTCoreDeployment *deployment = [children objectAtIndex:indexPath.row];
		if (!deployment.discovered)
		{
			LTCoreEditTableViewController *controller = [[LTCoreEditTableViewController alloc] initWithCoreToEdit:[children objectAtIndex:indexPath.row]];
			UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:controller];
			navController.navigationBar.tintColor = [UIColor colorWithWhite:120.0/255.0 alpha:1.0];
			navController.modalPresentationStyle = UIModalPresentationFormSheet;
			[self.navigationController presentModalViewController:navController animated:YES];
			[controller release];
			[navController release];	
			return;
		}
	}

	LTEntity *selectedEntity = nil;
	if (entity)
	{
		selectedEntity = [self entityAtIndexPath:indexPath inTableView:tableView];
	}
	else
	{
		selectedEntity = [children objectAtIndex:indexPath.row];
	}
	if (!selectedEntity) return;
	
	LTEntity *viableEntity = selectedEntity;
	while ([viableEntity.children count] == 1 && viableEntity.type < 2)
	{
		viableEntity = [viableEntity.children objectAtIndex:0];
	}
	if (viableEntity.type == 4 && viableEntity.children.count == 1)
	{ viableEntity = [viableEntity.children objectAtIndex:0]; }
	
	if (viableEntity.type == 6)
	{
		/* Metric */
		LTMetricTableViewController *metricView = [[LTMetricTableViewController alloc] initWithMetric:viableEntity];
		[self.navigationController pushViewController:metricView animated:YES];
		[metricView release];
	}
	else if (viableEntity.type == 3 && UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		/* Device and iPad */
		AppDelegate_Pad *appDelegate = (AppDelegate_Pad *) [[UIApplication sharedApplication] delegate];
		[appDelegate displayEntityInDetailView:viableEntity];
	}
	else
	{
		/* Default Handling, push new VC */
		LTEntityTableViewController *anotherViewController = [[LTEntityTableViewController alloc] initWithEntity:viableEntity];
		[anotherViewController.tableView setNeedsDisplay];		
		[self.navigationController pushViewController:anotherViewController animated:YES];
		[anotherViewController release];
	}	
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath 
{
	if (entity)
	{ 
		return NO;
	}
	else
	{
		AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
		if ([indexPath section] == 0)
		{
			if ([indexPath row] < appDelegate.coreDeployments.count)
			{ 
				LTCoreDeployment *deployment = [appDelegate.coreDeployments objectAtIndex:indexPath.row];
				if (deployment.discovered) return NO;
				else return YES;
			}
			else
			{ return NO; }
		}
		else
		{ return NO; }
	}
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath 
{ 
    if (editingStyle == UITableViewCellEditingStyleDelete && !entity) 
	{
        // Delete the row from the data source
		AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
		LTCoreDeployment *core = [appDelegate.coreDeployments objectAtIndex:[indexPath row]];
		[appDelegate removeCore:core];
		[children removeObject:core];
        [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:YES];		
    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) 
	{
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }
}

- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath
{
	[super tableView:tableView willDisplayCell:cell forRowAtIndexPath:indexPath];
	
	if (
#ifdef UI_USER_INTERFACE_IDIOM
		UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone
#else
		1
#endif
		)
	{ 
		/* Refresh the entity just-in-time (phone only) */
		LTEntity *displayEntity = [self entityAtIndexPath:indexPath inTableView:tableView];
		[displayEntity refresh];
	}
	
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


/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

#pragma mark Search Delegate

- (void)filterContentForSearchText:(NSString*)searchText scope:(NSString*)scope
{
	if (!searchFilteredItems) searchFilteredItems = [[NSMutableArray array] retain];
	[searchFilteredItems removeAllObjects];	
	
	NSArray *unfilteredEntities = nil;
	
	if (entity.type == ENT_CUSTOMER && [self _groupDevicesByLocation])
	{
		/* Build a list of all devices */
		unfilteredEntities = [children valueForKeyPath:@"@unionOfArrays.children"];
	}
	else 
	{
		unfilteredEntities = children;
	}

	NSPredicate *filterPredicate = nil;
	if ([scope isEqualToString:@"Name"])
	{
		filterPredicate = [NSPredicate predicateWithFormat:@"desc CONTAINS[cd] %@", searchText];
	}
	else if ([scope isEqualToString:@"IP"])
	{
		filterPredicate = [NSPredicate predicateWithFormat:@"ipAddress CONTAINS[cd] %@", searchText];
	}
	
	if (filterPredicate)
	{ [searchFilteredItems addObjectsFromArray:[unfilteredEntities filteredArrayUsingPredicate:filterPredicate]]; }
}

- (void) searchDisplayController:(UISearchDisplayController *)controller willShowSearchResultsTableView:(UITableView *)tableView
{
	tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
	tableView.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"RackBackTile.png"]];
}

-(void) searchDisplayControllerWillBeginSearch:(UISearchDisplayController *)controller
{
	searchBar.tintColor = nil;	
	searchBar.showsScopeBar = YES;
}

- (void) searchDisplayControllerWillEndSearch:(UISearchDisplayController *)controller
{
	searchBar.tintColor = [UIColor colorWithWhite:0.29 alpha:1.0];	
	searchBar.showsScopeBar = NO;
}

- (BOOL)searchDisplayController:(UISearchDisplayController *)controller shouldReloadTableForSearchString:(NSString *)searchString
{
    [self filterContentForSearchText:searchString scope:[[self.searchDisplayController.searchBar scopeButtonTitles] objectAtIndex:[self.searchDisplayController.searchBar selectedScopeButtonIndex]]];
    return YES;
}


- (BOOL)searchDisplayController:(UISearchDisplayController *)controller shouldReloadTableForSearchScope:(NSInteger)searchOption
{
    [self filterContentForSearchText:[self.searchDisplayController.searchBar text] scope:[[self.searchDisplayController.searchBar scopeButtonTitles] objectAtIndex:searchOption]];
    return YES;
}


#pragma mark "KVO Callbacks"

- (void) coreDeploymentArrayUpdated:(NSNotification *)notification
{
	[self sortAndFilterChildren];
	[[self tableView] reloadData];
}

- (void) coreDeploymentReachabilityChanged:(NSNotification *)notification
{
    if (entity && entity.coreDeployment.reachable)
    {
        /* Trigger a refresh based on reachability change */
        [entity refresh];
    }
    else if (!entity)
    {
        /* We're displaying the list of cores, update the table to reflect reachability */
        [self sortAndFilterChildren];
        [[self tableView] reloadData];
    }
}

- (void) entityStateChanged:(NSNotificationCenter *)notification
{
    /* Called when the root entity changes state */
    
}

- (void) entityChildrenChanged:(NSNotification *)notification
{
    NSLog (@"%@ Got entityChildrenChanged for %@", self, [notification object]);
    
    /* Called when there is a change in an entities children 
     * to force a reload of the table data 
     */
    [self sortAndFilterChildren];	
    [[self tableView] reloadData];        
}

- (void) entityRefreshFinished:(NSNotification *)notification
{
    /* What used to be done here is now done only on children changes */
}

- (void) entityRefreshStatusUpdated:(NSNotification *)notification
{
	[self sortAndFilterChildren];	
	[[self tableView] reloadData];
}

#pragma mark "Entity Actions"

- (IBAction) actionClicked:(id)sender
{
	UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:[NSString stringWithFormat:@"%@ Actions", entity.desc]
															 delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil
													otherButtonTitles:@"Add To Favorites", nil];
	actionSheet.actionSheetStyle = UIActionSheetStyleDefault;
	[actionSheet showInView:self.view];
	[actionSheet release];
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
	if (buttonIndex == 0)
	{
		/* Add to Favorites */
		AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
		[appDelegate.favoritesController addToFavorites:self.entity];
	}
}

- (IBAction) addNewCoreTouched:(id)sender
{
	LTCoreEditTableViewController *controller = [[LTCoreEditTableViewController alloc] initWithStyle:UITableViewStyleGrouped];
	UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:controller];
	navController.navigationBar.tintColor = [UIColor colorWithWhite:120.0/255.0 alpha:1.0];
	navController.modalPresentationStyle = UIModalPresentationFormSheet;
	[self.navigationController presentModalViewController:navController animated:YES];
	[controller release];
	[navController release];	
}

- (IBAction) addDeviceTouched:(id)sender
{
	LTDeviceEditTableViewController *vc = [[LTDeviceEditTableViewController alloc] initForNewDeviceAtSite:self.entity.site customer:self.entity.customer];
	UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:vc];
	navController.navigationBar.tintColor = [UIColor colorWithWhite:120.0/255.0 alpha:1.0];
	navController.modalPresentationStyle = UIModalPresentationFormSheet;
	[self.navigationController presentModalViewController:navController animated:YES];
	[vc release];
	[navController release];	
}

#pragma mark -
#pragma mark Internal List Management

- (void) sortAndFilterChildren
{
	[children release];
	children = [[NSMutableArray array] retain];

	/* Build children array */
	if (entity)
	{
		/* Representing a specific Entity */
		for (LTEntity *child in entity.children)
		{
			[children addObject:child]; 
		}
	}
	else
	{
		/* Representing deployment list */
		AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];		
		[children addObjectsFromArray:[appDelegate coreDeployments]];
	}	

    /* Sort list */
    if (entity.type == 5)
    {
        /* Sort metric list by triggers/no-triggers and recording method */
        NSSortDescriptor *opStateSortDesc = [[[NSSortDescriptor alloc] initWithKey:@"opState" ascending:NO] autorelease];
        
        NSSortDescriptor *summarySortDesc = [[[NSSortDescriptor alloc] initWithKey:@"showInSummary" ascending:NO] autorelease];
        NSSortDescriptor *triggerSortDesc = [[[NSSortDescriptor alloc] initWithKey:@"hasTriggers" ascending:NO] autorelease];
        NSSortDescriptor *graphableSortDesc = [[[NSSortDescriptor alloc] initWithKey:@"recordEnabled" ascending:NO] autorelease];
        [children sortUsingDescriptors:[NSArray arrayWithObjects:opStateSortDesc, summarySortDesc, triggerSortDesc, graphableSortDesc, nil]];
    }
    else
    {
        /* Default sort: By description */
        NSSortDescriptor *sortDesc = [[[NSSortDescriptor alloc] initWithKey:@"desc" ascending:YES selector:@selector(localizedCompare:)] autorelease];
        [children sortUsingDescriptors:[NSArray arrayWithObjects:sortDesc, nil]];			
    }
}

#pragma mark -
#pragma mark Properties

- (NSString *) title
{ 
	if (entity.type == 5 && [entity.name isEqualToString:@"master"] && [[[entity parent] children] count] == 1) 
	{ return entity.parent.desc; }
	else if (entity.type == 2 && [entity.name isEqualToString:@"default"]) 
	{ return entity.parent.desc; }
	else if (entity)
	{ return entity.desc; }
	else
	{ return @"Devices"; }
}

@synthesize entity;
- (void) setEntity:(LTEntity *)value
{
	if (entity)
	{
        /* Remove old notification listeners */
		[[NSNotificationCenter defaultCenter] removeObserver:self name:@"LTEntityXmlStatusChanged" object:entity];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:kLTEntityChildrenChanged object:entity];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:@"LTCoreDeploymentReachabilityChanged" object:entity.coreDeployment];
	}

    [entity release];
	entity = [value retain];

	self.navigationItem.title = entity.desc;
	[self sortAndFilterChildren];

	if (entity)
	{
        /* Listen to change in XML Status to update refresh progress */
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(entityRefreshStatusUpdated:)
													 name:@"LTEntityXmlStatusChanged" object:entity];		
        
        /* Listen to changes in children array to reload table */
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(entityChildrenChanged:)
                                                     name:kLTEntityChildrenChanged
                                                   object:entity];   
        
        /* Listen for changes in reachability to trigger a refresh */
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(coreDeploymentReachabilityChanged:)
                                                     name:@"LTCoreDeploymentReachabilityChanged" object:entity.coreDeployment];		
        
	}
    
    if (entity.type == ENT_SITE || (entity.type == ENT_CUSTOMER && [self _groupDevicesByLocation]))
    {
        /* The child entities are 'hardware' and we should draw as a rack */
        self.drawAsRack = YES;
    }
}	
	

@end

