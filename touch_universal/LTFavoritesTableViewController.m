//
//  LTFavoritesTableViewController.m
//  Lithium
//
//  Created by James Wilson on 16/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTFavoritesTableViewController.h"
#import "LTEntityTableViewCell.h"
#import "LTMetricGraphTableViewCell.h"
#import "LTMetricTableViewController.h"
#import "LTEntityDescriptor.h"
#import "LTEntityTableViewController.h"
#import "AppDelegate_Pad.h"

@implementation LTFavoritesTableViewController

@synthesize displayFavorites;

- (void) awakeFromNib
{
	[super awakeFromNib];
	
	/* Create Segment Control */
	displaySegment = [[UISegmentedControl alloc] initWithItems:[NSArray arrayWithObjects:@"View as Graphs", @"View as Table", nil]];
	displaySegment.segmentedControlStyle = UISegmentedControlStyleBar;
	displaySegment.selectedSegmentIndex = [[NSUserDefaults standardUserDefaults] integerForKey:@"FavoritesViewMode"];
	[displaySegment addTarget:self
					   action:@selector(displayTypeChanged:)
			 forControlEvents:UIControlEventValueChanged];
	self.navigationItem.titleView = displaySegment;	
	self.navigationItem.rightBarButtonItem = self.editButtonItem;
	
	/* Create arrays */
	favorites = [[NSMutableArray array] retain];
	displayFavorites = [[NSMutableArray array] retain];
	displayOrphans = [[NSMutableArray array] retain];

	/* Decode data */
	NSData *data = [[NSUserDefaults standardUserDefaults] objectForKey:@"favoritesData"];
	if (data)
	{
		descriptorDict = [[NSMutableDictionary dictionaryWithDictionary:[NSKeyedUnarchiver unarchiveObjectWithData:data]] retain];
		orphanDict = [[NSMutableDictionary dictionaryWithDictionary:[NSKeyedUnarchiver unarchiveObjectWithData:data]] retain];
	}
}

- (IBAction) displayTypeChanged:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setInteger:displaySegment.selectedSegmentIndex forKey:@"FavoritesViewMode"];
	[[NSUserDefaults standardUserDefaults] synchronize];
	[self rebuildDisplayFavorites];
	[self.tableView reloadData];
}

- (void)viewDidLoad 
{
    [super viewDidLoad];
}

- (void) saveFavorites
{
	NSData *data = [NSKeyedArchiver archivedDataWithRootObject:descriptorDict];
	[[NSUserDefaults standardUserDefaults] setObject:data forKey:@"favoritesData"];
	[[NSUserDefaults standardUserDefaults] synchronize];	
}

- (void) observeEntity:(LTEntity *)entity
{
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(entityRefreshFinished:)
												 name:@"RefreshFinished" 
											   object:entity];
}

- (void) unobserveEntity:(LTEntity *)entity
{
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"RefreshFinished" object:entity];
}

- (void) addToFavorites:(LTEntity *)entity
{
	/* Get deviceEntityAddress */
	NSString *deviceEntityAddress = nil;
	if (entity.deviceEntityAddress)
	{
		/* Entity is live */
		deviceEntityAddress = entity.deviceEntityAddress;
	}
	else
	{
		/* Entity is orphaned... create an address string */
		deviceEntityAddress = [NSString stringWithFormat:@"3:%@:%@:%@", 
							   ((LTEntityDescriptor *)entity.entityDescriptor).custName, 
							   ((LTEntityDescriptor *)entity.entityDescriptor).siteName, 
							   ((LTEntityDescriptor *)entity.entityDescriptor).devName];
	}		
	
	/* Add to User Defaults */
	NSMutableArray *devFavorites = [descriptorDict objectForKey:deviceEntityAddress];
	if (!devFavorites)
	{
		devFavorites = [NSMutableArray array];
		[descriptorDict setObject:devFavorites forKey:deviceEntityAddress];
	}
	[devFavorites addObject:[entity.entityDescriptor copy]];
	[self saveFavorites];
	
	/* Add to live list */
	[favorites addObject:entity];
	[self observeEntity:entity];

	/* Reload */
	[self rebuildDisplayFavorites];
	[self.tableView reloadData];
}

- (void) removeFromFavorites:(LTEntity *)entity
{
	/* Get deviceEntityAddress */
	NSString *deviceEntityAddress = nil;
	if (entity.deviceEntityAddress)
	{
		/* Entity is live */
		deviceEntityAddress = entity.deviceEntityAddress;
	}
	else
	{
		/* Entity is orphaned... create an address string */
		deviceEntityAddress = [NSString stringWithFormat:@"3:%@:%@:%@", 
							   ((LTEntityDescriptor *)entity.entityDescriptor).custName, 
							   ((LTEntityDescriptor *)entity.entityDescriptor).siteName, 
							   ((LTEntityDescriptor *)entity.entityDescriptor).devName];
	}		

	/* Remove from User Defaults */
	NSMutableArray *devFavorites = [descriptorDict objectForKey:deviceEntityAddress];
	NSMutableIndexSet *indexSet = [NSMutableIndexSet indexSet];
	for (LTEntityDescriptor *entDesc in devFavorites)
	{
		if ([entDesc.entityAddress isEqualToString:entity.entityAddress])
		{ [indexSet addIndex:[devFavorites indexOfObject:entDesc]]; }
	}
	if ([indexSet count] > 0)
	{ [devFavorites removeObjectsAtIndexes:indexSet]; }
	[self saveFavorites];
	
	/* Remove from Orphan Dict */	
	NSMutableArray *devOrphans = [orphanDict objectForKey:deviceEntityAddress];
	indexSet = [NSMutableIndexSet indexSet];
	for (LTEntityDescriptor *entDesc in devOrphans)
	{
		if ([entDesc.entityAddress isEqualToString:entity.entityAddress])
		{ [indexSet addIndex:[devOrphans indexOfObject:entDesc]]; }
	}
	if ([indexSet count] > 0)
	{ [devOrphans removeObjectsAtIndexes:indexSet]; }
	
	/* Remove from live list */	
	if ([favorites containsObject:entity]) [favorites removeObject:entity];
	[self unobserveEntity:entity];
	
	/* Rebuild */
	[self rebuildDisplayFavorites];
}

- (void) bindFavoritesFromDevice:(LTEntity *)device
{
	/* Called to bind any orphaned favorites to the supplied device */
	NSMutableIndexSet *removeSet = [NSMutableIndexSet indexSet];
	for (LTEntityDescriptor *entDesc in [orphanDict objectForKey:device.entityAddress])
	{
		/* Creeate stand-alone entity */
		LTEntity *entity = [[LTEntity new] autorelease];
		entity.type = entDesc.type;
		entity.name = entDesc.name;
		entity.desc = entDesc.desc;
		entity.username = device.username;
		entity.password = device.password;
		entity.customer = device.customer;
		entity.customerName = device.customerName;
		entity.entityAddress = entDesc.entityAddress;
		entity.resourceAddress = device.resourceAddress;
		entity.ipAddress = device.ipAddress;
		entity.coreDeployment = device.coreDeployment;
		entity.entityDescriptor = entDesc;
		[removeSet addIndex:[[orphanDict objectForKey:device.entityAddress] indexOfObject:entDesc]];
		[favorites addObject:entity];
		[self observeEntity:entity];
		[entity refresh];
	}
	[orphanDict removeObjectForKey:device.entityAddress];
	[self rebuildDisplayFavorites];
	[[self tableView] reloadData];
}

- (void) rebuildDisplayFavorites
{
	NSSortDescriptor *nameSortDesc = [[[NSSortDescriptor alloc] initWithKey:@"device.name" ascending:YES] autorelease];
	NSSortDescriptor *recordSortDesc = [[[NSSortDescriptor alloc] initWithKey:@"recordEnabled" ascending:NO] autorelease];

	/* Add Favorites */
	[displayFavorites removeAllObjects];
	for (LTEntity *entity in favorites)
	{
		if (entity.desc) [displayFavorites addObject:entity];
	}
	if (displaySegment.selectedSegmentIndex == 0)
	{
		[displayFavorites sortUsingDescriptors:[NSArray arrayWithObjects:recordSortDesc, nameSortDesc, nil]];
	}
	else
	{
		[displayFavorites sortUsingDescriptors:[NSArray arrayWithObjects:nameSortDesc, nil]];
	}
	
	/* Add Orphans */
	[displayOrphans removeAllObjects];
	for (NSString *devEntityAddress in [orphanDict allKeys])
	{
		NSMutableArray *array = [orphanDict objectForKey:devEntityAddress];
		for (LTEntityDescriptor *orphanEntDesc in array)
		{
			LTEntity *orphan = [[LTEntity new] autorelease];
			orphan.type = orphanEntDesc.type;
			orphan.name = orphanEntDesc.name;
			orphan.desc = orphanEntDesc.desc;
			orphan.entityDescriptor = orphanEntDesc;
			orphan.entityAddress = orphanEntDesc.entityAddress;

			orphan.opState = -1;
			[displayOrphans addObject:orphan];
		}
	}
	
	[[NSNotificationCenter defaultCenter] postNotificationName:@"DisplayFavoritesRebuilt" object:self];
}

- (void) entityRefreshFinished:(NSNotification *)notification
{
	[self rebuildDisplayFavorites];
	[[self tableView] reloadData];
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

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return YES;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

#pragma mark "Table view methods"

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    LTEntity *displayEntity  = nil;
	if (indexPath.section == 0) displayEntity = [displayFavorites objectAtIndex:indexPath.row];
	else if (indexPath.section == 1) displayEntity = [displayOrphans objectAtIndex:indexPath.row];
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		/* iPad */
		if (displaySegment.selectedSegmentIndex == 0 && displayEntity.recordEnabled == 1)
		{ return 120.0f; }
		else
		{ return 48.0; }
	}
	else
	{
		/* iPhone */
		if (displaySegment.selectedSegmentIndex == 0 && displayEntity.recordEnabled == 1)
		{ return 80.0f; }
		else
		{ return 48.0; }
	}
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
	return 2;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
	if (section == 1 && displayOrphans.count > 0) return @"Unreachable Favorites";
	return nil;
}

// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	if (section == 0) return displayFavorites.count;
	else if (section == 1) return displayOrphans.count;
	return 0;
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
    LTEntity *displayEntity = nil;
	if (indexPath.section == 0)
	{
		displayEntity = [displayFavorites objectAtIndex:indexPath.row];
	}
	else if (indexPath.section == 1)
	{
		displayEntity = [displayOrphans objectAtIndex:indexPath.row];
	}

	UITableViewCell *cell = nil;
	if (displaySegment.selectedSegmentIndex == 0 && displayEntity.recordEnabled == 1)
	{
		NSString *CellIdentifier = @"Graph";
		LTMetricGraphTableViewCell *graphViewCell = (LTMetricGraphTableViewCell *) [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
		if (graphViewCell == nil) 
		{
			graphViewCell = [(LTMetricGraphTableViewCell *) [[LTMetricGraphTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
		}
		graphViewCell.graphView.metrics = [NSArray arrayWithObject:displayEntity];
		
		cell = graphViewCell;
	}
	else
	{
		NSString *CellIdentifier = @"Metric";
		LTEntityTableViewCell *metricCell = (LTEntityTableViewCell *) [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
		if (metricCell == nil) 
		{
			metricCell = [[[LTEntityTableViewCell alloc] initWithReuseIdentifier:CellIdentifier] autorelease];
		}
		metricCell.showFullLocation = YES;
		metricCell.showCurrentValue = YES;
		if (indexPath.section == 0)
		{ metricCell.accessoryType = UITableViewCellAccessoryDisclosureIndicator; }
		else
		{ metricCell.accessoryType = UITableViewCellAccessoryNone; }
		metricCell.entity = displayEntity;
		cell = metricCell;
	}
    
    return cell;
}

- (NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	if (indexPath.section == 0) return indexPath;
	else return nil;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	LTEntity *viableEntity = nil;
	if (indexPath.section == 0) 
	{ viableEntity = [displayFavorites objectAtIndex:indexPath.row]; }
	
	UIViewController *viewController = nil;

	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		/* iPad */
		AppDelegate_Pad *appDelegate = (AppDelegate_Pad *) [[UIApplication sharedApplication] delegate];
		[appDelegate displayEntityInDetailView:viableEntity];
	}
	else
	{
		if (viableEntity.type == 6)
		{
			/* Metric */
			LTMetricTableViewController *metricView = [[LTMetricTableViewController alloc] initWithMetric:viableEntity];
			LTEntityDescriptor *entityDescriptor = [viableEntity entityDescriptor];
			metricView.navigationItem.prompt = [NSString stringWithFormat:@"%@ @ %@ %@ %@", entityDescriptor.devDesc, entityDescriptor.siteDesc, entityDescriptor.cntDesc, entityDescriptor.objDesc];
			viewController = metricView;	
		}
		else
		{
			LTEntityTableViewController *anotherViewController = [[LTEntityTableViewController alloc] initWithEntity:viableEntity];
			viewController = anotherViewController;
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
	return YES;
}

- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath
{
	/* Refresh the entity just-in-time */
	[super tableView:tableView willDisplayCell:cell forRowAtIndexPath:indexPath];
	if (indexPath.section == 0)
	{
		LTEntity *displayEntity = [displayFavorites objectAtIndex:indexPath.row];
		[displayEntity refresh];
	}
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath 
{ 
    if (editingStyle == UITableViewCellEditingStyleDelete) 
	{
        // Delete the row from the data source
		LTEntity *entity = nil;
		if (indexPath.section == 0) entity = [displayFavorites objectAtIndex:indexPath.row];
		else if (indexPath.section == 1) entity = [displayOrphans objectAtIndex:indexPath.row];
		[self removeFromFavorites:entity];
        [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:YES];
    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) 
	{
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }
}

- (void)dealloc {
    [super dealloc];
}


@end

