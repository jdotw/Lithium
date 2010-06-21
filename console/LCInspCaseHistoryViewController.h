//
//  LCInspCaseHistoryViewController.h
//  Lithium Console
//
//  Created by James Wilson on 10/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorViewController.h"
#import "LCBrowserTableView.h"
#import "LCCaseList.h"

@interface LCInspCaseHistoryViewController : LCInspectorViewController 
{
	IBOutlet LCBrowserTableView *tableView;
	LCCaseList *caseList;
}

+ (LCInspCaseHistoryViewController *) itemWithTarget:(id)initTarget caseList:(LCCaseList *)initCaseList;
- (id) initWithTarget:(id)initTarget caseList:(LCCaseList *)initCaseList;
@property (nonatomic,retain) LCCaseList *caseList;

@end
