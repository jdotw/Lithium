//
//  LTCoreEditTableViewController.h
//  Lithium
//
//  Created by James Wilson on 30/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTCoreDeployment.h"

@interface LTCoreEditTableViewController : UITableViewController 
{
	UITextField *addressTextField;
	UITextField *descTextField;
	UISwitch *useSSLSwitch;
	
	LTCoreDeployment *editCore;
}

- (UITextField *)createTextField;
- (id) initWithCoreToEdit:(LTCoreDeployment *)initCore;

@property (retain) LTCoreDeployment *editCore;

@end
