//
//  LCInspServiceConfigViewController.h
//  Lithium Console
//
//  Created by James Wilson on 8/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorViewController.h"
#import "LCTransparentOutlineView.h"
#import "LCService.h"

@interface LCInspServiceConfigViewController : LCInspectorViewController 
{
	/* Service */
	LCService *service;
}

- (float) rowHeight;

@property (nonatomic,retain) LCService *service;
@end
