//
//  LCInspXRaidLUNMiniView.h
//  Lithium Console
//
//  Created by James Wilson on 18/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspXRaidMiniView.h"
#import "LCLun.h"

@interface LCInspXRaidLUNMiniView : LCInspXRaidMiniView 
{
	LCLun *lun;
}

@property (nonatomic,retain) LCLun *lun;

@end
