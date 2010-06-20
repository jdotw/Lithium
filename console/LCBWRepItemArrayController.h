//
//  LCBWRepItemArrayController.h
//  Lithium Console
//
//  Created by James Wilson on 30/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCTableView.h"

@interface LCBWRepItemArrayController : NSArrayController 
{
	IBOutlet LCTableView *tableView;
	IBOutlet NSTableColumn *descColumn;
}

@property (retain) LCTableView *tableView;
@property (retain) NSTableColumn *descColumn;
@end
