//
//  LCXRDriveIconView.h
//  Lithium Console
//
//  Created by James Wilson on 26/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntityViewController.h"

@interface LCXRDriveIconView : NSView
{
	IBOutlet LCEntityViewController *viewController;
}

@property (retain) LCEntityViewController *viewController;
@end
