//
//  LCInspXRaidLUNViewController.h
//  Lithium Console
//
//  Created by James Wilson on 18/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorViewController.h"
#import "LCLun.h"
#import "LCInspXRaidLUNMiniView.h"

@interface LCInspXRaidLUNViewController : LCInspectorViewController 
{
	IBOutlet LCInspXRaidLUNMiniView *raidView;
}

+ (LCInspXRaidLUNViewController *) itemWithTarget:(id)initTarget lun:(LCLun *)initLun;\
- (id) initWithTarget:(id)initTarget lun:(LCLun *)lun;


@end
