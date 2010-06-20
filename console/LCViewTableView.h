//
//  LCViewTableView.h
//  Lithium Console
//
//  Created by James Wilson on 10/01/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCTableView.h"

@interface LCViewTableView : LCTableView 
{
	NSTableColumn *viewColumn;
	NSMutableDictionary *viewControllerDictionary;
}

@property (retain) NSTableColumn *viewColumn;
@property (retain) NSMutableDictionary *viewControllerDictionary;
@end
