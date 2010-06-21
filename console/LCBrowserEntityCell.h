//
//  LCBrowserEntityCell.h
//  Lithium Console
//
//  Created by James Wilson on 16/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"

@interface LCBrowserEntityCell : NSTextFieldCell 
{
	LCEntity  *entity;
}

@property (nonatomic, assign) LCEntity  *entity;
@end
