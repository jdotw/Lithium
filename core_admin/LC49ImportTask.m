//
//  LC49ImportTask.m
//  LCAdminTools
//
//  Created by James Wilson on 2/06/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LC49ImportTask.h"


@implementation LC49ImportTask

- (id) init
{
	[super init];
	
	self.isIndeterminate = YES;
	
	return self;
}

@synthesize name;
@synthesize result;
@synthesize icon;

@synthesize progressPercent;
@synthesize isIndeterminate;

- (void) setSucceeded
{ self.icon = [NSImage imageNamed:@"ok_16"]; }
- (void) setFailed
{ self.icon = [NSImage imageNamed:@"stop_16"]; }

@end
