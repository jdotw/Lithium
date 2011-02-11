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

@interface LTGroupTableViewController (private)
- (void) coreDeploymentArrayUpdated:(NSNotification *)notification;
- (void) entityRefreshFinished:(NSNotification *)notification;
- (void) sortAndFilterChildren;
- (void) refreshTimerFired:(NSTimer *)timer;
@end

@implementation LTGroupTableViewController

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

	refreshTimer = [NSTimer scheduledTimerWithTimeInterval:60.0
													target:self
												  selector:@selector(refreshTimerFired:)
												  userInfo:nil
												   repeats:YES];
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
	
	/* Tableview set */
	self.tableView.allowsSelectionDuringEditing = YES;

	/* Create segment control */
	displayStyleSegment = [[UISegmentedControl alloc] initWithItems:[NSArray arrayWithObjects:@"Groups Tree View", @"Top Level Groups", nil]];
	displayStyleSegment.segmentedControlStyle = UISegmentedControlStyleBar;
	displayStyleSegment.selectedSegmentIndex = [[NSUserDefaults standardUserDefaults] integerForKey:@"GroupsViewMode"];
	[displayStyleSegment addTarget:self
							action:@selector(displayStyleChanged:)
				  forControlEvents:UIControlEventValueChanged];
	self.navigationItem.titleView = displayStyleSegment;
	
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
	[refreshTimer invalidate];
    [super dealloc];
}

#pragma mark "View Delegates"

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Overriden to allow any orientation.
    return YES;
}

- (void)viewWillAppear:(BOOL)animated 
{
    [super viewWillAppear:animated];
	[self refresh];
	[self sortAndFilterChildren];
	[[self tableView] reloadData];
}

- (void)viewDidAppear:(BOOL)animated 
{
    [super viewDidAppear:animated];	
}

- (void)viewWillDisappear:(BOOL)animated 
{
	[super viewWillDisappear:animated];	
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
		else
		{ return 48.0; }
	}
	else
	{
		/* Aggregated groups, or top-level of showing groups only */
		if ([children count] > 0)
		{ 
			if ([[[children objectAtIndex:indexPath.row] class] isSubclassOfClass:[LTGroup class]] && [displayStyleSegment selectedSegmentIndex] == 0)
			{ return 28.0; }
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
						return [self.tableView frame].size.height;;
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
	if (displayEntity.type == 6)
	{ CellIdentifier = @"Metric"; }
	else if (displayEntity)
	{
		if ([displayEntity isMemberOfClass:[LTGroup class]] && [displayStyleSegment selectedSegmentIndex] == 0) CellIdentifier = @"GroupHeader";
		else CellIdentifier = @"EntityOrGroup"; 
	}
	else 
	{ CellIdentifier = @"Refresh"; }
    LTEntityTableViewCell *cell = (LTEntityTableViewCell *) [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
	{
		if ([CellIdentifier isEqualToString:@"Refresh"])
		{
			cell = [[[LTEntityRefreshProgressViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
			cell.selectionStyle = UITableViewCellSelectionStyleNone;
		}
		else 
		{
			cell = [[[LTEntityTableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:CellIdentifier] autorelease];
		}

		if (displayEntity) 
		{
			if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) 
			{ cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator; }
			if ([displayEntity isMemberOfClass:[LTGroup class]])
			{
				cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
			}
			if ([CellIdentifier isEqualToString:@"GroupHeader"])
			{
				cell.backgroundView = [[LTRackTableViewHeaderView alloc] initWithFrame:CGRectZero];
			}
			
		}
		
    }
    
    // Set up the cell...
	if (displayEntity)
	{
		cell.textLabel.text = displayEntity.desc;
		cell.entity = displayEntity;
		
		if ([displayEntity.customer.coreDeployment reachable] && [displayEntity.customer.coreDeployment enabled]) cell.imageView.alpha = 1.0;
		else cell.imageView.alpha = 0.5;
		
		if (displayStyleSegment && [displayStyleSegment selectedSegmentIndex] == 0)
		{
			cell.indentationLevel = displayEntity.indentLevel;			
		}
		else
		{
			cell.indentationLevel = 0;
		}
		
//		cell.metricLabel.text = displayEntity.longDisplayString;
//		cell.deviceLabel.text = displayEntity.longLocationString;
		if (displayEntity.type > 3) cell.showFullLocation = YES;
		else cell.showFullLocation = NO;
		if (displayEntity.type == 6) cell.showCurrentValue = YES;
		else cell.showCurrentValue = NO;
        
        /* Setup Cell for Group Headers */
        if ([CellIdentifier isEqualToString:@"GroupHeader"])
        {
            LTRackTableViewHeaderView *headerView = (LTRackTableViewHeaderView *) cell.backgroundView;
            headerView.textLabel.text = cell.textLabel.text;
            headerView.indentLevel = cell.indentationLevel;
            NSLog (@"Section DisplayEntity is %@, indentLevel is %i", displayEntity.desc, headerView.indentLevel);
            cell.textLabel.text = nil;
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

#pragma mark "KVO Callbacks"

- (void) groupTreeRefreshFinished:(NSNotification *)notification
{
	[self sortAndFilterChildren];
	[[self tableView] reloadData];
}

- (void) entityRefreshFinished:(NSNotification *)notification
{
	[[self tableView] reloadData];
}

- (void) deploymentRefreshFinished:(NSNotification *)notification
{
	[self sortAndFilterChildren];
	[[self tableView] reloadData];
}

#pragma mark "Item Management"

- (IBAction) displayStyleChanged:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setInteger:[displayStyleSegment selectedSegmentIndex] forKey:@"GroupsViewMode"];
	[self sortAndFilterChildren];
	[[self tableView] reloadData];
}

- (void) recursivelyBuildChildrenUsingGroup:(LTGroup *)buildGroup
{
	for (id item in buildGroup.children)
	{
		[children addObject:item];
		if ([[item class] isSubclassOfClass:[LTGroup class]] && (displayStyleSegment && [displayStyleSegment selectedSegmentIndex] == 0))
		{
			[self recursivelyBuildChildrenUsingGroup:item];
		}
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

