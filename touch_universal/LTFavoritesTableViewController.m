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
#import "LTCustomer.h"

#define kFavoritesData @"LTFavoritesArray"

@interface LTFavoritesTableViewController (Private)
- (void) loadFavorites:(NSNotification *)note;
@end

@implementation LTFavoritesTableViewController

#pragma mark -
#pragma Initialization

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
    [displaySegment release];
    
    /* Observe customer changes */
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(loadFavorites:) 
                                                 name:@"LTCustomerAdded"
                                               object:nil];
	
    /* Load favorites */
    [self loadFavorites:nil];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

- (void)dealloc 
{
    /* Remove customer-add observer */
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"LTCustomerAdded" object:nil];
    
    /* Release ivars */
    [favorites release];
    
    [super dealloc];
}

#pragma mark -
#pragma View Delegates

- (void)viewDidLoad 
{
    [super viewDidLoad];	
}

- (void) viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    /* Install refresh timer */
    refreshTimer = [NSTimer scheduledTimerWithTimeInterval:60.0
													target:self
												  selector:@selector(refreshTimerFired:)
												  userInfo:nil
												   repeats:YES];
}

- (void) viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    
    /* Remove refresh timer */
    [refreshTimer invalidate];
    refreshTimer = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return YES;
}

#pragma mark -
#pragma mark Favorites Management (Load/Add/Remove)

/* The favorites data is serialized as an array of NSDictionary objects, 
 * the dictionary contains a data object of the entity descriptor 
 * and the customer name to find the customer that the fav belongs to.
 *
 * It's possible that there will be more saved favorites than will be 
 * loaded into the favorites list at any time. This is because favorites
 * will only show up if the parent customer is also present.
 *
 */

- (LTEntity *) _createOrphanEntityFromDescritor:(LTEntityDescriptor *)entDesc customer:(LTCustomer *)customer
{
    LTEntity *entity = [LTEntity new];
    entity.type = entDesc.type;
    entity.name = entDesc.name;
    entity.desc = entDesc.desc;
    entity.username = [customer username];
    entity.password = [customer password];
    entity.customer = customer;
    entity.customerName = customer.name;
    entity.ipAddress = customer.ipAddress;
    entity.coreDeployment = customer.coreDeployment;
    entity.entityDescriptor = entDesc;
    entity.entityAddress = entDesc.entityAddress;
    return entity;
}

- (void) loadFavorites:(NSNotification *)note
{
    /* Remove all existing objects */
    if (favorites)
    { [favorites removeAllObjects]; }
    else
    { favorites = [[NSMutableArray array] retain]; }
    
    /* Loop through stores data */
    for (NSDictionary *favDict in [[NSUserDefaults standardUserDefaults] arrayForKey:kFavoritesData])
    {
        /* The dictionary contains the following keys:
         * entDesc = NSData serialization of the entity descriptor
         * entAddress = NSString of entity address
         * custName = NSString of the customer name for the favorite
         */
        
        /* Find customer */
        AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
        LTCustomer *customer = [appDelegate customerNamed:[favDict objectForKey:@"custName"]];
        if (!customer)
        {
            continue;
        }
        
        /* Create entity from entDesc */
        LTEntityDescriptor *entDesc = [NSKeyedUnarchiver unarchiveObjectWithData:[favDict objectForKey:@"entDesc"]];
        LTEntity *favorite = [self _createOrphanEntityFromDescritor:entDesc customer:customer];
        
        /* Add entity to favorites array */
        [favorites addObject:favorite];
    }
    
    /* Sort the favorites by desc */
    NSSortDescriptor *sortDesc = [NSSortDescriptor sortDescriptorWithKey:@"desc" ascending:YES];
    [favorites sortUsingDescriptors:[NSArray arrayWithObject:sortDesc]];
}

- (void) addToFavorites:(LTEntity *)liveEntity
{
    /* Add the favorite to the live list, 
     * and also adds it to the stored list
     */
    
    /* Create new orphan */
    LTEntity *favorite = [self _createOrphanEntityFromDescritor:liveEntity.entityDescriptor customer:liveEntity.customer];
    
    /* Add to favorites list */   
    [favorites addObject:favorite];
    
    /* Create store dict */
    NSMutableDictionary *favDict = [NSMutableDictionary dictionary];
    [favDict setObject:favorite.customer.name forKey:@"custName"];
    [favDict setObject:favorite.entityAddress forKey:@"entAddress"];
    [favDict setObject:[NSKeyedArchiver archivedDataWithRootObject:favorite.entityDescriptor] 
                forKey:@"entDesc"];
    
    /* Add favDict to stored favorites */
    NSMutableArray *storedFavorites;
    if ([[NSUserDefaults standardUserDefaults] arrayForKey:kFavoritesData])
    { storedFavorites = [[[NSUserDefaults standardUserDefaults] arrayForKey:kFavoritesData] mutableCopy]; }
    else
    { storedFavorites = [NSMutableArray array]; }
    [storedFavorites addObject:favDict];
    [[NSUserDefaults standardUserDefaults] setObject:storedFavorites forKey:kFavoritesData];
    [[NSUserDefaults standardUserDefaults] synchronize];
    
    /* Reload */
    [self.tableView reloadData];
}

- (void) _removeFavoriteFromStore:(LTEntity *)entity
{
    /* Remove the favorite from the live list
     * and also remove it from the stored list 
     */
    
    /* Remove from live list */
    for (LTEntity *favorite in [NSArray arrayWithArray:favorites])
    {
        if ([favorite.entityAddress isEqualToString:entity.entityAddress])
        { [favorites removeObject:favorite]; }
    }
    
    /* Remove from stores list */
    NSMutableArray *storedFavorites;
    if ([[NSUserDefaults standardUserDefaults] arrayForKey:kFavoritesData])
    { 
        storedFavorites = [[[NSUserDefaults standardUserDefaults] arrayForKey:kFavoritesData] mutableCopy]; 
        for (NSDictionary *favDict in [NSArray arrayWithArray:storedFavorites])
        { 
            if ([[favDict objectForKey:@"entAddress"] isEqualToString:entity.entityAddress])
            { [storedFavorites removeObject:favDict]; }
        }
        [[NSUserDefaults standardUserDefaults] setObject:storedFavorites forKey:kFavoritesData];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    
}

- (void) removeFromFavorites:(LTEntity *)entity
{
    [self _removeFavoriteFromStore:entity];
    [self.tableView reloadData];
}

#pragma mark -
#pragma UI Actions

- (IBAction) displayTypeChanged:(id)sender
{
	[[NSUserDefaults standardUserDefaults] setInteger:displaySegment.selectedSegmentIndex forKey:@"FavoritesViewMode"];
	[[NSUserDefaults standardUserDefaults] synchronize];
	[self.tableView reloadData];
}

#pragma mark -
#pragma mark Refresh

- (void) refresh
{
	/* This function refreshes the internal list of 
	 * favorites as well as refreshing the entities 
	 */
	
	/* Refresh the entities */
	for (LTEntity *entity in favorites)
	{
		[entity refresh];
	}
}

- (void) refreshTimerFired:(NSTimer *)timer
{
	[self refresh];
}

#pragma mark "Table view methods"

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		/* iPad */
		if (displaySegment.selectedSegmentIndex == 0)
		{ return 120.0f; }
		else
		{ return 48.0; }
	}
	else
	{
		/* iPhone */
		if (displaySegment.selectedSegmentIndex == 0)
		{ return 80.0f; }
		else
		{ return 48.0; }
	}
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
	return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
    return favorites.count;
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
    LTEntity *displayEntity = [favorites objectAtIndex:indexPath.row];

	UITableViewCell *cell = nil;
	if (displaySegment.selectedSegmentIndex == 0)
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
		metricCell.entity = displayEntity;
		metricCell.showFullLocation = YES;
		metricCell.showCurrentValue = YES;
		cell = metricCell;
	}
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	LTEntity *viableEntity = [favorites objectAtIndex:indexPath.row];
	
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

    LTEntity *displayEntity = [favorites objectAtIndex:indexPath.row];
	[displayEntity refresh];
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath 
{ 
    if (editingStyle == UITableViewCellEditingStyleDelete) 
	{
        // Delete the row from the data source
		LTEntity *entity = [favorites objectAtIndex:indexPath.row];
		[self _removeFavoriteFromStore:entity];
        [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:YES];
    }   
}


@end

