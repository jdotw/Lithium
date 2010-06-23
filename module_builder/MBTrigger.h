//
//  MBTrigger.h
//  ModuleBuilder
//
//  Created by James Wilson on 6/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MBEntity.h"


@interface MBTrigger : MBEntity 
{
}

- (NSNumber *) severity;
- (void) setSeverity:(NSNumber *)value;

- (NSNumber *) condition;
- (void) setCondition:(NSNumber *)value;

- (NSString *) xValue;
- (void) setXValue:(NSString *)value;

- (NSString *) yValue;
- (void) setYValue:(NSString *)value;

- (BOOL) hasSecondValue;
- (void) setHasSecondValue:(BOOL)flag;

- (NSNumber *) duration;
- (void) setDuration:(NSNumber *)value;

- (NSString *) conditionMatchString;

#pragma mark "Overlap Handling"
- (MBTrigger *) checkForOverlap;

@end
