//
//  LTIncidentListGroup.m
//  Lithium
//
//  Created by James Wilson on 16/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTIncidentListGroup.h"


@implementation LTIncidentListGroup

- (id) init
{
	[super init];
	
	children = [[NSMutableArray array] retain];
	
	return self;
}

@synthesize children;
@synthesize title;

@end
