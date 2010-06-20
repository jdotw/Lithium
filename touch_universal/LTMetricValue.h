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

@property (assign) float floatValue;
@property (copy) NSString *stringValue;
@property (copy) NSDate *timestamp;
@property (assign) float minValue;
@property (assign) float avgValue;
@property (assign) float maxValue;

@end
