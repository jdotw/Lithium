//
//  MBMetricPropertiesView.m
//  ModuleBuilder
//
//  Created by James Wilson on 22/02/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MBMetricPropertiesView.h"


@implementation MBMetricPropertiesView

- (id)initWithFrame:(NSRect)frame 
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    return self;
}

- (void)drawRect:(NSRect)rect 
{
	NSBezierPath *backPath = [NSBezierPath bezierPathWithRect:rect];
	[[NSColor colorWithCalibratedWhite:0.0 alpha:0.7] setFill];
	[backPath fill];
}

@end
