//
//  LTMetricValuesTableViewController.h
//  Lithium
//
//  Created by James Wilson on 21/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTEntity.h"
#import "LTMetricHistoryList.h"
#import "LTTableViewController.h"

@interface LTMetricValuesTableViewController : LTTableViewController 
{
	LTEntity *metric;
	LTMetricHistoryList *historyList;
}

@property (assign) LTEntity *metric;

@end
