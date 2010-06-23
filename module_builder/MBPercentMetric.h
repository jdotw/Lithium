//
//  MBPercentMetric.h
//  ModuleBuilder
//
//  Created by James Wilson on 22/02/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MBMetric.h"

@interface MBPercentMetric : MBMetric 
{
	MBMetric *gaugeMetric;
	MBMetric *maxMetric;
	BOOL bindingDependents;
}

@property (assign) MBMetric *gaugeMetric;
@property (assign) MBMetric *maxMetric;

@end
