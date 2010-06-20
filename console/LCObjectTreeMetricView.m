//
//  LCObjectTreeMetricView.m
//  Lithium Console
//
//  Created by James Wilson on 11/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCObjectTreeMetricView.h"


@implementation LCObjectTreeMetricView

- (NSView *)hitTest:(NSPoint)aPoint
{
	if (NSPointInRect([[self superview] convertPoint:aPoint toView:metricField], [metricField bounds])) 
	{
		if (NSPointInRect([[self superview] convertPoint:aPoint toView:metricField.historyButton], [metricField.historyButton bounds]))
		{ return metricField.historyButton; }
		else
		{ return metricField; }
	}
	else return nil;
}

@end
