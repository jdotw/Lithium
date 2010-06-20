//
//  LCGraphStadiumSliceLayer.m
//  Lithium Console
//
//  Created by James Wilson on 2/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCGraphStadiumSliceLayer.h"

#import "LCGraphStadiumSlice.h"
#import "LCMetricGraphController.h"
#import "LCObject.h"

@implementation LCGraphStadiumSliceLayer

- (CALayer *)hitTest:(CGPoint)thePoint
{
	if (CGRectContainsPoint([self bounds], thePoint))
	{
		NSNumber *sliceIndex = [self valueForKey:@"slice_index"];
		if (!sliceIndex) return [super hitTest:thePoint];		
		LCGraphStadiumSlice *slice = [controller.slices objectAtIndex:[sliceIndex intValue]];
		int graphIndex = slice.rows - ((int) slice.rows * (thePoint.y / [self bounds].size.height));
		if (graphIndex < [[slice graphControllers] count])
		{ 
			LCMetricGraphController *graphController = [slice.graphControllers objectAtIndex:graphIndex];
			LCObject *selectedObject = (LCObject *) [[[graphController.metricItems objectAtIndex:0] metric] parent];
			[self setValue:selectedObject forKey:@"selectedObject"];
		}
		else
		{ 
			[self setValue:nil forKey:@"selectedObject"]; 
		}
		
		return self;
	}
	else 
	{ return [super hitTest:thePoint]; }
}

@synthesize controller;

@end
