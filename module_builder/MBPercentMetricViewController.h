//
//  MBPercentMetricViewController.h
//  ModuleBuilder
//
//  Created by James Wilson on 22/02/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MBMetric.h"
#import "MBMetricTextField.h"
#import "MBMetricViewController.h"

@interface MBPercentMetricViewController : MBMetricViewController 
{
	IBOutlet MBMetricTextField *gaugeMetricField;
	IBOutlet MBMetricTextField *maxMetricField;
}

#pragma mark "Constructors"
+ (MBPercentMetricViewController *) viewForMetric:(MBMetric *)initMetric;
- (MBPercentMetricViewController *) initWithMetric:(MBMetric *)initMetric;

#pragma mark Accessors
- (MBMetric *) metric;



@end
