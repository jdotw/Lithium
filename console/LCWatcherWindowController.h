//
//  LCWatcherWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 12/01/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCWatcherWindowController : NSWindowController 
{
	IBOutlet NSObjectController *controllerAlias;
}

@property (nonatomic,retain) NSObjectController *controllerAlias;
@end
