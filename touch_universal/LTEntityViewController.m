    //
//  LTEntityViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 9/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTEntityViewController.h"
#import "LTGraphLegendTableViewController.h"
#import "LTEntityTableViewController.h"
#import "LTEntity.h"
#import "LTGraphTiledLayerDelegate.h"

@implementation LTEntityViewController

@synthesize entity;

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/

- (void)viewDidLoad 
{
    [super viewDidLoad];
	entityTableViewController.entity = self.entity;
	entityTableViewController.externalNavigationController = self.navigationController;
	legendTableViewController.entities = [self.entity graphableMetrics];
	
	CGRect contentRect = CGRectMake(0.0, 0.0, 8000.0, graphScrollView.frame.size.height);
	
	graphTiledLayer = [CATiledLayer layer];
    graphTiledLayer.tileSize = CGSizeMake(512.0, 512.0);
    graphTiledLayer.levelsOfDetail = 1;
    graphTiledLayer.levelsOfDetailBias = 1;
	graphTiledLayer.frame = contentRect;
	
	UIView *graphView = [[UIView alloc] initWithFrame:contentRect];
	[graphView.layer addSublayer:graphTiledLayer];
	
	[graphScrollView addSubview:graphView];
	graphScrollView.contentSize = graphView.frame.size;	
	graphScrollView.maximumZoomScale = 4.0;
	graphScrollView.minimumZoomScale = 0.75;
	[graphScrollView scrollRectToVisible:CGRectMake(CGRectGetMaxX(contentRect) - CGRectGetWidth(graphScrollView.frame),
													0.0, CGRectGetWidth(graphScrollView.frame), CGRectGetHeight(graphScrollView.frame)) animated:NO];
	
	LTGraphTiledLayerDelegate *tileDelegate = [[LTGraphTiledLayerDelegate alloc] init];
	tileDelegate.graphScrollView = graphScrollView;
	tileDelegate.metrics = self.entity.graphableMetrics;
	for (LTEntity *met in tileDelegate.metrics)
	{
		NSLog (@"Graphable is %@", met.entityAddress);
	}
	graphTiledLayer.delegate = tileDelegate;

}

- (void) viewWillAppear:(BOOL)animated
{
	CGRect contentRect = CGRectMake(0.0, 0.0, 8000.0, graphScrollView.frame.size.height);
	[graphScrollView scrollRectToVisible:CGRectMake(CGRectGetMaxX(contentRect) - CGRectGetWidth(graphScrollView.frame),
													0.0, CGRectGetWidth(graphScrollView.frame), CGRectGetHeight(graphScrollView.frame)) animated:NO];
	[graphTiledLayer setNeedsDisplayInRect:graphTiledLayer.frame];	
}

- (CGSize) contentSizeForViewInPopover
{
	return CGSizeMake(300.0, 500.0);
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation 
{
    // Overriden to allow any orientation.
    return YES;
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


- (void)dealloc {
    [super dealloc]; 
}


@end
