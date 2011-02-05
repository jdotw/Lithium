//
//  LTIncidentListGroup.m
//  Lithium
//
//  Created by James Wilson on 16/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTIncidentListGroup.h"
#import "LTIncident.h"

@implementation LTIncidentListGroup

- (id) init
{
	[super init];
	
	children = [[NSMutableArray array] retain];
	
	return self;
}

@synthesize children;
@synthesize title;

- (int) highestEntityState
{
    int highestEntityState = 0;
    for (LTIncident *inc in children)
    { 
        if (inc.entityDescriptor.opState > highestEntityState) 
        { highestEntityState = inc.entityDescriptor.opState; }
    }
    return highestEntityState;
}

@end
