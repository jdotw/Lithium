//
//  LTDeviceViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 7/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import "LTEntity.h"

@class LTGraphLegendTableViewController, LTGraphView, LTModalProgressViewController;

@interface LTDeviceViewController : UIViewController <UIScrollViewDelegate>
{
	LTEntity *_device;

	IBOutlet UIScrollView *containerScrollView;
	NSMutableArray *containerIconViewControllers;
	LTEntity *selectedContainer;
	NSArray *selectedContainerGraphableMetrics;

	IBOutlet UIScrollView *objectScrollView;
	NSMutableArray *objectIconViewControllers;
	LTEntity *selectedObject;
	NSArray *selectedObjectGraphableMetrics;
	
	IBOutlet UIView *graphOuterView;				/* Contains the layers for the min/max/avg */
	IBOutlet UIScrollView *graphScrollView;
	IBOutlet UILabel *leftMaxLabel;
	IBOutlet UILabel *leftAvgLabel;
	IBOutlet UILabel *leftMinLabel;
	IBOutlet UILabel *rightMaxLabel;
	IBOutlet UILabel *rightAvgLabel;
	IBOutlet UILabel *rightMinLabel;
	LTGraphView *graphView;
	IBOutlet LTGraphLegendTableViewController *graphLegendTableViewController;
	
	BOOL performingInitialRefresh;
	BOOL viewHasAppearedBefore;
	BOOL modalRefreshInProgress;
	LTModalProgressViewController *modalProgressViewController;
}

- (id) initWithDevice:(LTEntity *)device;
- (id) initWithEntityToHighlight:(LTEntity *)initEntityToHighlight;
- (void) highlightEntity:(LTEntity *)entity;

@property (nonatomic,retain) LTEntity *device;
@property (nonatomic,retain) LTEntity *selectedContainer;
@property (nonatomic,retain) LTEntity *selectedObject;
@property (nonatomic,retain) LTEntity *entityToHighlight;

@end
