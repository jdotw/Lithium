//
//  LTActionTableViewController.h
//  Lithium
//
//  Created by James Wilson on 4/03/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTAction.h"

@interface LTActionTableViewController : UITableViewController 
{
	LTAction *action;
	UIAlertView *executeAlertView;
	UIAlertView *cancelAlertView;
	
}

@property (retain) LTAction *action;

@end
