//
//  LCCaseLogUpdateController.h
//  Lithium Console
//
//  Created by James Wilson on 9/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCCase.h"

@interface LCCaseLogUpdateController : NSWindowController 
{
	/* Case */
	LCCase *cas;
	BOOL closeFlag;
	
	/* UI Elements */
	IBOutlet NSButton *recordButton;
	IBOutlet NSTextView *textView;
	IBOutlet NSObjectController *controllerAlias;
}

- (id) initWithCase:(LCCase *)initCase closeCase:(BOOL)initCloseFlag;

- (IBAction) recordClicked:(id)sender;
- (IBAction) cancelClicked:(id)sender;

@property (nonatomic,retain) LCCase *cas;
@property (nonatomic, assign) BOOL closeFlag;

@end
