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
@property (nonatomic, assign) id representedObject;
@property (nonatomic, assign) BOOL lastRow;
@property (nonatomic, assign) BOOL firstRow;

@end
