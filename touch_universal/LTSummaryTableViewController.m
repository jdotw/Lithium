//
//  LTSummaryTableViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 11/10/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTSummaryTableViewController.h"
#import "LTSummaryIconView.h"
#import "LTIconTableViewCell.h"
#import "LTIconView.h"
#import "LTEntity.h"
#import "LTCoreDeployment.h"
#import "LTCustomer.h"
#import "AppDelegate.h"
#import "LTFavoritesTableViewController.h"
#import "LTIncidentListTableViewController.h"
#import "LTIncidentListGroup.h"

@interface LTSummaryTableViewController (Private)
- (void) rebuildDevicesArray;
@end

@implementation LTSummaryTableViewController

#pragma mark -
#pragma mark View lifecycle

- (void) awakeFromNib
{
	[super awakeFromNib];
	[self rebuildDevicesArray];
	[[self tableView] setAllowsSelection:NO];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(customerRefreshFinished:) name:@"LTCustomerRefreshFinished" object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(incidentArrayRebuilt:) name:@"IncidentArrayRebuilt" object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(favoritesRebuilt:) name:@"DisplayFavoritesRebuilt" object:nil];
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


- (void)viewDidAppear:(BOOL)animated 
{
    [super viewDidAppear:animated];
}


- (void)viewWillDisappear:(BOOL)animated 
{
    [super viewWillDisappear:animated];
	if (sidePopoverController) [sidePopoverController dismissPopoverAnimated:YES];
}

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
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	if (section == 0)
	{
		if ([incidents count] > 0) return DATA_INCIDENTS;
		else if ([appDelegate.favoritesController.displayFavorites count] > 0) return DATA_FAVORITES;
		else return DATA_DEVICES;
	}
	else if (section == 1)
	{
		if ([appDelegate.favoritesController.displayFavorites count] > 0) return DATA_FAVORITES;
		else return DATA_DEVICES;
	}
	else
	{
		return DATA_DEVICES;
	}
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	int sections = 1;
	if ([incidents count] > 0) sections++;
	if ([appDelegate.favoritesController.displayFavorites count] > 0) sections++;
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
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	NSArray *sourceArray = nil;
	switch ([self dataTypeInSection:section]) {
		case DATA_INCIDENTS:
			sourceArray = incidents;
			break;
		case DATA_FAVORITES:
			sourceArray = appDelegate.favoritesController.displayFavorites;
			break;
		case DATA_DEVICES:
			sourceArray = devices;
			break;
	}
	
	int itemsPerRow = [LTIconTableViewCell itemsPerRowAtHeight:self.rowHeight width:self.tableView.contentSize.width];
	int rows = [sourceArray count] / itemsPerRow;
	if ([sourceArray count] % itemsPerRow) rows++;
	
	return rows;
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{ 
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
    static NSString *CellIdentifier = @"Cell";
    
    LTIconTableViewCell *cell = (LTIconTableViewCell *) [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[LTIconTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Configure the cell...
	NSArray *entityArray;
	switch ([self dataTypeInSection:indexPath.section]) 
	{
		case DATA_INCIDENTS:
			entityArray = incidents;
			break;
		case DATA_FAVORITES:
			entityArray = appDelegate.favoritesController.displayFavorites;
			break;
		case DATA_DEVICES:
			entityArray = devices;
			break;
		default:
			entityArray = nil;
			break;
	}
	[cell removeAllSubviews];
	int i;
	int itemsPerRow = [LTIconTableViewCell itemsPerRowAtHeight:self.rowHeight width:self.tableView.contentSize.width];
	for (i=0; i < itemsPerRow; i++)
	{
		int index = i + (indexPath.row * itemsPerRow);
		if (index < [entityArray count])
		{
			LTEntity *entity = [entityArray objectAtIndex:index];
			LTSummaryIconView *iconView = [[LTSummaryIconView alloc] initWithEntity:entity];
			[cell addSubview:iconView];
			[iconView release];
			[iconView setNeedsLayout];
		}
	}	
	[cell setNeedsLayout];
    
    return cell;
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the specified item to be editable.
    return NO;
}


#pragma mark -
#pragma mark Table view delegate

- (NSIndexPath *) tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	/* Disable row selection */
	return nil;
}

#pragma mark -
#pragma mark Memory management

- (void)didReceiveMemoryWarning 
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Relinquish ownership any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload 
{
    // Relinquish ownership of anything that can be recreated in viewDidLoad or on demand.
    // For example: self.myOutlet = nil;
}


- (void)dealloc 
{
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"LTCustomerRefreshFinished" object:nil];
	[devices release];
    [super dealloc];
}

#pragma mark -
#pragma mark Entity Array Management

- (void) customerRefreshFinished:(NSNotification *)note
{
	/* Called whenever a customer entity has been refreshed */
	[self rebuildDevicesArray];
}

- (void) rebuildDevicesArray
{
	/* Rebuilds the devices array */
	if (!devices) devices = [[NSMutableArray array] retain];
	LTEntity *lastDevice = nil;
	NSMutableArray *seenDevices = [NSMutableArray array];
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	for (LTCoreDeployment *coreDeployment in appDelegate.coreDeployments)
	{
		for (LTCustomer *customer in coreDeployment.children)
		{
			for (LTEntity *site in customer.children)
			{
				for (LTEntity *device in site.children)
				{
					if (![devices containsObject:device])
					{
						if (lastDevice)
						{ [devices insertObject:device atIndex:[devices indexOfObject:lastDevice]+1]; }
						else 
						{ [devices insertObject:device atIndex:[devices count]]; }
					}
					[seenDevices addObject:device];
					lastDevice = device;
				}
			}
		}
	}
	if ([seenDevices count] < [devices count])
	{
		NSMutableArray *obsoleteDevices = [NSMutableArray array];
		for (LTEntity *device in devices)
		{
			if (![seenDevices containsObject:device]) [obsoleteDevices addObject:device];
		}
		for (LTEntity *obsoleteDevice in obsoleteDevices)
		{
			[devices removeObjectAtIndex:[devices indexOfObject:obsoleteDevice]];
		}
	}
	[self.tableView reloadData];
}

- (void) incidentArrayRebuilt:(NSNotification *)note
{
	if (!incidents)
	{
		incidents = [[NSMutableArray array] retain];
	}
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];	
	NSMutableArray *seen = [NSMutableArray array];
	for (LTIncidentListGroup *group in appDelegate.incidentsController.sortedChildren)
	{
		for (LTIncident *inc in group.children)
		{
			if (![incidents containsObject:inc.metric])
			{ [incidents addObject:inc.metric]; }
			if (![seen containsObject:inc.metric])
			{ [seen addObject:inc.metric]; }
		}
	}
	if (seen.count < incidents.count)
	{
		NSMutableArray *obsolete = [NSMutableArray array];
		for (LTEntity *entity in incidents)
		{
			if (![seen containsObject:entity])
			{ [obsolete addObject:entity]; }
		}
		for (LTEntity *entity in obsolete)
		{
			[incidents removeObject:entity];
		}
	}
	
	NSLog (@"Incidents rebuilt as %@", incidents);
				
	[self.tableView reloadData];
}

- (void) favoritesRebuilt:(NSNotification *)note
{
	[self.tableView reloadData];
}

@end

