//
//  LTIncidentListTableViewController.h
//  Lithium
//
//  Created by James Wilson on 28/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTTableViewController.h"

@interface LTIncidentListTableViewController : LTTableViewController 
{
	UISegmentedControl *sortSegment;
	NSArray *sortedChildren;
}

- (void) coreDeploymentArrayUpdated:(NSNotification *)notification;
- (void) coreDeploymentRefreshFinished:(NSNotification *)notification;
- (void) incidentListRefreshFinished:(NSNotification *)notification;
- (void) rebuildIncidentsArray;

@property (copy) NSArray *sortedChildren;
@property (readonly) BOOL refreshInProgress;

@end
