//
//  LTTableViewSectionHeaderView.m
//  Lithium
//
//  Created by James Wilson on 2/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTTableViewSectionHeaderView.h"


@implementation LTTableViewSectionHeaderView


- (id)initWithFrame:(CGRect)frame 
{
    if (self = [super initWithFrame:frame]) 
	{
        // Initialization code
		imageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"TableViewSectionHeader.png"]];
		[self addSubview:imageView];

		titleLabel = [[UILabel alloc] initWithFrame:CGRectZero];
		titleLabel.backgroundColor = [UIColor clearColor];
		titleLabel.opaque = NO;
		titleLabel.textColor = [UIColor whiteColor];
		titleLabel.highlightedTextColor = [UIColor whiteColor];
		titleLabel.font = [UIFont boldSystemFontOfSize:14.0];
		titleLabel.shadowColor = [UIColor darkGrayColor];
		titleLabel.shadowOffset = CGSizeMake(0.0, 1.0);
		[self addSubview:titleLabel];
		
    }
    return self;
}

- (void)layoutSubviews 
{
	titleLabel.frame = CGRectMake(CGRectGetMinX(self.bounds) + 8.0, CGRectGetMinY(self.bounds), CGRectGetWidth(self.bounds), CGRectGetHeight(self.bounds));
	imageView.frame = self.bounds;
}

- (void)dealloc 
{
    [super dealloc];
}

@synthesize titleLabel;

@end
