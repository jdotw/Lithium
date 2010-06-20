//
//  LCInspActiveIncidentsViewController.h
//  Lithium Console
//
//  Created by James Wilson on 15/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorViewController.h"
#import "LCTableView.h"
#import "LCBrowserTableView.h"

@interface LCInspActiveIncidentsViewController : LCInspectorViewController 
{
	IBOutlet LCBrowserTableView *tableView;
}

@end
