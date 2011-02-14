//
//  LTMetricValue.m
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTMetricValue.h"


@implementation LTMetricValue

- (void) dealloc
{
    [timestamp release];
    [stringValue release];
    [super dealloc];
}

@synthesize floatValue;
@synthesize stringValue;
- (void) setStringValue:(NSString *)value
{
	[stringValue release];
    
    NSString *copy = [[value copy] autorelease];
	copy = [copy stringByReplacingOccurrencesOfString:@"bit" withString:@"b"];
	copy = [copy stringByReplacingOccurrencesOfString:@"byte" withString:@"B"];
    
    stringValue = [copy retain];
}
@synthesize timestamp;
@synthesize minValue;
@synthesize avgValue;
@synthesize maxValue;

@end
