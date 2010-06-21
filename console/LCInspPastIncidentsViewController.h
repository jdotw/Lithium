//
//  LCInspPastIncidentsViewController.h
//  Lithium Console
//
//  Created by James Wilson on 15/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorViewController.h"
#import "LCBrowserTableView.h"
#import "LCIncidentList.h"

@interface LCInspPastIncidentsViewController : LCInspectorViewController
{
	IBOutlet LCBrowserTableView *tableView;
	LCIncidentList *incidentList;
}

+ (LCInspPastIncidentsViewController *) itemWithTarget:(id)initTarget incidentList:(LCIncidentList *)initIncidentList;
- (id) initWithTarget:(id)initTarget incidentList:(LCIncidentList *)initIncidentList;
@property (nonatomic,retain) LCIncidentList *incidentList;

@end
