//
//  MBRateMetric.h
//  ModuleBuilder
//
//  Created by James Wilson on 22/02/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MBMetric.h"

@interface MBRateMetric : MBMetric 
{
	MBMetric *counterMetric;
	BOOL bindingDependents;
}

@property (assign) MBMetric *counterMetric;
@property (assign) BOOL convertBytesToBits;

@end
