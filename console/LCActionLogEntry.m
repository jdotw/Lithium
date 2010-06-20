//
//  LCActionLogEntry.m
//  Lithium Console
//
//  Created by James Wilson on 3/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCActionLogEntry.h"


@implementation LCActionLogEntry

- (id) init
{
	self = [super init];
	if (self != nil) 
	{
		if (!self.xmlTranslation) self.xmlTranslation = [NSMutableDictionary dictionary];
		[self.xmlTranslation setObject:@"logID" forKey:@"id"];
		[self.xmlTranslation setObject:@"timestampSeconds" forKey:@"timestamp"];
		[self.xmlTranslation setObject:@"comment" forKey:@"comment"];
	}
	return self;
}

#pragma mark "Properties"
@synthesize logID;
@synthesize timestampSeconds;
- (void) setTimestampSeconds:(int)value
{
	timestampSeconds = value;
	self.timestamp = [NSDate dateWithTimeIntervalSince1970:(float)value];
}
@synthesize timestamp;
@synthesize comment;

@end
