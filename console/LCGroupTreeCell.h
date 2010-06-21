//
//  LCGroupTreeCell.h
//  Lithium Console
//
//  Created by James Wilson on 30/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCObjectTreeItem.h"
#import "LCObjectTreeMetricItem.h"
#import "LCObjectTreeHeaderItem.h"
#import "LCShadowTextFieldCell.h"

@interface LCGroupTreeCell : LCShadowTextFieldCell 
{
	id representedObject;
	BOOL firstRow;
	BOOL expanded;
	BOOL lastRow;	
}

#pragma mark "Observed Object"
@property (nonatomic, assign) id representedObject;
@property (nonatomic, assign) BOOL firstRow;
@property (nonatomic, assign) BOOL lastRow;
@property (nonatomic, assign) BOOL expanded;


@end
