//
//  LCMetricAnalysisValue.h
//  Lithium Console
//
//  Created by Liam Elliott on 31/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCMetricAnalysisValue : NSObject {
	
	double timestamp;
	double value;

}


+(LCMetricAnalysisValue *) valueWithTimestamp:(double) ts andValue:(double)val;
-(LCMetricAnalysisValue *)init;

-(void) dealloc;

-(double)timestamp;
-(void)setTimestamp:(double)val;

-(double)value;
-(void)setValue:(double)val;

@property (getter=timestamp,setter=setTimestamp:) double timestamp;
@property (getter=value,setter=setValue:) double value;
@end
