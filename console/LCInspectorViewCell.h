//
//  LCInspectorViewCell.h
//  Lithium Console
//
//  Created by James Wilson on 4/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCViewCell.h"

@interface LCInspectorViewCell : LCViewCell 
{
	BOOL headerRow;
	BOOL lastRow;
}

@property (getter=headerRow,setter=setHeaderRow:) BOOL headerRow;
@property (getter=lastRow,setter=setLastRow:) BOOL lastRow;
@end
