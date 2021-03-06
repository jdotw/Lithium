//
//  LTDeviceEditLocationListTableViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 31/01/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTTableViewController.h"

@class LTCustomer, LTDeviceEditTableViewController;

@interface LTDeviceEditLocationListTableViewController : LTTableViewController 
{
	LTDeviceEditTableViewController *editTVC;	
}

- (id)initWithCustomer:(LTCustomer *)initCustomer editTableViewController:(LTDeviceEditTableViewController *)initEditTVC;

@property (nonatomic, retain) LTCustomer *customer;

@end
