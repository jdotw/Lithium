//
//  LCDeviceTreeCell.h
//  Lithium Console
//
//  Created by James Wilson on 4/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCShadowTextFieldCell.h"
#import "LCEntity.h"
#import "LCIncident.h"
#import "LCDeviceTreeItem.h"

@interface LCDeviceTreeCell : LCShadowTextFieldCell 
{
	id representedObject;
	BOOL lastRow;
	BOOL firstRow;
}

#pragma mark "Properties"
@property (assign) id representedObject;
@property (assign) BOOL lastRow;
@property (assign) BOOL firstRow;

@end
