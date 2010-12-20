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

- (void)drawRect:(CGRect)rect 
{
	/* Draw min/avg/max lines */
	CGRect minLineRect = CGRectMake(CGRectGetMinX(rect), roundf(CGRectGetMidY(minLabel.frame)), CGRectGetWidth(rect), 1.0);
	if (CGRectContainsRect(rect, minLineRect))
	{
		UIBezierPath *innerPath = [UIBezierPath bezierPathWithRect:minLineRect];
		[[UIColor colorWithWhite:0.0 alpha:0.1] setFill];
		[innerPath fill];
		UIBezierPath *outerPath = [UIBezierPath bezierPathWithRect:CGRectOffset(minLineRect, 0.0, 1.0)];
		[[UIColor colorWithWhite:1.0 alpha:0.1] setFill];
		[outerPath fill];
	}
	CGRect midLineRect = CGRectMake(CGRectGetMinX(rect), roundf(CGRectGetMidY(avgLabel.frame)), CGRectGetWidth(rect), 1.0);
	if (CGRectContainsRect(rect, midLineRect))
	{
		UIBezierPath *innerPath = [UIBezierPath bezierPathWithRect:midLineRect];
		[[UIColor colorWithWhite:0.0 alpha:0.1] setFill];
		[innerPath fill];
		UIBezierPath *outerPath = [UIBezierPath bezierPathWithRect:CGRectOffset(midLineRect, 0.0, 1.0)];
		[[UIColor colorWithWhite:1.0 alpha:0.1] setFill];
		[outerPath fill];
	}
	CGRect maxLineRect = CGRectMake(CGRectGetMinX(rect), roundf(CGRectGetMidY(maxLabel.frame)), CGRectGetWidth(rect), 1.0);
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
