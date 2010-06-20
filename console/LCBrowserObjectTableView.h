//
//  LCBrowserObjectTableView.h
//  Lithium Console
//
//  Created by James Wilson on 13/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCViewTableView.h"
#import "LCEntityArrayController.h"

@interface LCBrowserObjectTableView : LCViewTableView 
{
	IBOutlet LCEntityArrayController *containersArrayController;
	NSMutableDictionary *rowHeightDict;
}

@property (retain) LCEntityArrayController *containersArrayController;
@property (retain) NSMutableDictionary *rowHeightDict;
@end
