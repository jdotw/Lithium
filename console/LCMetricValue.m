//
//  LCMetricValue.m
//  Lithium Console
//
//  Created by James Wilson on 1/10/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMetricValue.h"


@implementation LCMetricValue

- (id) initWithXmlNode:(LCXMLNode *)xmlNode
{
	self = [self init];
	if (self)
	{
		/* Parse Value */
		self.valueString = [xmlNode.properties objectForKey:@"valstr"];
		self.rawValueString = [xmlNode.properties objectForKey:@"valstr_raw"];
		self.timestamp = (unsigned long) [[xmlNode.properties objectForKey:@"tstamp_sec"] longLongValue];		
	}
	return self;
}

- (void) dealloc
{
	[valueString release];
	[rawValueString release];
	[super dealloc];
}

@synthesize value;
@synthesize valueString;

- (void) setValueString:(NSString *)string
{
	[valueString release];
	valueString = [string copy];
	self.value = [valueString floatValue];
}

@synthesize rawValueString;
@synthesize timestamp;

@end
