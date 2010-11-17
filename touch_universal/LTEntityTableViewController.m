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
#import "LTMetricTableViewCell.h"
#import "LTMetricTableViewController.h"
#import "LTTableViewCellBackground.h"
#import "LTTableViewCellSelectedBackground.h"
#import "LTTableViewCell.h"
#import "LTEntityRefreshProgressViewCell.h"
#import "LTCoreEditTableViewController.h"
#import "LTFavoritesTableViewController.h"
#import "LTDeviceViewController.h"
#import "LTTableView.h"
#import "AppDelegate_Pad.h"

@interface LTEntityTableViewController (private)
- (void) coreDeploymentArrayUpdated:(NSNotification *)notification;
- (void) entityRefreshFinished:(NSNotification *)notification;
- (void) sortAndFilterChildren;
- (void) refreshTimerFired:(NSTimer *)timer;
@end

@implementation LTEntityTableViewController

@synthesize externalNavigationController;

- (id)initWithEntity:(LTEntity *)initEntity
{
	self = [super initWithNibName:@"LTEntityTableViewController" bundle:nil];
	if (!self) return nil;
	
	self.entity = initEntity;
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(coreDeploymentReachabilityChanged:)
												 name:@"LTCoreDeploymentReachabilityChanged" object:nil];		
	
	return self;
}

- (void)viewDidLoad 
{
    [super viewDidLoad];
	
	if (entity)
	{
		self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh
																								target:self 
																								action:@selector(refreshTouched:)] autorelease];
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
}

- (void) awakeFromNib
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];

	/* We are the root-level controller, observe core deployment changes */
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
	
	[super awakeFromNib];
	
	self.tableView.allowsSelectionDuringEditing = YES;
}

- (void)dealloc 
{
	[children release];
    [super dealloc];
}

#pragma mark "View Delegates"

- (CGSize) contentSizeForViewInPopover
{
	return CGSizeMake(300.0, 500.0);
}

- (void)viewWillAppear:(BOOL)animated 
{
    [super viewWillAppear:animated];
	[self refreshTimerFired:nil];
	if (!hasAppeared)
	{
		hasAppeared = YES;
	}
}

- (void)viewDidAppear:(BOOL)animated 
{
    [super viewDidAppear:animated];

	NSTimeInterval timerInterval;
	if (self.entity.refreshInterval < 15.0) timerInterval = 15.0;
	else timerInterval = (self.entity.refreshInterval * 0.5f);
	refreshTimer = [[NSTimer scheduledTimerWithTimeInterval:timerInterval
													 target:self
												   selector:@selector(refreshTimerFired:)
												   userInfo:nil
													repeats:YES] retain];
}
		
- (void)viewWillDisappear:(BOOL)animated 
{
	[super viewWillDisappear:animated];

	[refreshTimer invalidate];
	[refreshTimer release];
	refreshTimer = nil;
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


#pragma mark Refresh Timer

- (void) refreshTimerFired:(NSTimer *)timer
{
	/* Chck if app s active/locked */
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	if (!appDelegate.isActive) return;

	if (entity)
	{
		[entity refresh];
		if (entity.refreshInProgress) [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:YES];
	}
	else
	{
		for (LTEntity *child in children)
		{
			[child refresh];
			if (child.refreshInProgress) [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:YES];
		}
	}
}

- (IBAction) refreshTouched:(id)sender
{
	entity.lastRefresh = nil;
	[entity refresh];
	if (entity.refreshInProgress) [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:YES];	
}

#pragma mark "Table view methods"

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
	return 1;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
	return nil;
}

// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	if (entity)
	{
		if ([children count] == 0 && entity.refreshInProgress)
		{ return 1; }
		else
		{ return [children count]; }

	}
	else
	{
		return [children count];
	}
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	if ([children count] == 0 && entity.refreshInProgress)
	{ return self.tableView.frame.size.height; }
	else
	{ 
		if (tableView.frame.size.height > 600)
		{ 
			return 52.0; 
		}
		else
		{
			return 48.0; 
		}
	}
}

- (LTEntity *) entityAtIndexPath:(NSIndexPath *)indexPath
{
	if (entity)
	{
		/* iPhone */
		if (indexPath.row < [children count])
		{ return [children objectAtIndex:indexPath.row]; }
		else 
		{ return nil; }
	}
	else
	{
		return [children objectAtIndex:indexPath.row];
	}	
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
    LTEntity *displayEntity = [self entityAtIndexPath:indexPath];
	NSLog (@"%@ got %@", indexPath, displayEntity);
	
    NSString *CellIdentifier;
	if (displayEntity.type == 6 || (displayEntity.type == 5 && displayEntity.children.count == 1))
	{ CellIdentifier = @"Metric"; }
	else if (displayEntity)
	{ CellIdentifier = @"Entity"; }
	else 
	{ CellIdentifier = @"Refresh"; }
    LTEntityTableViewCell *cell = (LTEntityTableViewCell *) [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
	{
		if ([CellIdentifier isEqualToString:@"Metric"])
		{
			LTMetricTableViewCell *metricCell = [[[LTMetricTableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:CellIdentifier] autorelease];
			metricCell.showCurrentValue = YES;
			cell = metricCell;			
		}
		else if ([CellIdentifier isEqualToString:@"Entity"])
		{
			cell = [[[LTEntityTableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:CellIdentifier] autorelease];
			cell.detailTextLabel.font = [UIFont systemFontOfSize:12];
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
		if (displayEntity.type > 0)
		{
			cell.entityState = displayEntity.opState;
		}
		else
		{
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
		selectedEntity = [self entityAtIndexPath:indexPath];
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
		LTEntity *displayEntity = [self entityAtIndexPath:indexPath];
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

#pragma mark "KVO Callbacks"

- (void) coreDeploymentArrayUpdated:(NSNotification *)notification
{
	[self sortAndFilterChildren];
	[[self tableView] reloadData];
}

- (void) coreDeploymentReachabilityChanged:(NSNotification *)notification
{
	[self sortAndFilterChildren];
	[[self tableView] reloadData];
}

- (void) entityRefreshFinished:(NSNotification *)notification
{
	if (entity)	[[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO];	
	[self sortAndFilterChildren];	
	if (entity && children.count == 1)
	{
		/* Entity refresh completed returning a lone entity; 
		 * supplant our current entity with this lone entity
		 */
		LTEntity *singleEntity = [children objectAtIndex:0];
		self.entity = singleEntity;
	}
	[[self tableView] reloadData];
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


#pragma mark "Properties"

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

- (void) sortAndFilterChildren
{
	[children release];
	children = [[NSMutableArray array] retain];
	
	if (entity)
	{
		/* Representing a specific Entity */
		for (LTEntity *child in entity.children)
		{
			[children addObject:child]; 
		}
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
			NSSortDescriptor *opStateSortDesc = [[[NSSortDescriptor alloc] initWithKey:@"opState" ascending:NO] autorelease];
			[children sortUsingDescriptors:[NSArray arrayWithObjects:opStateSortDesc, nil]];			
		}
	}
	else
	{
		/* Representing deployment list */
		AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];		
		[children addObjectsFromArray:[appDelegate coreDeployments]];
	}	
}
	
@synthesize entity;
- (void) setEntity:(LTEntity *)value
{
	if (entity)
	{
		[[NSNotificationCenter defaultCenter] removeObserver:self name:@"RefreshFinished" object:entity];
		[[NSNotificationCenter defaultCenter] removeObserver:self name:@"LTEntityXmlStatusChanged" object:entity];
		[entity release];
	}

	entity = [value retain];
	self.navigationItem.title = entity.desc;
	[self sortAndFilterChildren];

	if (entity)
	{
		if (entity.refreshInProgress) [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:YES];
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(entityRefreshFinished:)
													 name:@"RefreshFinished" object:entity];
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(entityRefreshStatusUpdated:)
													 name:@"LTEntityXmlStatusChanged" object:entity];		
	}
}	
	

@end

