//
//  LTSubDeviceTableViewCellBackgroundView.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 7/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTSubDeviceTableViewCellBackgroundView.h"


@implementation LTSubDeviceTableViewCellBackgroundView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        self.opaque = NO;
    }
    return self;
}

- (void)drawRect:(CGRect)rect
{
    [[UIColor colorWithWhite:0.2 alpha:1.0] setFill];
    UIRectFill(self.bounds);
    
    /* Draw two-line top/bottom divider */
    CGRect dividerRect = CGRectMake(0., CGRectGetMinY(self.bounds), CGRectGetWidth(self.bounds), 1.);
    [[UIColor colorWithWhite:101./255. alpha:0.6] setFill];
    [[UIBezierPath bezierPathWithRect:dividerRect] fill];
    dividerRect = CGRectMake(0., CGRectGetMaxY(self.bounds)-1., CGRectGetWidth(self.bounds), 1.);
    [[UIColor colorWithWhite:0.0 alpha:0.75] setFill];
    [[UIBezierPath bezierPathWithRect:dividerRect] fill];
}

- (void)dealloc
{
    [super dealloc];
}

@end
