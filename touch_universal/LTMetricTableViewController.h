//
//  LTMetricTableViewController.h
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTTableViewController.h"

@class LTEntity, LTIncidentList, LTIncident, LTMetricLandscapeViewController, LTMetricGraphRequest, LTMetricGraphTableViewCell;

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
	
	IBOutlet UISearchBar *searchBar;
	IBOutlet UISearchDisplayController *searchDisplayController;
}

- (id) initWithMetric:(LTEntity *)initMetric;
- (NSString *) intervalString:(NSTimeInterval)timeInterval;

@property (retain) LTEntity *metric;
@property (retain) LTIncident *incident;

@end
