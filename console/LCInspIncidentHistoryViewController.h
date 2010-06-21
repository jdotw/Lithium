//
//  LCInspIncidentHistoryViewController.h
//  Lithium Console
//
//  Created by James Wilson on 10/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorViewController.h"
#import "LCBrowserTableView.h"
#import "LCIncidentList.h"

@interface LCInspIncidentHistoryViewController : LCInspectorViewController 
{
	IBOutlet LCBrowserTableView *tableView;
	LCIncidentList *incidentList;	
}

+ (LCInspIncidentHistoryViewController *) itemWithTarget:(id)initTarget incidentList:(LCIncidentList *)initIncidentList;
- (id) initWithTarget:(id)initTarget incidentList:(LCIncidentList *)initIncidentList;
@property (nonatomic,retain) LCIncidentList *incidentList;

@end
