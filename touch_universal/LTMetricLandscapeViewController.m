//
//  LTMetricLandscapeViewController.m
//  Lithium
//
//  Created by James Wilson on 7/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTMetricLandscapeViewController.h"

#define screenWidth 480.0f
#define screenHeight 320.0f

@implementation LTMetricLandscapeViewController

- (void)layoutScrollImages
{
	// reposition all image subviews in a horizontal serial fashion
	CGFloat curXLoc = 0;
	int numSubViews = 0;
	for (LTMetricGraphView *view in graphViews)
	{
		if ([view isKindOfClass:[LTMetricGraphView class]])
		{
			CGRect frame = view.frame;
			frame.origin = CGPointMake(curXLoc, 0);
			view.frame = frame;
			
			curXLoc += (screenWidth);
			
			numSubViews++;			
		}
	}
	
	// set the content size so it can be scrollable
	[scrollView setContentSize:CGSizeMake((numSubViews * screenWidth), [scrollView bounds].size.height)];
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil 
{
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) 
	{
		graphViews = [[NSMutableArray array] retain];
		
		[NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(hideDatesTimerCallback) userInfo:nil repeats:NO];
	}
    return self;
}

- (void) hideDatesTimerCallback
{
	for (LTMetricGraphView *graphView in graphViews)
	{ graphView.dateLabelsHidden = YES; }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}
- (void)dealloc {
    [super dealloc];
}

- (void)scrollViewDidScroll:(UIScrollView *)sv
{
	/* If the scrollview isn't being dragged and has hit x=0.0
	 * then add another view to the scroll stack with a reference
	 * date of lastDate - 2 Days 
	 */
	
	if ((scrollView.decelerating || !scrollView.dragging) && scrollView.contentOffset.x == 0.0)
	{
		/* Retrieve oldest graph view */
		LTMetricGraphView *oldestView = [graphViews objectAtIndex:0];

		
		/* Setup calendar */
		NSCalendar *calendar = [[[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar] autorelease]; 
		NSDateComponents *deltaComp = [[[NSDateComponents alloc] init] autorelease];
		[deltaComp setDay:-2];
		
		/* Create view */
		LTMetricGraphView *graphView = [[LTMetricGraphView alloc] initWithFrame:CGRectMake(0.0, 0.0, screenWidth, screenHeight)];
		graphView.graphViewStyle = 3;
		graphView.graphRequest.referenceDate = [calendar dateByAddingComponents:deltaComp toDate:oldestView.graphRequest.referenceDate options:0];
		graphView.graphRequest.size = CGSizeMake(480.0, 320.0);
		graphView.metric = metric;
		graphView.tag = oldestView.tag + 1;
		[graphView setNeedsLayout];
		[scrollView addSubview:graphView];
		[graphViews insertObject:graphView atIndex:0];
		[graphView release];
		
		[self layoutScrollImages];
		[scrollView scrollRectToVisible:CGRectMake(480.0, 0.0, 480.0, 320.0) animated:NO];		
	}
	
	if ((scrollView.decelerating || !scrollView.dragging) && ((int)scrollView.contentOffset.x) % 480 == 0)
	{
		visibleGraphView = [graphViews objectAtIndex:((int)scrollView.contentOffset.x) / 480];
		
		for (LTMetricGraphView *graphView in graphViews)
		{
			if (graphView != visibleGraphView)
			{
				graphView.animateScaling = NO;
				graphView.useArtificialScale = YES;
				graphView.scaledMax = visibleGraphView.graphRequest.maxValue;
				graphView.scaledMin = visibleGraphView.graphRequest.minValue;
			}
			else
			{ 
				graphView.animateScaling = YES;
				graphView.useArtificialScale = NO; 
			}
		}
	}
}

- (void)scrollViewWillBeginDragging:(UIScrollView *)scrollView
{
	for (LTMetricGraphView *graphView in graphViews)
	{
		graphView.dateLabelsHidden = NO;
	}
	
}

- (void)scrollViewDidEndDragging:(UIScrollView *)scrollView willDecelerate:(BOOL)decelerate
{
	if (!decelerate)
	{ 
		for (LTMetricGraphView *graphView in graphViews)
		{
			graphView.dateLabelsHidden = YES;
		}
	}
}

- (void)scrollViewDidEndDecelerating:(UIScrollView *)scrollView
{
	for (LTMetricGraphView *graphView in graphViews)
	{
		graphView.dateLabelsHidden = YES;
	}
}

@synthesize metric;
- (void) setMetric:(LTEntity *)value
{
	[metric release];
	metric = [value retain];

	/* Setup date */
	NSDate *referenceDate = [NSDate date];
	NSCalendar *calendar = [[[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar] autorelease];
	NSDateComponents *deltaComp = [[[NSDateComponents alloc] init] autorelease];
	[deltaComp setDay:-2];
	
	/* Create current graph view */
	LTMetricGraphView *graphView = [[LTMetricGraphView alloc] initWithFrame:CGRectMake(0.0, 0.0, screenWidth, screenHeight)];
	graphView.graphViewStyle = 3;
	graphView.graphRequest.referenceDate = referenceDate;
	graphView.graphRequest.size = CGSizeMake(480.0, 320.0);
	graphView.tag = 0;
	if (cachedGraphRequest) graphView.graphRequest = cachedGraphRequest;
	graphView.metric = metric;
	[scrollView addSubview:graphView];
	[graphViews addObject:graphView];
	[graphView autorelease];
	
	/* Create past graph view */
	graphView = [[LTMetricGraphView alloc] initWithFrame:CGRectMake(0.0, 0.0, screenWidth, screenHeight)];
	graphView.graphViewStyle = 3;
	referenceDate = [calendar dateByAddingComponents:deltaComp toDate:referenceDate options:0];	
	graphView.graphRequest.referenceDate = referenceDate;
	graphView.graphRequest.size = CGSizeMake(480.0, 320.0);
	graphView.metric = metric;
	graphView.tag = 1;
	[scrollView addSubview:graphView];
	[graphViews insertObject:graphView atIndex:0];
	[graphView autorelease];	

	/* Adjust layout and scroll to current */
	[self layoutScrollImages];
	[scrollView scrollRectToVisible:CGRectMake(480.0, 0.0, 480.0, 320.0) animated:NO];
}

@synthesize cachedGraphRequest;

@end
