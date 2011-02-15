//
//  LTIncidentListTableViewController.m
//  Lithium
//
//  Created by James Wilson on 28/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTIncidentListTableViewController.h"

#import "LTCoreDeployment.h"
#import "LTCustomer.h"
#import "LTIncidentListGroup.h"
#import "AppDelegate.h"
#import "AppDelegate_Pad.h"
#import "LTIncidentTableViewController.h"
#import "LTEntityDescriptor.h"
#import "LTEntityRefreshProgressViewCell.h"
#import "LTTableViewCell.h"
#import "LTIncidentTableViewCell.h"
#import "LTRackTableViewHeaderView.h"
#import "LTDeviceEntityTableViewCell.h"

@interface LTIncidentListTableViewController (Private)

- (void) rebuildIncidentsArray;

@end

#define SORT_BY_DEVICE 0
#define SORT_BY_TIME 1

@implementation LTIncidentListTableViewController

#pragma mark -
#pragma mark Initialization

- (void) awakeFromNib
{
	/* The awakeFromNib is only called for IncidentListTableViewControllers that 
	 * are instated from a NIB (i.e the root Incident List) and hence will only
	 * ever show the active incident list and not a historic list 
	 */
	[super awakeFromNib];	
    
    /* Setup rack appearance of table view */
    self.tableView.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"RackBackTile.png"]];

	/* Listen to refresh of any incident list (we are the global list if awoken from NIB) */
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(incidentListRefreshFinished:)
												 name:@"LTIncidentListRefreshFinished" 
											   object:nil];
	
	/* Listen to any chance in the incident count */
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(incidentListCountUpdated:)
												 name:@"LTIncidentListCountUpdated"
											   object:nil];	

	/* Create sort segment (Sort by Device | Sort by Time) */
	sortSegment = [[UISegmentedControl alloc] initWithItems:[NSArray arrayWithObjects:@"Sort by Device", @"Sort by Time", nil]];
	sortSegment.segmentedControlStyle = UISegmentedControlStyleBar;
	sortSegment.selectedSegmentIndex = [[NSUserDefaults standardUserDefaults] integerForKey:@"IncidentsViewMode"];
	[sortSegment addTarget:self
					action:@selector(sortTypeChanged:)
		  forControlEvents:UIControlEventValueChanged];
	self.navigationItem.titleView = sortSegment;
	
	/* Update array */
	[self rebuildIncidentsArray];
}

#pragma mark -
#pragma mark List Management

- (void) rebuildIncidentsArray
{
	int incidentCount = 0;
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	
	if (sortSegment && sortSegment.selectedSegmentIndex == 0)
	{
		/* Sort by device */
		NSMutableArray *array = [NSMutableArray array];
		NSMutableDictionary *deviceDict = [NSMutableDictionary dictionary];
		for (LTCoreDeployment *core in appDelegate.coreDeployments)
		{
			for (LTCustomer *customer in core.children)
			{
				for (LTIncident *incident in customer.incidentList.incidents)
				{
					NSString *key = [NSString stringWithFormat:@"%@:%@:%@", incident.entityDescriptor.custName, incident.entityDescriptor.siteName,  incident.entityDescriptor.devName];
					LTIncidentListGroup *group = [deviceDict objectForKey:key];
					if (!group)
					{
						group = [[LTIncidentListGroup new] autorelease];
                        group.title = incident.entityDescriptor.devDesc;
						[deviceDict setObject:group forKey:key];
						[array addObject:group];
					}
					[group.children addObject:incident];
					incidentCount++;
				}
			}
		}
        
        /* Sort by device desc and then incident severity */
        NSSortDescriptor *descSort = [NSSortDescriptor sortDescriptorWithKey:@"title" ascending:YES selector:@selector(localizedCompare:)];
        NSSortDescriptor *opStateSort = [NSSortDescriptor sortDescriptorWithKey:@"metric.opState" ascending:NO];
        [array sortUsingDescriptors:[NSArray arrayWithObject:descSort]];
		for (LTIncidentListGroup *group in array)
		{
			[group.children sortUsingDescriptors:[NSArray arrayWithObject:opStateSort]];
		}
		self.sortedChildren = array;
	}
	else if (!sortSegment || sortSegment.selectedSegmentIndex == 1)
	{
		/* Sort by Time */
		NSMutableArray *array = [NSMutableArray array];
		LTIncidentListGroup *lastHourGroup = [[LTIncidentListGroup new] autorelease];
		lastHourGroup.title = @"Last Hour";
		[array addObject:lastHourGroup];
		LTIncidentListGroup *lastDayGroup = [[LTIncidentListGroup new] autorelease];
		lastDayGroup.title = @"Last 24 Hours";
		[array addObject:lastDayGroup];
		LTIncidentListGroup *theRestGroup = [[LTIncidentListGroup new] autorelease];
		theRestGroup.title = @"Over 24 Hours Ago";
		[array addObject:theRestGroup];
		NSArray *incidents = nil;
		if (self.device) 
		{
			NSPredicate *deviceFilter = [NSPredicate predicateWithBlock:^(id evalObj, NSDictionary *bindings) { return [((LTIncident *)evalObj).entityDescriptor.devName isEqualToString:self.device.name]; }];
			incidents = [self.device.customer.incidentList.incidents filteredArrayUsingPredicate:deviceFilter];
		}
		else incidents = [appDelegate valueForKeyPath:@"coreDeployments.@unionOfArrays.children.@unionOfArrays.incidentList.incidents"];
		for (LTIncident *incident in incidents)
		{
			NSTimeInterval activeInterval = [[NSDate date] timeIntervalSinceDate:incident.startDate];
			if (activeInterval < (60.0 * 60.0))
			{
				/* Last Hour */
				[lastHourGroup.children addObject:incident];
			}
			else if (activeInterval < (60.0 * 60.0 * 24.0))
			{
				/* Last 24 Hours */
				[lastDayGroup.children addObject:incident];
			}
			else
			{
				/* The Rest */
				[theRestGroup.children addObject:incident];
			}
			incidentCount++;
		}
		NSSortDescriptor *sortDesc = [[NSSortDescriptor alloc] initWithKey:@"startDate" ascending:NO];
		[lastHourGroup.children sortUsingDescriptors:[NSArray arrayWithObject:sortDesc]];
		[lastDayGroup.children sortUsingDescriptors:[NSArray arrayWithObject:sortDesc]];
		[theRestGroup.children sortUsingDescriptors:[NSArray arrayWithObject:sortDesc]];
		[sortDesc release];
		if (lastHourGroup.children.count < 1) [array removeObject:lastHourGroup];
		if (lastDayGroup.children.count < 1) [array removeObject:lastDayGroup];
		if (theRestGroup.children.count < 1) [array removeObject:theRestGroup];
		self.sortedChildren = array;
	}
	
	[[self tableView] reloadData];
}

#pragma mark -
#pragma mark Refresh

- (BOOL) refreshInProgress
{
    if (device)
    { return device.customer.incidentList.refreshInProgress; }
    else
    {
        AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
        for (LTCustomer *customer in [appDelegate valueForKeyPath:@"coreDeployments.@unionOfArrays.children"])
        {
            if (customer.incidentList.refreshInProgress) return YES;
        }
        return NO;
    }
}

- (void) refresh
{
	if (device)
	{
        /* Refresh just the device's customer incidentList */
		[device.customer.incidentList refresh];
	}
	else 
	{
        /* Refreshes all incident lists */
		AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
		for (LTCustomer *customer in [appDelegate valueForKeyPath:@"coreDeployments.@unionOfArrays.children"])
		{ 
			[customer.incidentList refresh]; 
		}		
	}
    self.reloading = [self refreshInProgress];
}

- (void) forceRefresh
{
	if (device)
	{
        /* Refresh just the device's customer incidentList */
		[device.customer.incidentList forceRefresh];
	}
	else 
	{
        /* Refreshes all incident lists */
		AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
		for (LTCustomer *customer in [appDelegate valueForKeyPath:@"coreDeployments.@unionOfArrays.children"])
		{ 
			[customer.incidentList forceRefresh]; 
		}		
	}
    self.reloading = [self refreshInProgress];    
}

- (void) refreshTimerFired:(NSTimer *)timer
{
	/* Auto-refresh timer fired */
    [self refresh];
}

- (NSDate *) egoRefreshTableHeaderDataSourceLastUpdated:(EGORefreshTableHeaderView*)view
{	
    if (device)
    {
        return self.device.customer.incidentList.lastRefresh;
    }
    else
    {
        NSDate *date = nil;
		AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
		for (LTCustomer *customer in [appDelegate valueForKeyPath:@"coreDeployments.@unionOfArrays.children"])
		{ 
            if (!date || [customer.incidentList.lastRefresh laterDate:date] == customer.incidentList.lastRefresh)
            { date = customer.incidentList.lastRefresh; }
        }
        return date;
    }
}

- (void) incidentListRefreshFinished:(NSNotification *)notification
{
	/* An Incident List has been refreshed, rebuild our array */
	[self rebuildIncidentsArray];
    
    /* Cancel pull to refresh */
    if (self.reloading && ![self refreshInProgress])
    {
        /* Cancel pull to refresh view */
        self.reloading = NO;
        [self.refreshHeaderView egoRefreshScrollViewDataSourceDidFinishedLoading:self.tableView];
    }        
}

- (void) incidentListCountUpdated:(NSNotification *)notification
{
	/* An incident list had its count updated
	 * If we are visible, request a full list refresh
	 */
	
	if (self.isVisible)
	{
		LTIncidentList *list = [notification object];
		[list refresh];
	}
}

- (IBAction) sortTypeChanged:(id)sender
{
	/* Sort type changed, rebuild array */
	[[NSUserDefaults standardUserDefaults] setInteger:sortSegment.selectedSegmentIndex forKey:@"IncidentsViewMode"];
	[[NSUserDefaults standardUserDefaults] synchronize];
	[self rebuildIncidentsArray];
}

#pragma mark -
#pragma mark View Delegates

- (void)viewDidLoad 
{
    [super viewDidLoad];
	
	self.tableView.backgroundColor = [UIColor colorWithWhite:0.29 alpha:1.0];
	self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
	self.navigationItem.title = @"Incidents";
    self.pullToRefresh = YES;
	
	/* Refresh */
	[self refresh];
}

- (void)viewWillAppear:(BOOL)animated 
{
    [super viewWillAppear:animated];
    self.navigationController.navigationBar.tintColor = [UIColor colorWithWhite:0.29 alpha:1.0];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    /* Add auto-refresh timer */
	refreshTimer = [NSTimer scheduledTimerWithTimeInterval:60.0
													target:self
												  selector:@selector(refreshTimerFired:)
												  userInfo:nil
												   repeats:YES];
}

- (void) viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    [refreshTimer invalidate];
    refreshTimer = nil;
}

- (CGSize) contentSizeForViewInPopover
{
	return CGSizeMake(320.0, 200.0);
}

#pragma mark -
#pragma mark Table View Delegate Methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
	if (sortedChildren.count > 0)
	{ return sortedChildren.count; }
	else 
	{ return 1; }
}

- (CGFloat) tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    if (sortSegment && sortSegment.selectedSegmentIndex == SORT_BY_DEVICE) return 48.;
    else if (!sortSegment || sortSegment.selectedSegmentIndex == SORT_BY_TIME) return 28.; 
    else return 20.;
}

- (UIView *) tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
{
    if (sortSegment && sortSegment.selectedSegmentIndex == SORT_BY_DEVICE && section < sortedChildren.count)
    {
        /* Use device view */
        LTDeviceEntityTableViewCell *deviceView = [[LTDeviceEntityTableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:@"dummy"];
        LTIncidentListGroup *group = [sortedChildren objectAtIndex:section];
        deviceView.drawAsRack = YES;
        deviceView.textLabel.text = group.title;
        deviceView.entityState = group.highestEntityState;
        return [deviceView autorelease];
    }
    if (!sortSegment || (sortSegment.selectedSegmentIndex == SORT_BY_TIME && section < sortedChildren.count))
    {
        /* Use rack section header */
        LTRackTableViewHeaderView *header = [[LTRackTableViewHeaderView alloc] initWithFrame:CGRectZero]; 
        header.textLabel.text = [self tableView:tableView titleForHeaderInSection:section];
        if (!sortSegment) header.showScrews = NO;
        return [header autorelease];
    }
    else return nil;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
	if (sortedChildren.count > 0)
	{
		LTIncidentListGroup *group = (LTIncidentListGroup *) [sortedChildren objectAtIndex:section];
		return group.title;
	}
	else return nil;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	if (sortedChildren.count > 0)
	{ 
		LTIncidentListGroup *group = (LTIncidentListGroup *) [sortedChildren objectAtIndex:section];
		return group.children.count; 
	}
	else
	{ return 1; }
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	if ([sortedChildren count] == 0)
	{ 
        return [self.tableView frame].size.height; 
    }
	else
	{ return 38.0; }    // Ticket image height is 40., use 32. to overlap
}

- (NSString *) incidentActiveIntervalString:(LTIncident *)incident
{
	time_t interval = (time_t) (time_t ) [[NSDate date] timeIntervalSinceDate:incident.startDate];
	unsigned int days;
	unsigned int hours;
	unsigned int minutes;
	
	if (interval >= 60)
	{
		days = interval / (60 * 60 * 24);
		interval %= (60 * 60 * 24);
		
		hours = interval / (60 * 60);
		interval %= (60 * 60);
		
		minutes = interval / 60;
		
		if (days == 0 && hours == 0)
		{ return [NSString stringWithFormat:@"%im", minutes]; }
		else if (days == 0)
		{ return [NSString stringWithFormat:@"%ih %im", hours, minutes]; }
		else
		{ return [NSString stringWithFormat:@"%id %ih %im", days, hours, minutes]; }
	}
	else
	{
		return [NSString stringWithFormat:@"%lis", interval];
	}	
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{	
    NSString *CellIdentifier = @"Default";
	
	if (sortedChildren.count < 1)
	{ 
		/* There's no incidents to display, show a progress row with 
		 * either the "No Incidents" or "Downloading Incident List..." text
		 */
		LTEntityRefreshProgressViewCell *cell = (LTEntityRefreshProgressViewCell *) [tableView dequeueReusableCellWithIdentifier:@"Refresh"];
		if (!cell)
		{ 
			cell = [[[LTEntityRefreshProgressViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"Refresh"] autorelease];
		}
		if (self.refreshInProgress)
		{
			[cell.progressView startAnimating];
			cell.progressLabel.text = @"Downloading Incident List...";
		}
		else 
		{
			[cell.progressView stopAnimating];
			cell.progressLabel.text = @"No Incidents";
		}
		return cell;
	}
    
    LTIncidentTableViewCell *cell = (LTIncidentTableViewCell *) [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
	{
		cell = [[[LTIncidentTableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:CellIdentifier] autorelease];
    }
	
    // Set up the cell...
	LTIncidentListGroup *group = (LTIncidentListGroup *) [sortedChildren objectAtIndex:indexPath.section];
	LTIncident *incident = [group.children objectAtIndex:[indexPath row]];
	if (sortSegment && sortSegment.selectedSegmentIndex == 0)
	{
		/* By-Device Display */
		cell.incidentLabel.text = [NSString stringWithFormat:@"%@ %@ %@ %@", incident.entityDescriptor.cntDesc, incident.entityDescriptor.objDesc, incident.entityDescriptor.metDesc, incident.entityDescriptor.trgDesc];
		cell.detailTextLabel.text = [NSString stringWithFormat:@"Reached or exceeded %@ for %@", incident.raisedValue, [self incidentActiveIntervalString:incident]];
	}
	else
	{
		/* By-Time Display */
		if (![incident.entityDescriptor.siteName isEqualToString:@"default"])
		{ cell.incidentLabel.text = [NSString stringWithFormat:@"%@ %@ @ %@", incident.entityDescriptor.devDesc, incident.entityDescriptor.cntDesc, incident.entityDescriptor.siteDesc]; }
		else
		{ cell.incidentLabel.text = [NSString stringWithFormat:@"%@ %@", incident.entityDescriptor.devDesc, incident.entityDescriptor.cntDesc]; } 
		cell.detailTextLabel.text = [NSString stringWithFormat:@"%@ %@ Reached or exceeded %@", incident.entityDescriptor.objDesc, incident.entityDescriptor.metDesc, incident.raisedValue];
	}
	cell.entityState = incident.entityDescriptor.opState;
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{ cell.accessoryType = UITableViewCellAccessoryNone; }
	else
	{ cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator; }
    cell.row = indexPath.row;
    if (cell.row == 0) cell.firstRow = YES;
    else cell.firstRow = NO;
    if (cell.row == ([self tableView:tableView numberOfRowsInSection:indexPath.section]-1)) 
    { cell.lastRow = YES; }
    else
    { cell.lastRow = NO; }
	
    return cell;
}

- (NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	if (sortedChildren.count < 1) return nil;
	else return indexPath;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	if (sortedChildren.count < 1) return;
	
	LTIncidentListGroup *group = (LTIncidentListGroup *) [sortedChildren objectAtIndex:indexPath.section];
	LTIncident *incident = [group.children objectAtIndex:[indexPath row]];
	
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		/* iPad-style Incident selection */
		AppDelegate_Pad *appDelegate = (AppDelegate_Pad *) [[UIApplication sharedApplication] delegate];
		[appDelegate displayEntityInDetailView:incident.metric];
	}
	else
	{
		/* iPhone-style Incident selection 
         *
         * Use a copy of the metric so that the metric is disposed of
         * after the metric tableview is dealloced 
         */
		LTIncidentTableViewController *incidentView = [[LTIncidentTableViewController alloc] initWithMetric:[[incident.metric copy] autorelease]];
		incidentView.incident = incident;
		LTEntityDescriptor *entityDescriptor = [incidentView.metric entityDescriptor];
		incidentView.navigationItem.prompt = [NSString stringWithFormat:@"%@ @ %@ %@ %@", entityDescriptor.devDesc, entityDescriptor.siteDesc, entityDescriptor.cntDesc, entityDescriptor.objDesc];
		[incidentView.metric refresh];
		[self.navigationController pushViewController:incidentView animated:YES];
		[incidentView release];
	}
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath 
{
	if (sortedChildren.count > 0)
	{ return YES; }
	else 
	{ return NO; }

}

- (NSString *) tableView:(UITableView *)tableView titleForDeleteConfirmationButtonForRowAtIndexPath:(NSIndexPath *)indexPath
{
	return @"Clear";
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath 
{
	/* Handle clearing an incident by swiping it */
    if (editingStyle == UITableViewCellEditingStyleDelete) 
	{
		/* Clear the Incident */
		LTIncidentListGroup *group = (LTIncidentListGroup *) [sortedChildren objectAtIndex:indexPath.section];
		LTIncident *incident = [group.children objectAtIndex:[indexPath row]];
		if (incident.metric.coreDeployment.reachable)
		{
			/* Clear the incident (removes it from live customer.incidentList) */
			[incident clear];
			
			/* Remove from sorted group to allow table view delete */
			[group.children removeObjectAtIndex:indexPath.row];
			
			/* Delete row in table view */
			[self.tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath]
								  withRowAnimation:YES];
			
			/* If we're showing incident list for a specific device, refresh it */
			[self.device refresh];
		}
		else 
		{
			[incident.metric.coreDeployment showUnreachableAlert];
		}
    }   
}

#pragma mark -
#pragma mark Memory Management

- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}


- (void)dealloc 
{
	if (self.device)
	{
		[[NSNotificationCenter defaultCenter] removeObserver:self 
														name:@"LTIncidentListRefreshFinished"  
													  object:self.device.customer.incidentList];
		[device release];
	}		
	[[NSNotificationCenter defaultCenter] removeObserver:self 
													name:@"LTIncidentListRefreshFinished"  
												  object:nil];
	[sortedChildren release];
    [super dealloc];
}

#pragma mark -
#pragma mark Properties

@synthesize sortedChildren, device;

- (void) setDevice:(LTEntity *)value
{
	[device release];
	device = [value retain];

	/* Listen to refresh just from the device->customer's incident list */
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(incidentListRefreshFinished:)
												 name:@"LTIncidentListRefreshFinished" 
											   object:device.customer.incidentList];	
	
	[self rebuildIncidentsArray];
}

@end

