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

@property (nonatomic, assign) double minGradient;
@property (nonatomic, assign) double minIntercept;
@property (nonatomic, assign) double avgGradient;
@property (nonatomic, assign) double avgIntercept;
@property (nonatomic, assign) double maxGradient;
@property (nonatomic, assign) double maxIntercept;

@property (nonatomic,copy) NSDate *minDate;
@property (nonatomic,copy) NSDate *avgDate;
@property (nonatomic,copy) NSDate *maxDate;

@property (readonly) double xTriggerValue;
@property (readonly) double yTriggerValue;

@property (nonatomic,retain) LCEntity *baseEntity;
@property (readonly) LCEntity *entity;

@end
