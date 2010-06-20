//
//  LCActivityController.h
//  Lithium Console
//
//  Created by James Wilson on 16/10/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCBackgroundView.h"

@interface LCActivityController : NSWindowController 
{
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet LCBackgroundView *backView;
	NSMutableArray *activitySortDescriptors;
}

#pragma mark "Initialisation"
+ (LCActivityController *) activityController;
- (LCActivityController *) init;
- (void) dealloc;

#pragma mark "Accessor Methods"
- (LCActivityList *) activityList;
- (NSMutableArray *) activitySortDescriptors;

@property (retain) NSObjectController *controllerAlias;
@property (retain) LCBackgroundView *backView;
@property (retain,getter=activitySortDescriptors) NSMutableArray *activitySortDescriptors;
@end
