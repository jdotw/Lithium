//
//  MBRateMetricViewController.h
//  ModuleBuilder
//
//  Created by James Wilson on 22/02/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MBMetric.h"
#import "MBMetricTextField.h"
#import "MBRateMetric.h"
#import "MBMetricViewController.h"

@interface MBRateMetricViewController : MBMetricViewController 
{
	IBOutlet MBMetricTextField *counterMetricField;
}

#pragma mark "Constructors"
+ (MBRateMetricViewController *) viewForMetric:(MBMetric *)initMetric;
- (MBRateMetricViewController *) initWithMetric:(MBMetric *)initMetric;

#pragma mark Accessors
- (MBRateMetric *) metric;


@end
