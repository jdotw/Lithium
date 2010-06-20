//
//  LTMetricTableViewCellBackground.m
//  Lithium
//
//  Created by James Wilson on 19/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LTMetricTableViewCellBackground.h"


@implementation LTMetricTableViewCellBackground


- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        // Initialization code
    }
    return self;
}


- (void)drawRect:(CGRect)rect 
{
	UIImage *backImage = [UIImage imageNamed:@"mediumgraphback.png"]; 
	[backImage drawInRect:self.bounds
				blendMode:kCGBlendModeSourceAtop 
					alpha:1.0f];
	
	// Ridge lines
	CGRect topLineRect;
	
	// Bottom (Dark)
	[[UIColor colorWithRed:20.0/255.0 green:20.0/255.0 blue:20.0/255.0 alpha:1.0] setFill];
	topLineRect = CGRectMake(CGRectGetMinX(self.bounds), CGRectGetMaxY(self.bounds)-1.0, CGRectGetWidth(self.bounds), 1.0);
	UIRectFill(topLineRect);
	
	// Top (Light)
	[[UIColor colorWithRed:120.0/255.0 green:120.0/255.0 blue:120.0/255.0 alpha:1.0] setFill];
	topLineRect = CGRectMake(CGRectGetMinX(self.bounds), CGRectGetMinY(self.bounds)+0.0, CGRectGetWidth(self.bounds), 1.0);
	UIRectFill(topLineRect);	
}


- (void)dealloc {
    [super dealloc];
}


@end
