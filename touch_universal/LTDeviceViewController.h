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

@interface LTDeviceViewController : UIViewController <UIScrollViewDelegate, UIPopoverControllerDelegate, UISplitViewControllerDelegate>
{
	/* Entity */
	LTEntity *_device;

	/* Container Scrollview */
	IBOutlet UIView *containerEnclosingView;
	IBOutlet UIScrollView *containerScrollView;
	NSMutableArray *containerIconViewControllers;
	NSMutableDictionary *containerIconViewControllerDict;	// Keyed on Container name 
	LTEntity *selectedContainer;
	NSArray *selectedContainerGraphableMetrics;

	/* Object Scrollview */
	IBOutlet UIView *objectEnclosingView;
	IBOutlet UIScrollView *objectScrollView;
	NSMutableArray *objectIconViewControllers;
	NSMutableDictionary *objectIconViewControllerDict;		// Keyed on Object name
	LTEntity *selectedObject;
	NSArray *selectedObjectGraphableMetrics;
	BOOL objectScrollViewIsHidden;

	/* Drop shadow (Container/Object Scrollviews) */
	IBOutlet UIView *horizontalScrollDropShadowView;

	/* Graph */
	IBOutlet UIView *graphEnclosingView;
	IBOutlet UIView *graphOuterView;				/* Contains the layers for the min/max/avg */
	IBOutlet UIScrollView *graphScrollView;
	IBOutlet UILabel *leftMaxLabel;
	IBOutlet UILabel *leftAvgLabel;
	IBOutlet UILabel *leftMinLabel;
	IBOutlet UILabel *rightMaxLabel;
	IBOutlet UILabel *rightAvgLabel;
	IBOutlet UILabel *rightMinLabel;
	LTGraphView *graphView;
	BOOL graphAndLegendIsHidden;
    IBOutlet UIView *graphLegendEnclosingView;
	IBOutlet UITableView *graphLegendTableView;
	IBOutlet LTGraphLegendTableViewController *graphLegendTableViewController;

	/* State */
	BOOL performingInitialRefresh;
	BOOL viewHasAppearedBefore;
	BOOL modalRefreshInProgress;
	LTModalProgressViewController *modalProgressViewController;
	
	/* Splitview Controller Delegate */
	UIPopoverController *sidePopoverController;
	UIBarButtonItem *sidePopoverBarButtonItem;
	
	/* Pop Overs -- There must be only one shown at any time !! */
	UIPopoverController *activePopoverController;
	
	/* Top-right toolbar items */
    UIToolbar *topRightToolbar;
	UIBarButtonItem *availToolbarItem;
	UIBarButtonItem *sysinfoToolbarItem;
	UIBarButtonItem *incidentsToolbarItem;
	UIBarButtonItem *settingsToolbarItem;	
	
	/* Refresh Timer */
	NSTimer *refreshTimer;
	NSTimer *graphRefreshTimer;	// Refresh offset by half of refreshTimer
	
	/* Background UI Elements */
	IBOutlet UIImageView *consoleLogoImage;
}

- (void) displayDevice:(LTEntity *)device withInitialSelection:(LTEntity *)initialSelection;
- (void) refresh;

@property (nonatomic,retain) LTEntity *device;
@property (nonatomic,retain) LTEntity *selectedContainer;
@property (nonatomic,retain) LTEntity *selectedObject;
@property (nonatomic,retain) LTEntity *entityToHighlight;
@property (nonatomic,retain) UIPopoverController *activePopoverController;

@end
