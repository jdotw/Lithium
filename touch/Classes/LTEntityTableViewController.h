//
//  LTEntityTableViewController.h
//  Lithium
//
//  Created by James Wilson on 26/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTEntity.h"
#import "LTTableViewController.h"

@interface LTEntityTableViewController : LTTableViewController <UIActionSheetDelegate>
{
	LTEntity *entity;
	NSMutableArray *children;
	
	NSTimer *refreshTimer;
}

@property (retain) LTEntity *entity;

@end
