//
//  LCCaseSmallViewController.h
//  Lithium Console
//
//  Created by James Wilson on 10/01/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCase.h"

@interface LCCaseSmallViewController : NSObject 
{
	LCCase *cas;
	
	IBOutlet NSView *view;
	IBOutlet NSObjectController *objectController;
}

#pragma mark "Initialisation"
+ (LCCaseSmallViewController *) controllerForCase:(LCCase *)initCase;
- (LCCaseSmallViewController *) initForCase:(LCCase *)initCase;
- (void) removeViewAndContent;

#pragma mark "Accessor Methods"
@property (nonatomic, assign) LCCase *cas;
@property (readonly) NSView *view;

@end
