//
//  LCMetricGraphCompareButton.m
//  Lithium Console
//
//  Created by James Wilson on 6/02/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCMetricGraphCompareButton.h"


@implementation LCMetricGraphCompareButton

- (void) awakeFromNib
{

}

- (void) updateTrackingAreas
{
	[super updateTrackingAreas];
	if (trackingArea)
	{
		[self removeTrackingArea:trackingArea];
		[trackingArea release];
	}
	trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
												options:NSTrackingMouseEnteredAndExited|NSTrackingActiveInActiveApp
												  owner:self
											   userInfo:nil];
	[self addTrackingArea:trackingArea];
}

- (void) removeFromSuperview
{
	[self removeTrackingArea:trackingArea];
	[trackingArea release];
	[super removeFromSuperview];
}

- (BOOL)acceptsFirstResponder
{
	return YES; 
}

- (void) mouseEntered:(NSEvent *)event
{
	graphView.highlightBaselineView = YES;
}

- (void) mouseExited:(NSEvent *)event
{
	graphView.highlightBaselineView = NO;
}

@end
