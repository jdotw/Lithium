    //
//  LTDeviceViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 7/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTDeviceViewController.h"
#import "LTContainerIconViewController.h"
#import "LTGraphTiledLayerDelegate.h"
#import "LTMetricGraphRequest.h"
#import "LTGraphLegendTableViewController.h"
#import "LTModalProgressViewController.h"
#import "LTGraphView.h"
#import "LTObjectIconViewController.h"

@implementation LTDeviceViewController

@synthesize device=_device, selectedContainer, selectedObject, entityToHighlight;

- (id) initWithDevice:(LTEntity *)device
{
	self = [self initWithNibName:@"LTDeviceViewController" bundle:nil];
	if (!self) return nil;
	
	self.device = device;
	
	return self;
}

- (id) initWithEntityToHighlight:(LTEntity *)initEntityToHighlight
{
	self = [self initWithDevice:initEntityToHighlight.device];
	if (!self) return nil;
	
	self.entityToHighlight = initEntityToHighlight;
	
	return self;
}

- (void) rebuildContainerScrollView
{
	for (LTContainerIconViewController *vc in containerIconViewControllers)
	{
		[vc.view removeFromSuperview];
	}
	[containerIconViewControllers removeAllObjects];
	
	CGFloat contentWidth = 0.0;
	CGFloat contentHeight = 0.0;
	for (LTEntity *container in self.device.children)
	{
		LTContainerIconViewController *vc = [[LTContainerIconViewController alloc] initWithContainer:container];
		[containerScrollView addSubview:vc.view];
		CGRect viewFrame = vc.view.frame;
		viewFrame.origin.x = vc.view.frame.size.width * containerIconViewControllers.count;
		viewFrame.origin.y = 0.0;
		contentWidth = viewFrame.origin.x + viewFrame.size.width;
		if (viewFrame.size.height > contentHeight) contentHeight = viewFrame.size.height;
		vc.view.frame = viewFrame;
		vc.delegate = self;
		[containerIconViewControllers addObject:vc];
		[vc release];
	}
	containerScrollView.contentSize = CGSizeMake(contentWidth, contentHeight);
}

- (void) rebuildObjectScrollView
{
	for (LTObjectIconViewController *vc in objectIconViewControllers)
	{
		[vc.view removeFromSuperview];
	}
	[objectIconViewControllers removeAllObjects];
	
	CGFloat contentWidth = 0.0;
	CGFloat contentHeight = 0.0;
	for (LTEntity *object in self.selectedContainer.children)
	{
		LTObjectIconViewController *vc = [[LTObjectIconViewController alloc] initWithObject:object];
		[objectScrollView addSubview:vc.view];
		CGRect viewFrame = vc.view.frame;
		viewFrame.origin.x = vc.view.frame.size.width * objectIconViewControllers.count;
		viewFrame.origin.y = 0.0;
		contentWidth = viewFrame.origin.x + viewFrame.size.width;
		if (viewFrame.size.height > contentHeight) contentHeight = viewFrame.size.height;
		vc.view.frame = viewFrame;
		vc.delegate = self;
		[objectIconViewControllers addObject:vc];
		[vc release];
	}
	objectScrollView.contentSize = CGSizeMake(contentWidth, contentHeight);
}

- (void) resizeAndInvalidateScrollViewContent
{
	CGRect contentRect = CGRectMake(0.0, 0.0, 8000.0, graphScrollView.frame.size.height);
	graphView.frame = contentRect;
	graphScrollView.contentSize = contentRect.size;
}

- (void) viewDidLoad
{
	/* Setup graph view */
	CGRect contentRect = CGRectMake(0.0, 0.0, 8000.0, graphScrollView.frame.size.height);
	graphView = [[LTGraphView alloc] initWithFrame:contentRect];
	[graphView.minLabels addObject:leftMinLabel];
	[graphView.minLabels addObject:rightMinLabel];
	[graphView.avgLabels addObject:leftAvgLabel];
	[graphView.avgLabels addObject:rightAvgLabel];
	[graphView.maxLabels addObject:leftMaxLabel];
	[graphView.maxLabels addObject:rightMaxLabel];
	[graphScrollView addSubview:graphView];
	graphScrollView.contentSize = graphView.frame.size;	
	graphScrollView.maximumZoomScale = 1.0;
	graphScrollView.minimumZoomScale = 10.0;
	graphScrollView.delegate = self;
	[graphScrollView scrollRectToVisible:CGRectMake(CGRectGetMaxX(contentRect) - CGRectGetWidth(graphScrollView.frame),
													0.0, CGRectGetWidth(graphScrollView.frame), CGRectGetHeight(graphScrollView.frame)) animated:NO];
	
	NSLog (@"At load we're at %@", NSStringFromCGRect([graphScrollView frame]));

	/* Setup the container scrollview */
	containerIconViewControllers = [[NSMutableArray array] retain];
	[self rebuildContainerScrollView];
	objectIconViewControllers = [[NSMutableArray array] retain];
	
	self.navigationItem.title = self.device.desc;
}

- (void) viewWillAppear:(BOOL)animated
{
	[super viewWillAppear:animated];
	[self resizeAndInvalidateScrollViewContent];
	NSLog (@"At will appear we're at %@", NSStringFromCGRect([graphScrollView frame]));	
	
}

- (void) viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];
	NSLog (@"At did appear we're at %@", NSStringFromCGRect([graphScrollView frame]));
	if (!viewHasAppearedBefore)
	{
		/* First time the view will appear */
		if (!self.device.hasBeenRefreshed)
		{
			/* Need to show a modal refresh */
			modalRefreshInProgress = YES;
			modalProgressViewController = [[LTModalProgressViewController alloc] initWithNibName:@"LTModalProgressViewController" bundle:nil];
			modalProgressViewController.modalPresentationStyle = UIModalPresentationFormSheet;
			[self.navigationController presentModalViewController:modalProgressViewController animated:YES];
		}
		else if (self.entityToHighlight)
		{
			/* Device is already refreshed and we have something to highlight */
			[self highlightEntity:self.entityToHighlight];
			self.entityToHighlight = nil;
		}			
		viewHasAppearedBefore = YES;
	}
}

- (void) entityRefreshFinished:(NSNotification *)note
{
	if (self.device.children.count != containerIconViewControllers.count)
	{
		[self rebuildContainerScrollView];
	}
	if (modalRefreshInProgress)
	{
		NSLog (@"Attempting to dismiss %@", modalProgressViewController);
		[modalProgressViewController dismissModalViewControllerAnimated:YES];
	}
	if (self.entityToHighlight)
	{
		/* Device is already refreshed and we have something to highlight */
		[self highlightEntity:self.entityToHighlight];
		self.entityToHighlight = nil;
	}			
}

- (void) entityRefreshStatusUpdated:(NSNotification *)note
{
	
}

- (void) setDevice:(LTEntity *)value
{
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"RefreshFinished" object:self.device];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"LTEntityXmlStatusChanged" object:self.device];
	[_device release];
	_device = [value retain];
	
	if (containerScrollView) [self rebuildContainerScrollView];
	self.navigationItem.title = self.device.desc;
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(entityRefreshFinished:)
												 name:@"RefreshFinished" object:self.device];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(entityRefreshStatusUpdated:)
												 name:@"LTEntityXmlStatusChanged" object:self.device];	
	
	[self.device refresh];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Overriden to allow any orientation.
    return YES;
}

- (void) didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
	[self resizeAndInvalidateScrollViewContent];
}

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}


- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (void)dealloc 
{
	[containerIconViewControllers release];	
    [super dealloc];
}

- (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView
{
	return graphView;
}

- (void)scrollViewDidEndZooming:(UIScrollView *)scrollView withView:(UIView *)view atScale:(float)scale
{
	
}

- (void) setSelectedContainer:(LTEntity *)value
{
	[selectedContainer release];
	selectedContainer = [value retain];
	
	/* Rebuild object list */
	[self rebuildObjectScrollView];
	
	/* Set selection on container icon */
	for (LTContainerIconViewController *vc in containerIconViewControllers)
	{
		if (vc.container == selectedContainer) 
		{
			vc.selected = YES;
		}
		else
		{
			vc.selected = NO;
		}
	}
	
	/* Reset graph layer */
	[graphView setMetrics:selectedContainer.graphableMetrics];
	[graphScrollView scrollRectToVisible:CGRectMake(graphScrollView.contentSize.width - CGRectGetWidth(graphScrollView.frame),
													0.0, CGRectGetWidth(graphScrollView.frame), CGRectGetHeight(graphScrollView.frame)) animated:NO];

	/* Reset legend tableview */
	graphLegendTableViewController.entities = selectedContainer.graphableMetrics;
}

- (void) setSelectedObject:(LTEntity *)value
{
	[selectedObject release];
	selectedObject = [value retain];
	
	/* Set selection on container icon */
	for (LTObjectIconViewController *vc in objectIconViewControllers)
	{
		if (vc.object == selectedObject) 
		{
			vc.selected = YES;
		}
		else
		{
			vc.selected = NO;
		}
	}
	
	/* Reset graph layer */
	[graphView setMetrics:selectedObject.graphableMetrics];
	[graphScrollView scrollRectToVisible:CGRectMake(graphScrollView.contentSize.width - CGRectGetWidth(graphScrollView.frame),
													0.0, CGRectGetWidth(graphScrollView.frame), CGRectGetHeight(graphScrollView.frame)) animated:NO];
	
	/* Reset legend tableview */
	graphLegendTableViewController.entities = selectedObject.graphableMetrics;
}

- (void) highlightEntity:(LTEntity *)entity
{
	/* Performs selections and pop-ups as if the 
	 * user had drilled-down to this entity
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
	
	/* Check to see if we're graphable */
	LTEntity *metric = [liveEntity parentOfType:6];
	if ([container.graphableMetrics containsObject:metric])
	{
		/* We're graphable */
		[graphLegendTableViewController highlightEntity:metric];
	}
	else if ([container.graphableMetrics count] > 0)
	{
		/* The specified metric isn't graphable, use the first graphable */
		[graphLegendTableViewController highlightEntity:[container.graphableMetrics objectAtIndex:0]];
	}
}

@end
