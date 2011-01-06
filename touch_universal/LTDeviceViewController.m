//
//  LTDeviceViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 7/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTDeviceViewController.h"
#import "LTContainerIconViewController.h"
#import "LTMetricGraphRequest.h"
#import "LTGraphLegendTableViewController.h"
#import "LTModalProgressViewController.h"
#import "LTGraphView.h"
#import "LTObjectIconViewController.h"
#import "LTEntityDescriptor.h"
#import "LTEntityTableViewController.h"
#import "LTIncidentListTableViewController.h"
#import "LTDeviceEditTableViewController.h"
#import "AppDelegate.h"

@interface LTDeviceViewController (Private)

- (void) selectEntity:(LTEntity *)entity;
- (void) rebuildContainerScrollViewRemovingExistingViews:(BOOL)removeExisting;
- (void) rebuildObjectScrollView;
- (void) hideGraphAndLegend;
- (void) showGraphAndLegend;
- (void) graphMetrics:(NSArray *)metrics fromEntity:(LTEntity *)parentEntity;
- (void) resizeAndInvalidateGraphViewContent;
- (void) availabilityTapped:(id)sender;

@end


@implementation LTDeviceViewController

@synthesize device=_device, selectedContainer, selectedObject, entityToHighlight, activePopoverController;

#pragma mark -
#pragma mark Entity Selection

- (void) displayDevice:(LTEntity *)device withInitialSelection:(LTEntity *)initialSelection
{
	/* This is the main external function used to control
	 * what is displayed in the view. The entity may be a 
	 * device or something lower. If it's lower, then that
	 * entity is set as the entity to highlight.
	 *
	 * entityToDisplay may be an orphan entity and we can not
	 * rely on the device having being refreshed yet. So we just
	 * set it as entityToHighlight and wait for the refresh 
	 */
	
	/* Set Device, this will start a refresh operation
	 * after which the rebuilds, etc can be done
	 */
	if (device != self.device)
	{
		self.device = device;
	}
	
	/* Hide any active pop-overs */
	if (self.activePopoverController.popoverVisible) 
	{
		[activePopoverController dismissPopoverAnimated:YES];
	}	
	
	/* Check to see if a lower-than-device entity was specified. 
	 * If so, set self.entityToHighlight which will be selected 
	 * once the first refresh is done
	 */
	if (initialSelection.type > ENT_DEVICE)
	{
		if (self.device.hasBeenRefreshed) [self selectEntity:initialSelection];
		else self.entityToHighlight = initialSelection;
	}
	else 
	{
		/* Just viewing device, check to see if availability is OK and
		 * if not, present the availability pop-over 
		 */
		self.entityToHighlight = nil;
		LTEntity *availContainer = [self.device.childDict objectForKey:@"avail"];
		if (availContainer.opState > 0) [self availabilityTapped:availToolbarItem];
	}
	
	/* If the device hasn't been refreshed yet, pop up a 
	 * modal progress view 
	 */
	
	if (!self.device.hasBeenRefreshed && self.device.refreshInProgress)
	{
		LTModalProgressViewController *modalVC = [[LTModalProgressViewController alloc] initWithEntity:self.device];
		modalVC.modalPresentationStyle = UIModalPresentationFormSheet;
		[self presentModalViewController:modalVC animated:YES];
		[modalVC release];
		modalRefreshInProgress = YES;
	}	
}

- (void) setDevice:(LTEntity *)value
{
	/* Remove old observers */
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"RefreshFinished" object:self.device];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"LTEntityXmlStatusChanged" object:self.device];
	
	/* Set device */
	[_device release];
	_device = [value retain];

	/* Reset Container/Object Selection and rebuild scrollers */
	self.selectedContainer = nil;
	self.selectedObject = nil;
	[self rebuildContainerScrollViewRemovingExistingViews:YES];
	[self rebuildObjectScrollView];

	/* Setup navigation and toolbar items */
	self.navigationItem.title = self.device.desc;
	availToolbarItem.enabled = (self.device) ? YES : NO;
	sysinfoToolbarItem.enabled = (self.device) ? YES : NO;
	incidentsToolbarItem.enabled = (self.device) ? YES : NO;
	settingsToolbarItem.enabled = (self.device) ? YES : NO;
	
	/* Add new observers */
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(entityRefreshFinished:)
												 name:@"RefreshFinished" object:self.device];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(entityRefreshStatusUpdated:)
												 name:@"LTEntityXmlStatusChanged" object:self.device];	
	
	/* Refresh the device */
	[self.device refresh];
	
	/* Install refresh timer */
	if (refreshTimer) 
	{
		[refreshTimer invalidate];
		refreshTimer = nil;
		[graphRefreshTimer invalidate];
		graphRefreshTimer = nil;
	}
	if (self.device)
	{
		NSTimeInterval refreshInterval = 15.0;
		if (self.device.refreshInterval > 30.0) refreshInterval = self.device.refreshInterval * 0.5;
		refreshTimer = [NSTimer scheduledTimerWithTimeInterval:refreshInterval 
														target:self
													  selector:@selector(refreshTimerFired:)
													  userInfo:nil
													   repeats:YES];
		graphRefreshTimer = [NSTimer scheduledTimerWithTimeInterval:refreshInterval + (refreshInterval * 0.5)
															 target:self
														   selector:@selector(graphRefreshTimerFired:)
														   userInfo:nil
															repeats:YES];
		
	}
}

- (void) selectEntity:(LTEntity *)entity
{
	/* Performs selections and pop-ups as if the 
	 * user had drilled-down to this entity. This
	 * does NOT assume that the entity in self.entityToHighlight
	 * is a live entity and so a locate is done first. 
	 * It is safe to assume a refresh has been done on the
	 * device at thos point.
	 */
	
	/* Locate the live entity */
	LTEntity *liveEntity = [self.device locateChildUsingEntityDescriptor:entity.entityDescriptor];
	
	/* Select Container */
	LTEntity *container = [liveEntity parentOfType:4];
	if (container)
	{
		/* Select the container */
		[self setSelectedContainer:container];
		for (LTContainerIconViewController *vc in containerIconViewControllers)
		{
			if (vc.selected)
			{
				[containerScrollView scrollRectToVisible:vc.view.frame animated:NO];
				break;
			}
		}
	}

	/* Select Object */
	LTEntity *object = [liveEntity parentOfType:5];
	if (object)
	{
		/* Select the container */
		[self setSelectedObject:object];
		for (LTObjectIconViewController *vc in objectIconViewControllers)
		{
			if (vc.selected)
			{
				[objectScrollView scrollRectToVisible:vc.view.frame animated:NO];
				break;
			}
		}
	}	
	
	/* Clear active popover */
	if (self.activePopoverController) 
	{
		[self.activePopoverController dismissPopoverAnimated:YES];
		[self popoverControllerDidDismissPopover:self.activePopoverController];		// Manually dismissing doesn't call this delegate function
	}	
	
	/* Display popover for metric in graph legend */
	LTEntity *metric = [liveEntity parentOfType:6];
	UIPopoverController *popover = [graphLegendTableViewController highlightEntity:metric];
	if (popover) 
	{
		self.activePopoverController = popover;
		popover.delegate = self;
	}
}

#pragma mark -
#pragma mark Selection Management (from ScrollView)

- (NSString *) lastSelectionKey
{
	return [NSString stringWithFormat:@"LTDeviceViewControllerLastSelectionFor%@", self.device.entityAddress];
}

- (void) setSelectedContainer:(LTEntity *)value
{
	[selectedContainer release];
	selectedContainer = [value retain];
	
	/* Remove all and Rebuild object list */
	[self rebuildObjectScrollView];
	
	/* Set selection on container icon */
	for (LTContainerIconViewController *vc in containerIconViewControllers)
	{
		if (vc.entity == selectedContainer) 
		{
			vc.selected = YES;
		}
		else
		{
			vc.selected = NO;
		}
	}
	
	/* Reset graph layer */
	[self graphMetrics:selectedContainer.graphableMetrics fromEntity:selectedContainer];
	if (self.selectedContainer) [self showGraphAndLegend];
	else [self hideGraphAndLegend];
	
	/* Save selection */
	if (selectedContainer)
	{
		[[NSUserDefaults standardUserDefaults] setObject:selectedContainer.entityAddress forKey:[self lastSelectionKey]];
		[[NSUserDefaults standardUserDefaults] synchronize];
	}
}

- (void) setSelectedObject:(LTEntity *)value
{
	[selectedObject release];
	selectedObject = [value retain];
	
	/* Set selection on container icon */
	for (LTObjectIconViewController *vc in objectIconViewControllers)
	{
		if (vc.entity == selectedObject) 
		{
			vc.selected = YES;
		}
		else
		{
			vc.selected = NO;
		}
	}
	
	/* Reset graph */
	[self graphMetrics:selectedObject.graphableMetrics fromEntity:selectedObject];
	
	/* Show/Hide Graph */
	if (self.selectedObject) 
	{
		[self showGraphAndLegend];
	}
	else
	{
		[self hideGraphAndLegend];
	}
	
	/* Save selection */
	if (selectedObject)
	{
		[[NSUserDefaults standardUserDefaults] setObject:selectedObject.entityAddress forKey:[self lastSelectionKey]];
		[[NSUserDefaults standardUserDefaults] synchronize];
	}
}

- (void) hideObjectScrollView
{
	if (!objectScrollViewIsHidden)
	{
		CGFloat yDelta = objectEnclosingView.frame.size.height;

		CGRect graphRect = graphEnclosingView.frame;
		graphRect.origin.y -= yDelta;
		graphRect.size.height += yDelta;
		
		CGRect objectRect = objectEnclosingView.frame;
		objectRect.origin.y -= yDelta;
		
		CGRect dropRect = horizontalScrollDropShadowView.frame;
		dropRect.origin.y -= yDelta;

		[UIView animateWithDuration:0.25 
						 animations:^{ graphEnclosingView.frame = graphRect; objectEnclosingView.frame = objectRect; horizontalScrollDropShadowView.frame = dropRect; }
						 completion:^(BOOL finished){ objectEnclosingView.hidden = YES; }];

		objectScrollViewIsHidden = YES;

		[self resizeAndInvalidateGraphViewContent];
	}
}

- (void) showObjectScrollView
{
	if (objectScrollViewIsHidden)
	{
		CGFloat yDelta = objectEnclosingView.frame.size.height;

		CGRect graphRect = graphEnclosingView.frame;
		graphRect.origin.y += objectEnclosingView.frame.size.height;
		graphRect.size.height -= objectEnclosingView.frame.size.height;

		CGRect objectRect = objectEnclosingView.frame;
		objectRect.origin.y += yDelta;

		CGRect dropRect = horizontalScrollDropShadowView.frame;
		dropRect.origin.y += yDelta;

		objectEnclosingView.hidden = NO;
		
		[UIView animateWithDuration:0.25 
						 animations:^{ graphEnclosingView.frame = graphRect; objectEnclosingView.frame = objectRect; horizontalScrollDropShadowView.frame = dropRect; }];
		
		objectScrollViewIsHidden = NO;
		
		[self resizeAndInvalidateGraphViewContent];
	}
}

- (void) hideGraphAndLegend
{
	if (!graphAndLegendIsHidden)
	{
		[UIView animateWithDuration:0.25 animations:^{ graphEnclosingView.alpha = 0.; graphLegendTableView.alpha = 0.; }];
		graphAndLegendIsHidden = YES;
	}
}

- (void) showGraphAndLegend
{
	if (graphAndLegendIsHidden)
	{
		[UIView animateWithDuration:0.25 animations:^{ graphEnclosingView.alpha = 1.; graphLegendTableView.alpha = 1.; }];
		graphAndLegendIsHidden = NO;
	}
}

#pragma mark -
#pragma mark Graph Management

- (void) graphMetrics:(NSArray *)metrics fromEntity:(LTEntity *)parentEntity
{
	/* Reset graph layer */
	[graphView setMetrics:metrics];
	[graphScrollView scrollRectToVisible:CGRectMake(graphScrollView.contentSize.width - CGRectGetWidth(graphScrollView.frame),
													0.0, CGRectGetWidth(graphScrollView.frame), CGRectGetHeight(graphScrollView.frame)) animated:NO];
	
	/* Reset legend tableview */
	if (parentEntity.type == 5)
	{
		/* Object selection, show everything in legend */
		graphLegendTableViewController.entities = parentEntity.children;
	}
	else 
	{
		/* Otherwise, show graphable metrics in legend */
		graphLegendTableViewController.entities = metrics;
	}

	
}

#pragma mark -
#pragma mark Container and Object Horizontal ScrollView

- (void) rebuildContainerScrollViewRemovingExistingViews:(BOOL)removeExisting
{
	/* This method may or may not remove all existing views depending on
	 * the removeExiting flag. This method is called from a few places including
	 * when a device is set (removeExisting:YES) and when a device is refrsshed
	 * (removeExisting:NO).
	 */
	
	if (removeExisting)
	{
		for (LTContainerIconViewController *vc in containerIconViewControllers)
		{
			[vc.view removeFromSuperview];
		}
		[containerIconViewControllers removeAllObjects];
		[containerIconViewControllerDict removeAllObjects];
	}
	
	CGFloat tileWidth = 120.0;
	CGFloat contentHeight = 90.0;
	for (LTEntity *container in self.device.children)
	{
		/* Filter out unwanted containers */
		if ([container.name isEqualToString:@"avail"]) continue;
		if ([container.name isEqualToString:@"snmp_sysinfo"]) continue;
		if ([container.name isEqualToString:@"snmp_sysinfo"]) continue;
		if ([container.name isEqualToString:@"icmp"]) continue;
		if ([container.name isEqualToString:@"ipaddr"]) continue;
		
		/* Check to see if view exists */
		LTContainerIconViewController *vc = [containerIconViewControllerDict objectForKey:container.name];
		if (!vc)
		{
			/* View Doesn't Exist, Create It */
			vc = [[LTContainerIconViewController alloc] initWithContainer:container];
			[containerScrollView addSubview:vc.view];
			CGRect viewFrame = vc.view.frame;
			viewFrame.origin.x = tileWidth * containerIconViewControllers.count;
			viewFrame.origin.y = 0.0;
			viewFrame.size.width = tileWidth;
			viewFrame.size.height = contentHeight;
			vc.view.frame = viewFrame;
			vc.delegate = self;
			[containerIconViewControllers addObject:vc];
			[containerIconViewControllerDict setObject:vc forKey:container.name];
			[vc release];
		}
	}
	containerScrollView.contentSize = CGSizeMake(tileWidth * containerIconViewControllers.count, contentHeight);
	
	if (containerIconViewControllers.count > 0) 
	{
		containerEnclosingView.hidden = NO;
		horizontalScrollDropShadowView.hidden = NO;
	}
	else 
	{
		containerEnclosingView.hidden = YES;
		horizontalScrollDropShadowView.hidden = YES;
	}
}

- (void) rebuildObjectScrollView
{
	/* This function always removed all existing views
	 * and completely rebuilds them. It's only called
	 * if the container selection changes
	 */
    for (LTObjectIconViewController *vc in objectIconViewControllers)
    {
        [vc.view removeFromSuperview];
    }
    [objectIconViewControllers removeAllObjects];
	
    if (self.selectedContainer.children.count > 1)
    {
        CGFloat contentWidth = 0.0;
        CGFloat contentHeight = 48.0;
        for (LTEntity *object in self.selectedContainer.children)
        {
            LTObjectIconViewController *vc = [[LTObjectIconViewController alloc] initWithObject:object];
            [objectScrollView addSubview:vc.view];
            CGRect viewFrame = vc.view.frame;
            viewFrame.origin.x = vc.view.frame.size.width * objectIconViewControllers.count;
            viewFrame.origin.y = 0.0;
            contentWidth = viewFrame.origin.x + viewFrame.size.width;
            vc.view.frame = viewFrame;
            vc.delegate = self;
            [objectIconViewControllers addObject:vc];
            [vc release];
        }
        objectScrollView.contentSize = CGSizeMake(contentWidth, contentHeight);
    }
	
    if (objectIconViewControllers.count > 0)
    {
        [self showObjectScrollView];
    }
    else
    {
        if (self.selectedContainer.children.count == 1)
        {
            /* Single object, select it */
            self.selectedObject = [self.selectedContainer.children objectAtIndex:0];
        }
        [self hideObjectScrollView];
    }
}

- (void) resizeAndInvalidateGraphViewContent
{
	CGRect contentRect = CGRectMake(0.0, 0.0, 8000.0, graphScrollView.frame.size.height);
	graphView.frame = contentRect;
	graphScrollView.contentSize = contentRect.size;
	[graphView setNeedsLayout];
	[graphView setNeedsDisplayInRect:contentRect];
}

#pragma mark -
#pragma mark View Delegates

- (void) viewDidLoad
{
	/* Setup graph view */
	CGRect contentRect = CGRectMake(0.0, 0.0, 8000.0, graphScrollView.frame.size.height);
	NSArray *labels = [NSArray arrayWithObjects:leftMaxLabel, leftAvgLabel, leftMinLabel, rightMaxLabel, rightAvgLabel, rightMinLabel, nil];
	for (UILabel *label in labels)
	{
		label.layer.shadowOffset = CGSizeMake(2.0, 2.0);
		label.layer.shadowRadius = 3.0;
		label.layer.shadowOpacity = 0.8;
	}
	graphView = [[LTGraphView alloc] initWithFrame:contentRect];
	[graphView.minLabels addObject:leftMinLabel];
	[graphView.minLabels addObject:rightMinLabel];
	[graphView.avgLabels addObject:leftAvgLabel];
	[graphView.avgLabels addObject:rightAvgLabel];
	[graphView.maxLabels addObject:leftMaxLabel];
	[graphView.maxLabels addObject:rightMaxLabel];
	[graphView setNeedsDisplay];
	[graphScrollView addSubview:graphView];
	graphScrollView.contentSize = graphView.frame.size;	
	graphScrollView.maximumZoomScale = 1.0;
	graphScrollView.minimumZoomScale = 10.0;
	graphScrollView.delegate = self;
	[graphScrollView scrollRectToVisible:CGRectMake(CGRectGetMaxX(contentRect) - CGRectGetWidth(graphScrollView.frame),
													0.0, CGRectGetWidth(graphScrollView.frame), CGRectGetHeight(graphScrollView.frame)) animated:NO];
	graphLegendTableViewController.graphView = graphView;
	
	/* Setup the container scrollview */
	containerIconViewControllers = [[NSMutableArray array] retain];
	containerIconViewControllerDict = [[NSMutableDictionary dictionary] retain];
	[self rebuildContainerScrollViewRemovingExistingViews:YES];
	objectIconViewControllers = [[NSMutableArray array] retain];
	objectIconViewControllerDict = [[NSMutableDictionary dictionary] retain];
	
	/* Move/Hide interface components */
	containerEnclosingView.hidden = YES;
	graphEnclosingView.alpha = 0.;
	graphLegendTableView.alpha = 0.;
	graphAndLegendIsHidden = YES;
	CGRect graphRect = graphEnclosingView.frame;
	graphRect.origin.y -= objectEnclosingView.frame.size.height;
	graphRect.size.height += objectEnclosingView.frame.size.height;
	graphEnclosingView.frame = graphRect;
	CGRect objectRect = objectEnclosingView.frame;
	objectRect.origin.y -= objectEnclosingView.frame.size.height;
	objectEnclosingView.frame = objectRect;
	objectEnclosingView.hidden = YES;
	objectScrollViewIsHidden = YES;
	CGRect dropShadowRect = horizontalScrollDropShadowView.frame;
	dropShadowRect.origin.y -= objectEnclosingView.frame.size.height;
	horizontalScrollDropShadowView.frame = dropShadowRect;
	horizontalScrollDropShadowView.hidden = YES;
	[self.view bringSubviewToFront:containerEnclosingView];
	[self resizeAndInvalidateGraphViewContent];
	
	/* Create top-right toolbar */
	UIToolbar* tools = [[UIToolbar alloc] initWithFrame:CGRectMake(0, 0, 200.0, 44.01)];
	tools.tintColor = [UIColor colorWithWhite:0.29 alpha:1.0];
	NSMutableArray* buttons = [[NSMutableArray alloc] initWithCapacity:3];

	/* Availability */
	availToolbarItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"handshake_small.png"]
														style:UIBarButtonItemStylePlain
													   target:self
													   action:@selector(availabilityTapped:)];
	availToolbarItem.style = UIBarButtonItemStylePlain;
	availToolbarItem.enabled = NO;
	[buttons addObject:availToolbarItem];
	[availToolbarItem release];
	[buttons addObject:[[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease]];	
	
	/* Info (System Info) Button */
	sysinfoToolbarItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"info_small.png"]
														  style:UIBarButtonItemStylePlain
														 target:self
														 action:@selector(systemInfoTapped:)];
	sysinfoToolbarItem.style = UIBarButtonItemStylePlain;
	sysinfoToolbarItem.enabled = NO;
	[buttons addObject:sysinfoToolbarItem];
	[sysinfoToolbarItem release];
	[buttons addObject:[[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease]];

	/* Incident List */
	incidentsToolbarItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"flag_small.png"]
															style:UIBarButtonItemStylePlain
														   target:self
														   action:@selector(incidentsTapped:)];
	incidentsToolbarItem.style = UIBarButtonItemStylePlain;
	incidentsToolbarItem.enabled = NO;
	[buttons addObject:incidentsToolbarItem];
	[incidentsToolbarItem release];
	[buttons addObject:[[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil] autorelease]];

	/* Device Settings */
	settingsToolbarItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"settings_small.png"]
														   style:UIBarButtonItemStylePlain
														  target:self
														  action:@selector(deviceSettingsTapped:)];
	settingsToolbarItem.style = UIBarButtonItemStylePlain;
	settingsToolbarItem.enabled = NO;
	[buttons addObject:settingsToolbarItem];
	[settingsToolbarItem release];
	
	/* Place buttons in toolbar and add to nav bar */
	[tools setItems:buttons animated:NO];
	[buttons release];
	self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:tools];
	[tools release];
}

- (void) viewWillAppear:(BOOL)animated
{
	[super viewWillAppear:animated];
	[self resizeAndInvalidateGraphViewContent];
}

- (void) viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Overriden to allow any orientation.
    return YES;
}

- (void) didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
	[self resizeAndInvalidateGraphViewContent];
}

- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}



#pragma mark -
#pragma mark Memory Management

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}


- (void)dealloc 
{
	[containerIconViewControllers release];	
	[containerIconViewControllerDict release];
	[refreshTimer invalidate];
	[graphRefreshTimer invalidate];
    [super dealloc];
}

#pragma mark -
#pragma mark Entity Delegate

- (void) entityRefreshFinished:(NSNotification *)note
{
	if (self.device.children.count != containerIconViewControllers.count)
	{
		[self rebuildContainerScrollViewRemovingExistingViews:NO];
	}
	
	if (modalRefreshInProgress)
	{
		/* The modal controller will dismiss itself when it has appeared
		 * This is done to avoid the race condition of the dismiss being
		 * called before the modal view has actually appeared
		 */
		modalRefreshInProgress = NO;
		LTEntity *availContainer = [self.device.childDict objectForKey:@"avail"];

		if (self.entityToHighlight)
		{
			/* Device is refreshed and we have something to highlight */
			[self selectEntity:self.entityToHighlight];
			self.entityToHighlight = nil;
		}
		else if (availContainer.opState > 0)
		{
			/* Device is refreshed, nothing to highlight but availability is bad */
			[self availabilityTapped:availToolbarItem];
		}
		else if ([[NSUserDefaults standardUserDefaults] objectForKey:[self lastSelectionKey]])
		{
			/* Try to use last-selected */
			NSString *entAddr = [[NSUserDefaults standardUserDefaults] objectForKey:[self lastSelectionKey]];
			LTEntityDescriptor *entDesc = [[[LTEntityDescriptor alloc] initWithEntityAddress:entAddr] autorelease];
			LTEntity *lastSelectionEntity = [self.device locateChildUsingEntityDescriptor:entDesc];
			if (lastSelectionEntity) [self selectEntity:lastSelectionEntity];
		}
	}
}

- (void) entityRefreshStatusUpdated:(NSNotification *)note
{
	
}

#pragma mark -
#pragma mark Refresh

- (void) refresh
{
	[self.device refresh];
}

- (void) refreshTimerFired:(NSTimer *)timer
{
	AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
	if (appDelegate.isActive)
	{
		[self refresh];
	}
}

- (void) graphRefreshTimerFired:(NSTimer *)timer
{
	AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
	if (appDelegate.isActive)
	{
		[graphView refreshGraph];
	}
}

#pragma mark -
#pragma mark Scroll View Delegate


- (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView
{
	return graphView;
}

- (void)scrollViewDidEndZooming:(UIScrollView *)scrollView withView:(UIView *)view atScale:(float)scale
{
	
}

#pragma mark -
#pragma mark SplitView Delegate

- (void) splitViewController:(UISplitViewController *)svc willHideViewController:(UIViewController *)aViewController withBarButtonItem:(UIBarButtonItem *)barButtonItem forPopoverController:(UIPopoverController *)pc
{
	barButtonItem.enabled = YES;
	barButtonItem.image = [UIImage imageNamed:@"navlist.png"];
	self.navigationItem.leftBarButtonItem = barButtonItem;
	sidePopoverController = pc;
	sidePopoverBarButtonItem = barButtonItem;
	if (!self.activePopoverController) self.activePopoverController = pc;
}

- (void) splitViewController:(UISplitViewController *)svc willShowViewController:(UIViewController *)aViewController invalidatingBarButtonItem:(UIBarButtonItem *)barButtonItem
{
	self.navigationItem.leftBarButtonItem = nil;
	sidePopoverController = nil;
	sidePopoverBarButtonItem = nil;
	if (self.activePopoverController == sidePopoverController) self.activePopoverController = nil;
}

#pragma mark -
#pragma mark Popover Delegate (Generic)

- (void) popoverControllerDidDismissPopover:(UIPopoverController *)popoverController
{
	if (self.activePopoverController == popoverController) self.activePopoverController = nil;
	availToolbarItem.enabled = YES;
	sysinfoToolbarItem.enabled = YES;
	incidentsToolbarItem.enabled = YES;
	settingsToolbarItem.enabled = YES;
	
	/* Default to having the sidePopoverController as the
	 * active PopOver controller because we get no 
	 * feedback as to when this popover goes active
	 */
	if (sidePopoverController) self.activePopoverController = sidePopoverController;		
}

#pragma mark -
#pragma mark Toolbar Item Actions

- (void) availabilityTapped:(id)sender
{
	if (self.activePopoverController) 
	{
		[self.activePopoverController dismissPopoverAnimated:YES];
		[self popoverControllerDidDismissPopover:self.activePopoverController];		// Manually dismissing doesn't call this delegate function
	}
	
	LTEntity *availContainer = [self.device.childDict objectForKey:@"avail"];
	if (availContainer)
	{
		LTEntityTableViewController *vc = [[LTEntityTableViewController alloc] initWithEntity:availContainer];
		UINavigationController *nc = [[UINavigationController alloc] initWithRootViewController:vc];
		self.activePopoverController = [[UIPopoverController alloc] initWithContentViewController:nc];
		self.activePopoverController.delegate = self;
		[self.activePopoverController presentPopoverFromBarButtonItem:sender permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
		[vc release];
		[nc release];
		
		availToolbarItem.enabled = NO;
	}	
}

- (void) systemInfoTapped:(id)sender
{
	if (self.activePopoverController) 
	{
		[self.activePopoverController dismissPopoverAnimated:YES];
		[self popoverControllerDidDismissPopover:self.activePopoverController];		// Manually dismissing doesn't call this delegate function
	}
	
	LTEntity *container = [self.device.childDict objectForKey:@"snmp_sysinfo"];
	if (container && container.children.count > 0)
	{
		LTEntityTableViewController *vc = [[LTEntityTableViewController alloc] initWithEntity:[container.children objectAtIndex:0]];
		UINavigationController *nc = [[UINavigationController alloc] initWithRootViewController:vc];
		self.activePopoverController = [[UIPopoverController alloc] initWithContentViewController:nc];
		self.activePopoverController.delegate = self;
		[self.activePopoverController presentPopoverFromBarButtonItem:sender permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
		[vc release];
		[nc release];

		sysinfoToolbarItem.enabled = NO;
	}	
}

- (void) incidentsTapped:(id)sender
{
	if (self.activePopoverController)
	{
		[self.activePopoverController dismissPopoverAnimated:YES];
		[self popoverControllerDidDismissPopover:self.activePopoverController];		// Manually dismissing doesn't call this delegate function
	}
	
	if (self.device)
	{
		LTIncidentListTableViewController *vc = [[LTIncidentListTableViewController alloc] initWithStyle:UITableViewStylePlain];
		vc.device = self.device;
		UINavigationController *nc = [[UINavigationController alloc] initWithRootViewController:vc];
		self.activePopoverController = [[UIPopoverController alloc] initWithContentViewController:nc];
		self.activePopoverController.delegate = self;
		[self.activePopoverController presentPopoverFromBarButtonItem:sender permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
		[vc release];
		[nc release];

		incidentsToolbarItem.enabled = NO;
	}	
}

- (void) deviceSettingsTapped:(id)sender
{
	if (self.activePopoverController) 
	{
		[self.activePopoverController dismissPopoverAnimated:YES];
		[self popoverControllerDidDismissPopover:self.activePopoverController];		// Manually dismissing doesn't call this delegate function
	}

	if (self.device)
	{
		LTDeviceEditTableViewController *vc = [[LTDeviceEditTableViewController alloc] initWithDeviceToEdit:self.device];
		UINavigationController *nc = [[UINavigationController alloc] initWithRootViewController:vc];
		nc.modalPresentationStyle = UIModalPresentationFormSheet;
		[self presentModalViewController:nc animated:YES];
		[vc release];
		[nc release];
	}
}

@end
