//
//  LCReviewActiveTriggersTableView.h
//  Lithium Console
//
//  Created by James Wilson on 28/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCReviewActiveTriggersWindowController.h"
#import "LCTableView.h"

@interface LCReviewActiveTriggersTableView : LCTableView 
{
	IBOutlet NSTableColumn *checkBoxColumn;
	IBOutlet LCReviewActiveTriggersWindowController *windowController;
}

@end
