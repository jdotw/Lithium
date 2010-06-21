//
//  LCErrorLogWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 30/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCBackgroundView.h"
#import "LCErrorLog.h"

@interface LCErrorLogWindowController : NSWindowController 
{
	IBOutlet LCBackgroundView *backView;
	IBOutlet NSObjectController *controllerAlias;
}

+ (LCErrorLogWindowController *) errorLogController;
- (LCErrorLogWindowController *) init;
- (LCErrorLog *) errorLog;
- (IBAction) clearLogClicked:(id)sender;

@property (nonatomic,retain) LCBackgroundView *backView;
@property (nonatomic,retain) NSObjectController *controllerAlias;
@end
