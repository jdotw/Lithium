//
//  MBEnumeratorWindowController.h
//  ModuleBuilder
//
//  Created by James Wilson on 13/05/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class MBMetric;

@interface MBEnumeratorWindowController : NSWindowController 
{
	MBMetric *metric;
	IBOutlet NSArrayController *enumeratorArrayController;
	IBOutlet NSObjectController *objectController;
	
}

@property (nonatomic, retain) MBMetric *metric;
- (IBAction) addClicked:(id)sender;
- (IBAction) removeClicked:(id)sender;

@end
