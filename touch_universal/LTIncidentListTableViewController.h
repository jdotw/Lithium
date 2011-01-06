//
//  LTIncidentListTableViewController.h
//  Lithium
//
//  Created by James Wilson on 28/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTTableViewController.h"

@class LTEntity;

@interface LTIncidentListTableViewController : LTTableViewController 
{
	UISegmentedControl *sortSegment;
	NSArray *sortedChildren;
	
	LTEntity *device;
	
	NSTimer *refreshTimer;
}

- (void) refresh;

@property (copy) NSArray *sortedChildren;
@property (readonly) BOOL refreshInProgress;
@property (nonatomic,retain) LTEntity *device;

@end
