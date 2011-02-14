//
//  LTMetricValue.h
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface LTMetricValue : NSObject 
{
	NSDate *timestamp;

	/* Instantaneous Value */
	float floatValue;
	NSString *stringValue;
	
	/* Recorded (RRD) Value */
	float minValue;
	float avgValue;
	float maxValue;
}

@property (nonatomic,assign) float floatValue;
@property (nonatomic,copy) NSString *stringValue;
@property (nonatomic,copy) NSDate *timestamp;
@property (nonatomic,assign) float minValue;
@property (nonatomic,assign) float avgValue;
@property (nonatomic,assign) float maxValue;

@end
