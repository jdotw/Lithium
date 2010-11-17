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

@implementation LTIncidentListTableViewController

- (void) awakeFromNib
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	[super awakeFromNib];	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(coreDeploymentArrayUpdated:)
												 name:@"CoreDeploymentAdded" object:appDelegate];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(coreDeploymentArrayUpdated:)
												 name:@"CoreDeploymentRemoved" object:appDelegate];	

	self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh
																							target:self action:@selector(refreshTouched:)] autorelease];

	sortSegment = [[UISegmentedControl alloc] initWithItems:[NSArray arrayWithObjects:@"Sort by Device", @"Sort by Time", nil]];
	sortSegment.segmentedControlStyle = UISegmentedControlStyleBar;
	sortSegment.selectedSegmentIndex = [[NSUserDefaults standardUserDefaults] integerForKey:@"IncidentsViewMode"];
	[sortSegment addTarget:self
					action:@selector(sortTypeChanged:)
		  forControlEvents:UIControlEventValueChanged];
	self.navigationItem.titleView = sortSegment;

	[self coreDeploymentArrayUpdated:nil];
	[self rebuildIncidentsArray];
}

- (IBAction) sortTypeChanged:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setInteger:sortSegment.selectedSegmentIndex forKey:@"IncidentsViewMode"];
	[[NSUserDefaults standardUserDefaults] synchronize];
	[self rebuildIncidentsArray];
}

- (void) coreDeploymentArrayUpdated:(NSNotification *)notification
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	for (LTCoreDeployment *core in appDelegate.coreDeployments)
	{
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(coreDeploymentRefreshFinished:)
													 name:@"RefreshFinished" 
												   object:core];		
	}	
	[self rebuildIncidentsArray];
}

- (void) coreDeploymentRefreshFinished:(NSNotification *)notification
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	for (LTCoreDeployment *core in appDelegate.coreDeployments)
	{
		for (LTCustomer *customer in core.children)
		{
			[[NSNotificationCenter defaultCenter] addObserver:self
													 selector:@selector(incidentListRefreshFinished:)
														 name:@"IncidentListRefreshFinished" 
													   object:customer.incidentList];
		}
	}	
	[self rebuildIncidentsArray];
}

- (void) incidentListRefreshFinished:(NSNotification *)notification
{
	[self rebuildIncidentsArray];
}

- (void) rebuildIncidentsArray
{
	int incidentCount = 0;
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	
	if (sortSegment.selectedSegmentIndex == 0)
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
						if ([incident.entityDescriptor.siteName isEqualToString:@"default"])
						{ group.title = [NSString stringWithFormat:@"%@", incident.entityDescriptor.devDesc];	}
						else
						{ 
							group.title = [NSString stringWithFormat:@"%@ @ %@", incident.entityDescriptor.devDesc, incident.entityDescriptor.siteDesc];
							if ([incident.entityDescriptor.siteSuburb length] > 0) 
							{ group.title = [NSString stringWithFormat:@"%@ (%@)", group.title, incident.entityDescriptor.siteSuburb]; }
						}
						[deviceDict setObject:group forKey:key];
						[array addObject:group];
					}
					[group.children addObject:incident];
					incidentCount++;
				}
			}
		}
		NSSortDescriptor *sortDesc = [[NSSortDescriptor alloc] initWithKey:@"startDate" ascending:NO];
		for (LTIncidentListGroup *group in array)
		{
			[group.children sortUsingDescriptors:[NSArray arrayWithObject:sortDesc]];
		}
		[sortDesc release];
		self.sortedChildren = array;
	}
	else if (sortSegment.selectedSegmentIndex == 1)
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
		for (LTIncident *incident in [appDelegate valueForKeyPath:@"coreDeployments.@unionOfArrays.children.@unionOfArrays.incidentList.incidents"])
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
	
	if ([sortedChildren count] > 0)
	{ 
		[self.navigationController.tabBarItem setBadgeValue:[NSString stringWithFormat:@"%i", incidentCount]]; 
		[[UIApplication sharedApplication] setApplicationIconBadgeNumber:incidentCount];
	}
	else 
	{
		[self.navigationController.tabBarItem setBadgeValue:nil];
		[[UIApplication sharedApplication] setApplicationIconBadgeNumber:0];
	}
	
	[[NSNotificationCenter defaultCenter] postNotificationName:@"IncidentArrayRebuilt" object:self];
}

- (IBAction) refreshTouched:(id)sender
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	for (LTCustomer *customer in [appDelegate valueForKeyPath:@"coreDeployments.@unionOfArrays.children"])
	{ 
		[customer.incidentList refresh]; 
	}
}

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

//    self.navigationItem.rightBarButtonItem = self.editButtonItem;
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}

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


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation 
{
    return YES;
}

- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

#pragma mark Table view methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
	if (sortedChildren.count > 0)
	{ return sortedChildren.count; }
	else 
	{ return 1; }
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



// Customize the number of rows in the table view.
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
	{ return [self.tableView frame].size.height; }
	else
	{ return 44.0; }
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

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{	
    NSString *CellIdentifier = @"Default";
	
	if (sortedChildren.count < 1)
	{ 
		LTEntityRefreshProgressViewCell *cell = (LTEntityRefreshProgressViewCell *) [tableView dequeueReusableCellWithIdentifier:@"Refresh"];
		if (!cell)
		{ 
			cell = [[[LTEntityRefreshProgressViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"Refresh"] autorelease];
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

		}
		return cell;
	}
    
    LTTableViewCell *cell = (LTTableViewCell *) [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
	{
		cell = [[[LTTableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:CellIdentifier] autorelease];
    }
	
    // Set up the cell...
	LTIncidentListGroup *group = (LTIncidentListGroup *) [sortedChildren objectAtIndex:indexPath.section];
	LTIncident *incident = [group.children objectAtIndex:[indexPath row]];
	cell.textLabel.font = [UIFont boldSystemFontOfSize:16.0];
	cell.detailTextLabel.font = [UIFont systemFontOfSize:10.0];
	if (sortSegment.selectedSegmentIndex == 0)
	{
		/* By-Device Display */
		cell.textLabel.text = [NSString stringWithFormat:@"%@ %@ %@ %@", incident.entityDescriptor.cntDesc, incident.entityDescriptor.objDesc, incident.entityDescriptor.metDesc, incident.entityDescriptor.trgDesc];
		cell.detailTextLabel.text = [NSString stringWithFormat:@"Reached or exceeded %@ for %@", incident.raisedValue, [self incidentActiveIntervalString:incident]];
	}
	else
	{
		/* By-Time Display */
		if (![incident.entityDescriptor.siteName isEqualToString:@"default"])
		{ cell.textLabel.text = [NSString stringWithFormat:@"%@ %@ @ %@", incident.entityDescriptor.devDesc, incident.entityDescriptor.cntDesc, incident.entityDescriptor.siteDesc]; }
		else
		{ cell.textLabel.text = [NSString stringWithFormat:@"%@ %@", incident.entityDescriptor.devDesc, incident.entityDescriptor.cntDesc]; } 
		cell.detailTextLabel.text = [NSString stringWithFormat:@"%@ %@ Reached or exceeded %@", incident.entityDescriptor.objDesc, incident.entityDescriptor.metDesc, incident.raisedValue];
	}
	cell.entityState = incident.entityDescriptor.opState;
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{ cell.accessoryType = UITableViewCellAccessoryNone; }
	else
	{ cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator; }
	
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
		AppDelegate_Pad *appDelegate = [[UIApplication sharedApplication] delegate];
		[appDelegate displayEntityInDetailView:incident.metric];
	}
	else
	{
		/* iPhone-style Incident selection */
		LTIncidentTableViewController *incidentView = [[LTIncidentTableViewController alloc] initWithStyle:UITableViewStylePlain];
		incidentView.incident = incident;
		incidentView.metric = incident.metric;
		LTEntityDescriptor *entityDescriptor = [incident.metric entityDescriptor];
		incidentView.navigationItem.prompt = [NSString stringWithFormat:@"%@ @ %@ %@ %@", entityDescriptor.devDesc, entityDescriptor.siteDesc, entityDescriptor.cntDesc, entityDescriptor.objDesc];
		[incident.metric refresh];
		[self.navigationController pushViewController:incidentView animated:YES];
		[incidentView release];
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

- (void)dealloc 
{
	[sortedChildren release];
    [super dealloc];
}

@synthesize sortedChildren;

- (BOOL) refreshInProgress
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	for (LTCoreDeployment *core in appDelegate.coreDeployments)
	{
		for (LTCustomer *customer in core.children)
		{
			if (customer.incidentList.refreshInProgress) return YES;
		}
	}
	return NO;
}

@end

