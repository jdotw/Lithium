//
//  LTGraphBackgroundView.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 5/11/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTGraphBackgroundView.h"


@implementation LTGraphBackgroundView

@synthesize minLabel, avgLabel, maxLabel;

- (CGRect) minLineRect
{
    return CGRectMake(CGRectGetMinX(self.bounds), roundf(CGRectGetMidY(minLabel.frame)), CGRectGetWidth(self.bounds), 1.0);
}

- (CGRect) midLineRect
{
    return CGRectMake(CGRectGetMinX(self.bounds), roundf(CGRectGetMidY(avgLabel.frame)), CGRectGetWidth(self.bounds), 1.0);
}

- (CGRect) maxLineRect
{
    return CGRectMake(CGRectGetMinX(self.bounds), roundf(CGRectGetMidY(maxLabel.frame)), CGRectGetWidth(self.bounds), 1.0);
}

- (void)drawRect:(CGRect)rect 
{
	/* Draw min/avg/max lines */
	CGRect minLineRect = self.minLineRect;
	if (CGRectContainsRect(rect, minLineRect))
	{
		UIBezierPath *innerPath = [UIBezierPath bezierPathWithRect:minLineRect];
		[[UIColor colorWithWhite:0.0 alpha:0.1] setFill];
		[innerPath fill];
		UIBezierPath *outerPath = [UIBezierPath bezierPathWithRect:CGRectOffset(minLineRect, 0.0, 1.0)];
		[[UIColor colorWithWhite:1.0 alpha:0.1] setFill];
		[outerPath fill];
	}
	CGRect midLineRect = self.midLineRect;
	if (CGRectContainsRect(rect, midLineRect))
	{
		UIBezierPath *innerPath = [UIBezierPath bezierPathWithRect:midLineRect];
		[[UIColor colorWithWhite:0.0 alpha:0.1] setFill];
		[innerPath fill];
		UIBezierPath *outerPath = [UIBezierPath bezierPathWithRect:CGRectOffset(midLineRect, 0.0, 1.0)];
		[[UIColor colorWithWhite:1.0 alpha:0.1] setFill];
		[outerPath fill];
	}
	CGRect maxLineRect = self.maxLineRect;
	if (CGRectContainsRect(rect, maxLineRect))
	{
		UIBezierPath *innerPath = [UIBezierPath bezierPathWithRect:maxLineRect];
		[[UIColor colorWithWhite:0.0 alpha:0.1] setFill];
		[innerPath fill];
		UIBezierPath *outerPath = [UIBezierPath bezierPathWithRect:CGRectOffset(maxLineRect, 0.0, 1.0)];
		[[UIColor colorWithWhite:1.0 alpha:0.1] setFill];
		[outerPath fill];
	}	
}


@end
