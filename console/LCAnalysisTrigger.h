//
//  LCAnalysisTrigger.h
//  Lithium Console
//
//  Created by Liam Elliott on 29/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCEntity.h"

@interface LCAnalysisTrigger : NSObject 
{	
	LCEntity *baseEntity;
	
	double minGradient;
	double minIntercept;
	NSDate *minDate;
	
	double avgGradient;
	double avgIntercept;
	NSDate *avgDate;
	
	double maxGradient;
	double maxIntercept;
	NSDate *maxDate;

}

+ (id) analysisTriggerWithEntity:(LCEntity *)entity;
- (id) initWithEntity:(LCEntity *) entity;

@property (assign) double minGradient;
@property (assign) double minIntercept;
@property (assign) double avgGradient;
@property (assign) double avgIntercept;
@property (assign) double maxGradient;
@property (assign) double maxIntercept;

@property (copy) NSDate *minDate;
@property (copy) NSDate *avgDate;
@property (copy) NSDate *maxDate;

@property (readonly) double xTriggerValue;
@property (readonly) double yTriggerValue;

@property (retain) LCEntity *baseEntity;
@property (readonly) LCEntity *entity;

@end
