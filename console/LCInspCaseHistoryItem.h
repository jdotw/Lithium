//
//  LCInspCaseHistoryItem.h
//  Lithium Console
//
//  Created by James Wilson on 10/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorItem.h"
#import "LCCaseList.h"

@interface LCInspCaseHistoryItem : LCInspectorItem 
{
	LCCaseList *caseList;
}

@property (retain) LCCaseList *caseList;

@end
