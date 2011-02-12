//
//  LTMetricTableViewController.m
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTMetricTableViewController.h"
#import <QuartzCore/QuartzCore.h>


#import "LTMetricGraphTableViewCell.h"
#import "LTIncident.h"
#import "LTMetricLandscapeViewController.h"
#import "LTMetricValuesTableViewController.h"
#import "LTActionTableViewController.h"
#import "AppDelegate.h"
#import "LTTableViewCellBackground.h"
#import "LTTableViewCellSelectedBackground.h"
#import "LTFavoritesTableViewController.h"
#import "LTMetricGraphRequest.h"
#import "LTIncidentList.h"
#import "LTMetricLandscapeViewController.h"
#import "LTTableViewController.h"
#import "LTTableViewCell.h"

#define kAnimationKey @"transitionViewAnimation"

@implementation LTMetricTableViewController

#pragma mark -
#pragma mark Initialization

- (id) initWithMetric:(LTEntity *)initMetric
{
	self = [super initWithNibName:@"LTEntityTableViewController" bundle:nil];
	if (!self) return nil;
		
	incidentList = [[LTIncidentList alloc] init];
	incidentList.historicList = YES;
	incidentList.maxResultsCount = 20;
	
	self.metric = initMetric;	// Also sets up incidentList

	return self;
}

- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning]; 
}

- (void)dealloc 
{
    /* Remove notifications */
    [[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(incidentListRefreshFinished:)
												 name:@"LTIncidentListRefreshFinished" 
											   object:incidentList];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(actionUpdated:)
												 name:@"ActionUpdated" 
											   object:nil];

    /* nil the metric to remove observers and release */
    self.metric = nil;
    
    /* Release other ivars */
    [incidentList release];
    
    [super dealloc];
}

#pragma mark -
#pragma mark Metric

@synthesize metric;
- (void) setMetric:(LTEntity *)value
{
    if (metric)
    {
        [[NSNotificationCenter defaultCenter] removeObserver:self name:@"RefreshFinished" object:metric];
    }
	
    [metric release];
	metric = [value retain];
	
	self.title = metric.desc;
	incidentList.entity = metric;
	
    [metric refresh];
	
    if (metric)
    {
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(metricRefreshFinished:)
                                                     name:@"RefreshFinished" 
                                                   object:metric];
    }
}

#pragma mark -
#pragma mark View Delegates

- (CGSize) contentSizeForViewInPopover
{
	return CGSizeMake(300.0, 500.0);
}

- (void)viewDidLoad 
{
    [super viewDidLoad];

	self.navigationItem.rightBarButtonItem = [[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAction
																							target:self action:@selector(actionClicked:)] autorelease];
	
	self.tableView.tableHeaderView = nil;		// Hides search bar
    
    self.pullToRefresh = YES;
}

- (void)viewWillAppear:(BOOL)animated 
{
    [super viewWillAppear:animated];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(orientationDidChange:)
												 name:UIDeviceOrientationDidChangeNotification
											   object:nil];
}

- (void)viewDidAppear:(BOOL)animated 
{
    [super viewDidAppear:animated];
	if (!metric.hasBeenRefreshed)
	{
		[metric refresh];
	}
	[incidentList refresh];
    
    refreshTimer = [NSTimer scheduledTimerWithTimeInterval:(metric.device.refreshInProgress * 0.5)
                                                    target:self
                                                  selector:@selector(refreshTimerFired:)
                                                  userInfo:nil
                                                   repeats:YES];
}

- (void)viewWillDisappear:(BOOL)animated 
{
	[super viewWillDisappear:animated];
	[[NSNotificationCenter defaultCenter] removeObserver:self 
													name:UIDeviceOrientationDidChangeNotification 
												  object:nil];
}

- (void) viewDidDisappear:(BOOL)animated
{
    [refreshTimer invalidate];
    refreshTimer = nil;
}

- (NSDate *) egoRefreshTableHeaderDataSourceLastUpdated:(EGORefreshTableHeaderView*)view
{
    return metric.lastRefresh;
}

#pragma mark -
#pragma mark Table view methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
	int numSections = 3;
	
	if (incident)
	{ numSections++; }
	
	return numSections;
}

#define SECTION_GRAPH 1
#define SECTION_RECENT 2
#define SECTION_FAULTS 3
#define SECTION_ACTIONS 4

- (int) sectionTypeForSection:(int)section
{
	if (section == 0)
	{
		/* Graph */
		return SECTION_GRAPH; 
	}
	else if (section == 1)
	{
		/* Recent History */
		return SECTION_RECENT;
	}
	else if (section == 2)
	{
		if (incident)
		{ return SECTION_ACTIONS; }
		else
		{ return SECTION_FAULTS; }
	}
	else if (section == 3)
	{ return SECTION_FAULTS; }
	
	return 0;
}

// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
	if ([self sectionTypeForSection:section] == SECTION_GRAPH)
	{
		/* Graph */
		return 1; 
	}
	else if ([self sectionTypeForSection:section] == SECTION_RECENT)
	{
		/* Recent History */
		if (metric.values.count > 0)
		{ return 1; }
		else
		{ return 1; }
	}
	else if ([self sectionTypeForSection:section] == SECTION_ACTIONS)
	{
		/* Fault History */
		return incident.actions.count;
	}
	else if ([self sectionTypeForSection:section] == SECTION_FAULTS)
	{
		/* Fault History */
		return incidentList.incidents.count;
	}
    return 0;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
	switch ([self sectionTypeForSection:section])
	{
		case SECTION_GRAPH:
			return nil;
		case SECTION_RECENT:
			return @"Recent Values";
		case SECTION_ACTIONS:
			return @"Actions";
		case SECTION_FAULTS:
			return @"Fault History (Last 20 Incidents)";
		default: 
			return @"";
	}
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
	switch ([self sectionTypeForSection:indexPath.section]) {
		case SECTION_GRAPH:
			return 161.;
		case SECTION_RECENT:
			return 62.;
		default:
			return 48.;
	}
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
	/* Find/Create Cell */
    static NSString *CellIdentifier = nil;
	switch ([self sectionTypeForSection:indexPath.section])
	{
		case SECTION_GRAPH:
			CellIdentifier = @"Graph";
			break;
		case SECTION_RECENT:
			CellIdentifier = @"Subtitle";
			break;
		case SECTION_ACTIONS:
			CellIdentifier = @"Subtitle";
			break;
		case SECTION_FAULTS:
			if (indexPath.row < incidentList.incidents.count)
			{ CellIdentifier = @"Subtitle"; }
			else
			{ CellIdentifier = @"Cell"; }
			break;
		default:
			CellIdentifier = @"Cell";
	}
	
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
	{
		if ([CellIdentifier isEqualToString:@"Graph"])
		{
			graphViewCell = [[LTMetricGraphTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
			graphViewCell.graphView.metrics = [NSArray arrayWithObject:metric];
			graphViewCell.selectionStyle = UITableViewCellSelectionStyleNone;
			cell = graphViewCell;
		}
		else if ([CellIdentifier isEqualToString:@"Subtitle"])
		{
			cell = [[LTTableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:CellIdentifier];
			[cell autorelease];
		}
		else
		{ cell = [[[LTTableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease]; }
    }

    /* Setup the Cell */
	if ([self sectionTypeForSection:indexPath.section] == SECTION_GRAPH)
	{
		/* Graph */
		LTMetricGraphTableViewCell *graphCell = (LTMetricGraphTableViewCell *) cell;
		graphCell.accessoryType = UITableViewCellAccessoryNone;
	}
	else if ([self sectionTypeForSection:indexPath.section] == SECTION_RECENT)
	{
		/* Recent Values */
		if (metric.values.count > 0 && indexPath.row == 0)
		{
			NSSortDescriptor *sortDesc = [[[NSSortDescriptor alloc] initWithKey:@"floatValue" ascending:YES] autorelease];
			NSArray *sortedValues = [metric.values sortedArrayUsingDescriptors:[NSArray arrayWithObject:sortDesc]];
			NSString *string = [NSString stringWithFormat:@"%@ - %@", [[sortedValues objectAtIndex:0] stringValue], [[sortedValues lastObject] stringValue]];
			NSTimeInterval elapsedTime = [[(LTMetricValue *)[metric.values lastObject] timestamp] timeIntervalSinceDate:[(LTMetricValue *)[metric.values objectAtIndex:0] timestamp]];
			cell.textLabel.text = string;
			cell.detailTextLabel.text = [NSString stringWithFormat:@"Over the last %@", [self intervalString:elapsedTime]];
			cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
		}
	}
	else if ([self sectionTypeForSection:indexPath.section] == SECTION_ACTIONS)
	{
		/* Action */
		LTAction *action = [incident.actions objectAtIndex:indexPath.row];
		cell.textLabel.text = action.desc;
		if (action.runState == 0)
		{ cell.detailTextLabel.text = @"Action is dormant"; }
		else
		{ cell.detailTextLabel.text = [NSString stringWithFormat:@"Action is scheduled to run or re-run"]; }
		cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
		if (action.runState == 0)
		{ cell.imageView.image = [UIImage imageNamed:@"tools_grey_16.tif"]; }
		else
		{ cell.imageView.image = [UIImage imageNamed:@"tools_16.tif"]; }
	}
	else if ([self sectionTypeForSection:indexPath.section] == SECTION_FAULTS)
	{
		/* Fault History */
		LTIncident *pastIncident = [incidentList.incidents objectAtIndex:[indexPath row]];
		NSDateFormatter *dateFormatter = [[[NSDateFormatter alloc] init]  autorelease];
		[dateFormatter setDateStyle:NSDateFormatterMediumStyle];
		[dateFormatter setTimeStyle:NSDateFormatterMediumStyle];
		if (pastIncident.endDate)
		{
			NSTimeInterval timeInterval = [pastIncident.endDate timeIntervalSinceDate:pastIncident.startDate];
			cell.textLabel.text = [NSString stringWithFormat:@"%@", [dateFormatter stringFromDate:pastIncident.startDate]];
			cell.detailTextLabel.text = [NSString stringWithFormat:@"Metric reached or exceeded %@ for %@", pastIncident.raisedValue, [self intervalString:timeInterval]];
		}
		else
		{
			cell.textLabel.text = [NSString stringWithFormat:@"%@", [dateFormatter stringFromDate:pastIncident.startDate]];				
			cell.detailTextLabel.text = [NSString stringWithFormat:@"Metric reached or exceeded %@", pastIncident.raisedValue];
		}
		cell.accessoryType = UITableViewCellAccessoryNone;
		switch (pastIncident.entityDescriptor.opState)
		{
			case -2:
				cell.imageView.image = [[UIImage alloc] initWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"BlueDot" ofType:@"tiff"]];
				break;				
			case 0:
				cell.imageView.image = [UIImage imageNamed:@"GreenDot.tiff"];
				break;
			case 1:
				cell.imageView.image = [UIImage imageNamed:@"YellowDot.tiff"];
				break;
			case 2:
				cell.imageView.image = [UIImage imageNamed:@"YellowDot.tiff"];
				break;
			case 3:
				cell.imageView.image = [UIImage imageNamed:@"RedDot.tiff"];
				break;
			default:
				cell.imageView.image = [UIImage imageNamed:@"GreyDot.tiff"];
		}
	}

    return cell;
}

- (NSString *) intervalString:(NSTimeInterval)timeInterval
{
	time_t interval = (time_t) timeInterval;
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

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	if ([self sectionTypeForSection:indexPath.section] == SECTION_RECENT)
	{
		LTMetricValuesTableViewController *viewController = [[LTMetricValuesTableViewController alloc] initWithNibName:@"MetricValues" bundle:nil];
		viewController.metric = self.metric;
		[self.navigationController pushViewController:viewController animated:YES];
		[viewController release];
	}
	else if ([self sectionTypeForSection:indexPath.section] == SECTION_ACTIONS)
	{
		LTActionTableViewController *viewController = [[LTActionTableViewController alloc] initWithStyle:UITableViewStyleGrouped];
		viewController.action = [incident.actions objectAtIndex:indexPath.row];
		if (self.navigationItem.prompt) viewController.navigationItem.prompt = self.navigationItem.prompt;
		[self.navigationController pushViewController:viewController animated:YES];
		[viewController release];		
	}
}

- (NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	int sectionType = [self sectionTypeForSection:indexPath.section];
	if (sectionType == SECTION_GRAPH) return nil;
	else if (sectionType == SECTION_FAULTS) return nil;
	else return indexPath;
}

#pragma mark -
#pragma mark Notification Receivers

- (void) incidentListRefreshFinished:(NSNotification *)notification
{
	[[self tableView] reloadData];
}

- (void) metricRefreshFinished:(NSNotification *)notification
{
    if (_reloading && ![self refreshInProgress])
    {
        /* Cancel pull to refresh view */
        _reloading = NO;
        [_refreshHeaderView egoRefreshScrollViewDataSourceDidFinishedLoading:self.tableView];
    }        
    
    [[self tableView] reloadData];
}

- (void) orientationDidChange:(NSNotification *)notification
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
	{
		UIView *window = [(AppDelegate *)[[UIApplication sharedApplication] delegate] window];
		UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
		
		if (orientation == UIDeviceOrientationLandscapeLeft || orientation == UIDeviceOrientationLandscapeRight) 
		{
			[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:YES];
			
			if(!landscapeViewController)
			{
				landscapeViewController = [[LTMetricLandscapeViewController alloc] initWithMetric:self.metric];
			}
			
			[window addSubview:landscapeViewController.view];
			if (orientation == UIDeviceOrientationLandscapeLeft)
			{
				landscapeViewController.view.frame = CGRectMake(-80.0, 80.0, 480.0, 320.0);
				landscapeViewController.view.transform = CGAffineTransformMakeRotation(90.0 * M_PI / 180.0);
			}
			else
			{
				landscapeViewController.view.frame = CGRectMake(-80.0, 80.0, 480.0, 320.0);
				landscapeViewController.view.transform = CGAffineTransformMakeRotation(-90.0 * M_PI / 180.0);
				
			}		
		} 
		else
		{
			[[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:YES];
			[landscapeViewController.view removeFromSuperview];
			[landscapeViewController release];
			landscapeViewController = nil;
		}
		
		CATransition *animation = [CATransition animation];
		[animation setDelegate:self];
		[animation setType:kCATransitionFade];
		[animation setDuration:0.5];
		[animation setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
		[[window layer] addAnimation:animation forKey:kAnimationKey];
	}
}

- (void) actionUpdated:(NSNotification *)notification
{
	[[self tableView] reloadData];
}

#pragma mark -
#pragma mark Refresh Methods

- (BOOL) refreshInProgress
{
    return metric.refreshInProgress;
}

- (void) refresh
{
    [metric refresh];
    _reloading = [self refreshInProgress];
}

- (void) forceRefresh
{
    [metric forceRefresh];
    _reloading = [self refreshInProgress];
}

- (void) refreshTimerFired:(NSTimer *)timer
{
    [self refresh];
}

#pragma mark -
#pragma mark UI Actions

- (IBAction) actionClicked:(id)sender
{
	UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:[NSString stringWithFormat:@"%@ Actions", metric.desc]
															 delegate:self 
													cancelButtonTitle:@"Cancel" 
											   destructiveButtonTitle:nil
													otherButtonTitles:@"Add To Favorites", nil];
	actionSheet.actionSheetStyle = UIActionSheetStyleDefault;
	if ([sender isMemberOfClass:[UIBarButtonItem class]])
	{ [actionSheet showFromBarButtonItem:sender animated:YES]; }
	else if (self.tabBarController)
	{ [actionSheet showFromTabBar:(UITabBar *)self.tabBarController.view]; }
	[actionSheet release];
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
	if (buttonIndex == 0)
	{
		/* Add to Favorites */
		AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
		[appDelegate.favoritesController addToFavorites:self.metric];
	}
}

#pragma mark -
#pragma mark Properties

@synthesize incident;

@end

