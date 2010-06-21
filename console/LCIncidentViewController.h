//
//  LCIncidentViewController.h
//  Lithium Console
//
//  Created by James Wilson on 20/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCIncident.h"

@interface LCIncidentViewController : NSObject 
{
	LCIncident *incident;
	
	IBOutlet NSView *view;
	IBOutlet NSObjectController *objectController;	
}

+ (LCIncidentViewController *) controllerForIncident:(LCIncident *)initIncident;
- (LCIncidentViewController *) initForIncident:(LCIncident *)initIncident;
- (LCIncidentViewController *) init;
- (void) dealloc;
- (void) removeViewAndContent;

#pragma mark Accessor Methods
- (LCIncident *) incident;
- (void) setIncident:newIncident;
- (NSView *) view;


@property (nonatomic,retain,getter=view) NSView *view;
@property (nonatomic,retain) NSObjectController *objectController;
@end
