//
//  LTMetricTableViewController.h
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTEntity.h"
#import "LTMetricGraphView.h"
#import "LTIncidentList.h"
#import "LTMetricLandscapeViewController.h"
#import "LTMetricGraphTableViewCell.h"
#import "LTTableViewController.h"

@interface LTMetricTableViewController : LTTableViewController <UIActionSheetDelegate>
{
	UIButton *moreValuesButton;
	UIButton *moreFaultsButton;
	UIAlertView *alert;
	
	LTMetricGraphTableViewCell *graphViewCell;
	
	LTEntity *metric;
	LTIncident *incident;
	
	LTIncidentList *incidentList;
	
	LTMetricLandscapeViewController *landscapeViewController;
	LTMetricGraphRequest *landscapeGraphRequest;	
}

- (NSString *) intervalString:(NSTimeInterval)timeInterval;

@property (retain) LTEntity *metric;
@property (retain) LTIncident *incident;

@end
