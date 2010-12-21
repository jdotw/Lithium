//
//  LTMetricGraphTableViewCellBackground.m
//  Lithium
//
//  Created by James Wilson on 19/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LTMetricGraphTableViewCellBackground.h"


@implementation LTMetricGraphTableViewCellBackground


- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) 
	{
        // Initialization code
		imageView = [[UIImageView alloc] initWithFrame:CGRectZero];
		imageView.image = [UIImage imageNamed:@"LTMetricTableViewGraphBack"];
		
		imageView.alpha = 0.4;
		[self addSubview:imageView];
    }
    return self;
}

- (void) layoutSubviews
{
	imageView.frame = self.bounds;
}

- (void)drawRect:(CGRect)rect 
{
	/* Draw back */
	[[UIColor colorWithWhite:0.16 alpha:1.0] setFill];
	UIRectFill(self.bounds);
	
	// Ridge lines
	CGRect topLineRect;
	
	// Bottom (Dark)
	[[UIColor colorWithRed:20.0/255.0 green:20.0/255.0 blue:20.0/255.0 alpha:1.0] setFill];
	topLineRect = CGRectMake(CGRectGetMinX(self.bounds), CGRectGetMaxY(self.bounds)-1.0, CGRectGetWidth(self.bounds), 1.0);
	UIRectFill(topLineRect);
}


- (void)dealloc {
    [super dealloc];
}


@end
