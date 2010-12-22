//
//  LTDeviceEditRefreshIntervalListTableViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 22/12/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

@class LTDeviceEditTableViewController;

@interface LTDeviceEditRefreshIntervalListTableViewController : UITableViewController 
{
	LTDeviceEditTableViewController *editTVC;
}

- (id)initWithEditTableViewController:(LTDeviceEditTableViewController *)initEditTVC;

@end
