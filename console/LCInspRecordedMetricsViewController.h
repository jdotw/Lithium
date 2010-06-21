//
//  LCInspRecordedMetricsViewController.h
//  Lithium Console
//
//  Created by James Wilson on 5/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorViewController.h"
#import "LCBrowserTableView.h"

@interface LCInspRecordedMetricsViewController : LCInspectorViewController 
{
	NSPredicate *filterPredicate;
	
	IBOutlet LCBrowserTableView *tableView;
}

@property (nonatomic,retain) NSPredicate *filterPredicate;

@end
