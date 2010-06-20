//
//  LCInspEntityTreeViewController.h
//  Lithium Console
//
//  Created by James Wilson on 2/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorViewController.h"

#import "LCTransparentOutlineView.h"

@interface LCInspEntityTreeViewController : LCInspectorViewController 
{
	IBOutlet LCTransparentOutlineView *outlineView;
}

@end
