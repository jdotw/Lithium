//
//  LCInspPropertiesViewController.h
//  Lithium Console
//
//  Created by James Wilson on 5/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorViewController.h"

@interface LCInspPropertiesViewController : LCInspectorViewController
{
	NSMutableArray *properties;
}

@property (readonly) NSMutableArray *properties;
@end
