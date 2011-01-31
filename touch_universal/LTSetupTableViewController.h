//
//  LTSetupTableViewController.h
//  Lithium
//
//  Created by James Wilson on 29/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface LTSetupTableViewController : UITableViewController 
{
	UIButton *addCoreButton;
	UIButton *groupByLocationSwitch;
}

- (UIButton *) createButtonControl:(UIButtonType)type;
- (UISwitch *) createSwitchControl;
- (IBAction) addNewCoreTouched:(id)sender;



@end
