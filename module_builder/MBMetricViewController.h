//
//  MBMetricViewController.h
//  ModuleBuilder
//
//  Created by James Wilson on 7/09/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MBMetric.h"
#import "MBOutlineViewController.h"

@interface MBMetricViewController : MBOutlineViewController 
{
	IBOutlet NSMenu *newMenu;
}

#pragma mark "Constructors"
+ (MBMetricViewController *) viewForMetric:(MBMetric *)initMetric;
- (MBMetricViewController *) initWithMetric:(MBMetric *)initMetric;

#pragma mark UI Actions
- (IBAction) newClicked:(id)sender;
- (IBAction) recordClicked:(id)sender;
- (IBAction) summaryClicked:(id)sender;
- (IBAction) createRateMetricClicked:(id)sender;
- (IBAction) createPercentMetricClicked:(id)sender;
- (IBAction) addTriggerClicked:(id)sender;
- (IBAction) deleteMetricClicked:(id)sender;
- (IBAction) enumeratorsClicked:(id)sender;

#pragma mark Accessors
- (MBMetric *) metric;

@end
