//
//  LTDeviceEditModuleListTableViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 21/12/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "LTTableViewController.h"

@class LTCustomer, LTDeviceEditTableViewController;

@interface LTDeviceEditModuleListTableViewController : LTTableViewController 
{
	LTCustomer *customer;
	LTDeviceEditTableViewController *editTVC;
	
	NSArray *customModuleKeys;
}

+ (NSString *) descriptionForModuleName:(NSString *)name;
- (id)initWithCustomer:(LTCustomer *)initCustomer deviceEditTVC:(LTDeviceEditTableViewController *)initEditTVC;

@end
