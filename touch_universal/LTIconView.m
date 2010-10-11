//
//  LTIconView.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 12/10/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTIconView.h"


@implementation LTIconView


- (id)initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])) {
        // Initialization code
    }
    return self;
}

- (void)drawRect:(CGRect)rect 
{
	UIBezierPath *bezier = [UIBezierPath bezierPathWithOvalInRect:self.bounds];
	[[UIColor blueColor] setFill];
	[bezier fill];
}

- (void)dealloc {
    [super dealloc];
}



@end
