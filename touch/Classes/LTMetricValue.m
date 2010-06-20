//
//  LTMetricValue.m
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTMetricValue.h"


@implementation LTMetricValue

@synthesize floatValue;
@synthesize stringValue;
- (void) setStringValue:(NSString *)value
{
	[stringValue release];
	stringValue = [value copy];
	stringValue = [[stringValue stringByReplacingOccurrencesOfString:@"bit" withString:@"b"] copy];
	stringValue = [[stringValue stringByReplacingOccurrencesOfString:@"byte" withString:@"B"] copy];
}
@synthesize timestamp;
@synthesize minValue;
@synthesize avgValue;
@synthesize maxValue;

@end
