//
//  LCTrigger.h
//  Lithium Console
//
//  Created by James Wilson on 27/09/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"

@interface LCTrigger : LCEntity 
{
	/* Trigger Properties */
	int valueType;
	int triggerType;
	int effect;
	unsigned long duration;
	unsigned long activeTimeStamp;
	float xValue;
	float yValue;
	NSString *xValueString;
	NSString *yValueString;
	
}

#define TRGTYPE_EQUAL 1                         /* Metric = Trigger */
#define TRGTYPE_LT 2                            /* Metric < Trigger */
#define TRGTYPE_GT 3                            /* Metric > Trigger */
#define TRGTYPE_NOTEQUAL 4                      /* Metric != Trigger */
#define TRGTYPE_RANGE 5                         /* xValue <= Trigger < yValue */

- (LCTrigger *) init;

@property (nonatomic, assign) int valueType;
@property (nonatomic, assign) int triggerType;
@property (nonatomic, assign) int effect;
@property (nonatomic, assign) unsigned long duration;
@property (nonatomic, assign) unsigned long activeTimeStamp;
@property (nonatomic, assign) float xValue;
@property (nonatomic, assign) float yValue;
@property (nonatomic,copy) NSString *xValueString;
@property (nonatomic,copy) NSString *yValueString;

- (void) updateDisplayValue;

@end
