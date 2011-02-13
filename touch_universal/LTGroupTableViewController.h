//
//  LTGroupTableViewController.h
//  Lithium
//
//  Created by James Wilson on 18/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTEntity.h"
#import "LTGroup.h"
#import "LTTableViewController.h"

@interface LTGroupTableViewController : LTTableViewController 
{
	LTGroup *group;
	NSMutableArray *children;
	
	NSTimer *refreshTimer;
}

- (void) refresh;
@property (nonatomic,retain) LTGroup *group;

@end
