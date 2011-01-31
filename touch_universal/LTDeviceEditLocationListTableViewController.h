//
//  LTDeviceEditLocationListTableViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 31/01/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

@class LTCustomer, LTDeviceEditTableViewController;

@interface LTDeviceEditLocationListTableViewController : UITableViewController 
{
	LTDeviceEditTableViewController *editTVC;	
}

- (id)initWithCustomer:(LTCustomer *)initCustomer editTableViewController:(LTDeviceEditTableViewController *)initEditTVC;

@property (nonatomic, retain) LTCustomer *customer;

@end
