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

@class LTGraphLegendTableViewController, LTGraphView;

@interface LTDeviceViewController : UIViewController <UIScrollViewDelegate>
{
	IBOutlet UIScrollView *containerScrollView;
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
	
	LTEntity *_device;
	
	NSMutableArray *containerIconViewControllers;
	
	LTEntity *selectedContainer;
	NSArray *selectedContainerGraphableMetrics;
}

- (id) initWithDevice:(LTEntity *)device;

@property (nonatomic,retain) LTEntity *device;
@property (nonatomic,retain) LTEntity *selectedContainer;

@end
