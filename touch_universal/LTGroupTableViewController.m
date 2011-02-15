//
//  LTGroupTableViewController.m
//  Lithium
//
//  Created by James Wilson on 18/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LTGroupTableViewController.h"
#import "LTEntityTableViewCell.h"
#import "AppDelegate.h"
#import "LTCoreDeployment.h"
#import "LTMetricTableViewController.h"
#import "LTTableViewCellBackground.h"
#import "LTTableViewCellSelectedBackground.h"
#import "LTTableViewCell.h"
#import "LTEntityRefreshProgressViewCell.h"
#import "LTCoreEditTableViewController.h"
#import "LTEntityTableViewController.h"
#import "LTGroupTree.h"
#import "LTTableView.h"
#import "LTTableViewSectionHeaderView.h"
#import "AppDelegate_Pad.h"
#import "LTRackTableViewHeaderView.h"
#import "LTHardwareEntityTableViewCell.h"
#import "LTDeviceEntityTableViewCell.h"
#import "LTContainerEntityTableViewCell.h"
#import "LTObjectEntityTableViewCell.h"
#import "LTMetricEntityTableViewCell.h"

@interface LTGroupTableViewController (private)
- (void) coreDeploymentArrayUpdated:(NSNotification *)notification;
- (void) entityRefreshFinished:(NSNotification *)notification;
- (void) sortAndFilterChildren;
- (void) refreshTimerFired:(NSTimer *)timer;
@end

@implementation LTGroupTableViewController

#pragma mark -
#pragma mark Preferences

- (BOOL) _showEntitiesInRootList
{
    return [[NSUserDefaults standardUserDefaults] boolForKey:@"LTSetupGroupListShowEntities"];
}

- (id)initWithStyle:(UITableViewStyle)style 
{
    // Override initWithStyle: if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
    if ((self = [super initWithStyle:style])) 
	{
		self.tableView.backgroundColor = [UIColor colorWithRed:32.0/255.0 green:32.0/255.0 blue:32.0/255.0 alpha:1.0];
		self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;		
    }
    return self;
}

- (void)viewDidLoad 
{
    [super viewDidLoad];	
    
    self.tableView.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"RackBackTile.png"]];

    self.pullToRefresh = YES;
}

- (void) awakeFromNib
{
	[super awakeFromNib];

	/* We are the root-level controller, observe core deployment changes */
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(groupTreeRefreshFinished:)
												 name:@"LTGroupTreeRefreshFinished" object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(deploymentRefreshFinished:)
												 name:@"LTCoreDeploymentRefreshFinished"
											   object:nil];

	/* Build item list */
	[self sortAndFilterChildren];
	
	/* Tableview setup */
	self.tableView.allowsSelectionDuringEditing = YES;
	self.navigationItem.title = @"Groups";
	
}

#pragma mark Memory Management

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)dealloc 
{
	[children release];
    [super dealloc];
}

#pragma mark "View Delegates"

- (void)viewWillAppear:(BOOL)animated 
{
    [super viewWillAppear:animated];
	[self refresh];
	[self sortAndFilterChildren];
	[[self tableView] reloadData];
    
    self.navigationController.navigationBar.tintColor = [UIColor colorWithWhite:0.29 alpha:1.0];
}

- (void)viewDidAppear:(BOOL)animated 
{
    [super viewDidAppear:animated];	
    
    refreshTimer = [NSTimer scheduledTimerWithTimeInterval:60.0
													target:self
												  selector:@selector(refreshTimerFired:)
												  userInfo:nil
												   repeats:YES];
}

- (void)viewWillDisappear:(BOOL)animated 
{
	[super viewWillDisappear:animated];	
}

- (void)viewDidDisappear:(BOOL)animated
{
    [refreshTimer invalidate];
    refreshTimer = nil;    
}

#pragma mark Refresh Timer

- (void) refresh
{
	if (group)
	{
		/* Refresh local group */
		[group refresh];
	}
	else
	{
		/* Refresh all customers */
		AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
		for (LTCoreDeployment *core in [appDelegate coreDeployments])
		{
			for (LTCustomer *customer in core.children)
			{
				[customer.groupTree setLastRefresh:nil];
				[customer.groupTree refresh];
			}
		}
	}	
    self.reloading = [self refreshInProgress];
}

- (void)forceRefresh
{
    if (group)
    {
        /* Force a refresh of single group */
        [group forceRefresh];
    }
    else
    {
        /* Force a refresh of all groups */
		AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
		for (LTCoreDeployment *core in [appDelegate coreDeployments])
		{
			for (LTCustomer *customer in core.children)
			{
				[customer.groupTree forceRefresh];
			}
		}
    }
    self.reloading = [self refreshInProgress];
}

- (void) refreshTimerFired:(NSTimer *)timer
{
	/* Chck if view is visible */
	if (self.isVisible)
	{
		AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
		for (LTCustomer *customer in [appDelegate valueForKeyPath:@"coreDeployments.@unionOfArrays.children"])
		{
			[customer.groupTree refresh];
		}
	}
}

- (BOOL) refreshInProgress
{
    if (group)
    { return group.refreshInProgress; }
    else
    {
		AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
		for (LTCustomer *customer in [appDelegate valueForKeyPath:@"coreDeployments.@unionOfArrays.children"])
        {
            LTGroupTree *groupTree = (LTGroupTree *)customer.groupTree;
            if (groupTree.refreshInProgress) return YES;
        }
        return NO;
    }
}

- (NSDate*)egoRefreshTableHeaderDataSourceLastUpdated:(EGORefreshTableHeaderView*)view
{
    if (self.group) 
    {
        return self.group.lastRefresh;
    }
    else
    {
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        NSDate *latestRefresh = nil;
        for (LTCustomer *customer in [appDelegate valueForKeyPath:@"coreDeployments.@unionOfArrays.children"])
        {
            LTGroupTree *groupTree = (LTGroupTree *)customer.groupTree;
            if (!latestRefresh || [groupTree.lastRefresh laterDate:latestRefresh] == groupTree.lastRefresh)
            { latestRefresh = groupTree.lastRefresh; }
        }
        return latestRefresh;
    }
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

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	if (group)
	{
		/* Single group */
		if ([children count] == 0 && group.refreshInProgress)
		{ return 1; }
		else
		{ return [children count]; }
	}
	else
	{
		/* Aggregated groups */
		if ([children count] > 0)
		{ 
            return [children count]; 
        }
		else 
		{
            /* No groups present, see if a refresh is in progress */
			AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
			for (LTCustomer *customer in [appDelegate valueForKeyPath:@"coreDeployments.@unionOfArrays.children"])
            {
                if ([customer.groupTree refreshInProgress]) 
                {
                    /* A refresh is in progress, use refresh indicator */
                    return 1;	
                }
			}
            
            /* No groups, no refresh -- no rows */
			return 0;
		}
	}
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	if (group)
	{
		/* Single group */
		if ([children count] == 0 && group.refreshInProgress)
		{ return [self.tableView frame].size.height; }
		else if ([[[children objectAtIndex:indexPath.row] class] isSubclassOfClass:[LTGroup class]])
        {
            return 28.0;
        }
        else
		{ 
            return 48.0; 
        }
	}
	else
	{
		/* Aggregated groups, or top-level of showing groups only */
		if ([children count] > 0)
		{ 
            BOOL childIsAGroup = [[[children objectAtIndex:indexPath.row] class] isSubclassOfClass:[LTGroup class]];
			if (childIsAGroup && [self _showEntitiesInRootList])
			{ 
                return 28.0; 
            }
			else 
			{
				return 48.0;
			}
		}
		else
		{
			/* No children/groups */
			AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
			for (LTCoreDeployment *core in [appDelegate coreDeployments])
			{
				for (LTCustomer *customer in core.children)
				{
					if ([customer.groupTree refreshInProgress]) 
					{
						return [self.tableView frame].size.height;
					}
				}
			}
			return 48.0;
		}
	}
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
    LTEntity *displayEntity = nil;
	if (indexPath.row < [children count])
	{ 
		displayEntity = [children objectAtIndex:indexPath.row]; 
	}
	
    NSString *CellIdentifier;
    /* Set CellIdentifier based on entity type */
    if (displayEntity.type >= 3)
    {
        switch (displayEntity.type) {
            case 3:
                CellIdentifier = @"Device";
                break;
            case 4:
                CellIdentifier = @"Container";
                break;
            case 5:
                CellIdentifier = @"Object";
                break;
            case 6:
                CellIdentifier = @"Metric";
                break;
            default:
                CellIdentifier = @"Hardware";
                break;
        }
    }
    else if (displayEntity)
    {
        BOOL entityIsAGroup = [displayEntity isMemberOfClass:[LTGroup class]];
        if (entityIsAGroup && (group || [self _showEntitiesInRootList])) CellIdentifier = @"GroupHeader";
        else CellIdentifier = @"EntityOrGroup"; 
    }
    else 
    { CellIdentifier = @"Refresh"; }
    
    /* Create or re-use a cell */
    LTEntityTableViewCell *cell = (LTEntityTableViewCell *) [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
	{
		if ([CellIdentifier isEqualToString:@"Device"])
		{
			cell = [[[LTDeviceEntityTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
		}
		else if ([CellIdentifier isEqualToString:@"Container"])
		{
			cell = [[[LTContainerEntityTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
		}
		else if ([CellIdentifier isEqualToString:@"Object"])
		{
			cell = [[[LTObjectEntityTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
		}
		else if ([CellIdentifier isEqualToString:@"Metric"])
		{
			cell = [[[LTMetricEntityTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
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
        else
        {
			cell = [[[LTEntityTableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:CellIdentifier] autorelease];
            cell.indentationWidth = 10.;
            cell.drawAsRack = YES;
            
            if ([CellIdentifier isEqualToString:@"GroupHeader"] && (group || [self _showEntitiesInRootList]))
            {
                cell.backgroundView = [[LTRackTableViewHeaderView alloc] initWithFrame:CGRectZero];
            }            
        }
    }
    
    /* Setup the cell */
	if (displayEntity)
	{
		cell.textLabel.text = displayEntity.desc;
		cell.entity = displayEntity;
		
		if ([displayEntity.customer.coreDeployment reachable] && [displayEntity.customer.coreDeployment enabled]) cell.imageView.alpha = 1.0;
		else cell.imageView.alpha = 0.5;
		cell.indentationLevel = displayEntity.indentLevel;			
        NSLog (@"Using indent level %i for %i:%@", displayEntity.indentLevel, displayEntity.type, displayEntity.desc);
		
        /* Setup Cell for Group Headers */
        if ([CellIdentifier isEqualToString:@"GroupHeader"])
        {
            LTRackTableViewHeaderView *headerView = (LTRackTableViewHeaderView *) cell.backgroundView;
            headerView.textLabel.text = cell.textLabel.text;
            headerView.indentLevel = cell.indentationLevel;
            cell.textLabel.text = nil;
        }
        
        if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) 
        { 
            /* iPhone style -- Accessory on all cells */
            cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator; 
        }
        else
        {
            /* iPad Style -- Accessory on groups only */
            if ([displayEntity isMemberOfClass:[LTGroup class]])
            {
                cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
            }
            else
            {
                cell.accessoryType = UITableViewCellAccessoryNone; 
            }
        }
	}
	else
	{
		/* Refresh info */
		LTEntityRefreshProgressViewCell *progressCell = (LTEntityRefreshProgressViewCell *) cell;
		if (group)
		{
			if (![group.coreDeployment enabled])
			{
				[progressCell.progressView stopAnimating];
				progressCell.progressLabel.text = @"Deployment is disabled.";
			}
			else if (![group.coreDeployment reachable])
			{
				[progressCell.progressView stopAnimating];
				progressCell.progressLabel.text = @"Deployment is unreachable.";
			}
			else
			{
				[progressCell.progressView startAnimating];
				progressCell.progressLabel.text = @"Downloading Group...";
			}
		}
		else
		{
			[progressCell.progressView startAnimating];
			progressCell.progressLabel.text = @"Downloading Groups";
		}
	}
	
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	LTEntity *selectedEntity = nil;
	if (group)
	{
		if (indexPath.row < [children count])
		{ selectedEntity = [children objectAtIndex:indexPath.row]; }
	}
	else
	{
		/* Select the deployment */
		selectedEntity = [children objectAtIndex:indexPath.row];
	}
	
	if (!selectedEntity) return;
	
	UIViewController *viewController = nil;
	if ([[selectedEntity class] isSubclassOfClass:[LTGroup class]])
	{
		/* Group Selected */
		LTGroup *selectedGroup = (LTGroup *)selectedEntity;
		LTGroupTableViewController *groupView = [[LTGroupTableViewController alloc] initWithNibName:@"LTGroupTableViewController" bundle:nil];
		groupView.group = selectedGroup;
		viewController = groupView;	
	}
	else 
	{
		/* Entity Selected */
		if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
		{
			if (selectedEntity.type == 6)
			{
				/* Metric */
				LTMetricTableViewController *metricView = [[LTMetricTableViewController alloc] initWithMetric:selectedEntity];
                [selectedEntity refresh];
				viewController = metricView;	
			}
			else
			{
				LTEntityTableViewController *anotherViewController = [[LTEntityTableViewController alloc] initWithEntity:selectedEntity];
				viewController = anotherViewController;
				[anotherViewController.tableView setNeedsDisplay];
			}			
		}
		else 
		{
			AppDelegate_Pad *appDelegate = (AppDelegate_Pad *) [[UIApplication sharedApplication] delegate];
			[appDelegate displayEntityInDetailView:selectedEntity];
		}
	}

	if (viewController)
	{
		[self.navigationController pushViewController:viewController animated:YES];
		[viewController release];
	}
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath 
{
	return NO;
}	

- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath
{
	[super tableView:tableView willDisplayCell:cell forRowAtIndexPath:indexPath];
	
	/* Refresh the entity just-in-time */
	LTEntity *displayEntity = nil;
	if (indexPath.row < [children count])
	{ displayEntity = [children objectAtIndex:indexPath.row]; }
	if (displayEntity.type == 6) [displayEntity refresh];
}

#pragma mark "Notifcation Receivers"

- (void) groupTreeRefreshFinished:(NSNotification *)notification
{
	[self sortAndFilterChildren];
	[[self tableView] reloadData];

    if (self.reloading && ![self refreshInProgress])
    {
        /* Cancel pull to refresh view */
        self.reloading = NO;
        [self.refreshHeaderView egoRefreshScrollViewDataSourceDidFinishedLoading:self.tableView];
    }
}

- (void) entityRefreshFinished:(NSNotification *)notification
{
	[[self tableView] reloadData];

    if (self.reloading && ![self refreshInProgress])
    {
        /* Cancel pull to refresh view */
        self.reloading = NO;
        [self.refreshHeaderView egoRefreshScrollViewDataSourceDidFinishedLoading:self.tableView];
    }
}

- (void) deploymentRefreshFinished:(NSNotification *)notification
{
	[self sortAndFilterChildren];
	[[self tableView] reloadData];
}

#pragma mark "Item Management"

- (void) recursivelyBuildChildrenUsingGroup:(LTGroup *)buildGroup
{
	for (id item in buildGroup.children)
	{
        BOOL itemIsAGroup = [[item class] isSubclassOfClass:[LTGroup class]];        
        if (itemIsAGroup || group || [self _showEntitiesInRootList])
        {
            /* Item is a group, or regardless of what it is this is not the 
             * root list, or if it is the root list, _showEntitiesInRootList is enabled
             */
            [children addObject:item];
        }

		if (itemIsAGroup) [self recursivelyBuildChildrenUsingGroup:item];
	}		
}

- (void) sortAndFilterChildren
{
	/* Remove observation of old */
	for (LTEntity *child in children)
	{
		[[NSNotificationCenter defaultCenter] removeObserver:self name:@"RefreshFinished" object:child];
	}
	[children release];
	children = [[NSMutableArray array] retain];
	
	if (group)
	{
		/* Representing a specific Group */
		[self recursivelyBuildChildrenUsingGroup:group];
	}
	else
	{
		/* Representing customer list */
		AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
		for (LTCoreDeployment *core in [appDelegate coreDeployments])
		{
			for (LTCustomer *customer in core.children)
			{
				[self recursivelyBuildChildrenUsingGroup:customer.groupTree];
			}
		}
	}

	for (LTEntity *child in children)
	{
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(entityRefreshFinished:)
													 name:@"RefreshFinished"
												   object:child];
	}	
}


#pragma mark "Properties"

- (NSString *) title
{ 
	if (group)
	{ return group.desc; }
	else
	{ return @"Groups"; }
}

@synthesize group;
- (void) setGroup:(LTGroup *)value
{
	if (group)
	{
		[[NSNotificationCenter defaultCenter] removeObserver:self name:@"RefreshFinished" object:group];
		[group release];
	}
	
	group = [value retain];
	[self sortAndFilterChildren];
	
	if (group)
	{
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(entityRefreshFinished:)
													 name:@"RefreshFinished" 
												   object:group];
	}
}	



@end

