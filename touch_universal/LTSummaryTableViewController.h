//
//  LTSummaryTableViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 11/10/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTIconTableViewController.h"

@interface LTSummaryTableViewController : LTIconTableViewController <UISplitViewControllerDelegate>
{
	NSMutableArray *devices;
	NSMutableArray *incidents;
	
	UIPopoverController *sidePopoverController;
	UIBarButtonItem *sidePopoverBarButtonItem;
}



@end
