//
//  LCGraphStadiumView.h
//  Lithium Console
//
//  Created by James Wilson on 2/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCGraphStadiumController.h"
#import "LCBrowserDeviceContentController.h"

@interface LCGraphStadiumView : NSView 
{
	IBOutlet LCGraphStadiumController *controller;
	IBOutlet id delegate;
}



@end
