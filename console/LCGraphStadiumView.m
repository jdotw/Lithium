//
//  LCGraphStadiumView.m
//  Lithium Console
//
//  Created by James Wilson on 2/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCGraphStadiumView.h"

#import "LCGraphStadiumSlice.h"
#import "LCGraphFlowCard.h"

@implementation LCGraphStadiumView

- (void) setFrame:(NSRect)frameRect
{
	[super setFrame:frameRect];
	[controller.bodyLayer setNeedsLayout];
}

- (void)mouseDown:(NSEvent *)theEvent
{
	CALayer *hitLayer = (CALayer *) [[self layer] hitTest:NSPointToCGPoint([self convertPoint:[theEvent locationInWindow] fromView:nil])];
	
	/* Check for a slice (stadium) */
	NSNumber *sliceIndex = [hitLayer valueForKey:@"slice_index"];
	if (sliceIndex)
	{
		LCGraphStadiumSlice *slice = [controller.slices objectAtIndex:[sliceIndex intValue]];
		slice.stadiumController.selectedEntity = [slice.imageLayer valueForKey:@"selectedObject"];
	}
	
	/* Check for a metric (flow) */
	LCMetric *metric = [hitLayer valueForKey:@"metric"];
	if (metric)
	{
		LCGraphFlowCard *card = [controller.flowController.cardDictionary objectForKey:[metric uniqueIdentifier]];
		if (card && card.orientation == FLAT)
		{ 
			if ([delegate respondsToSelector:@selector(setDoNotScrollFlow:)])
			{ [delegate setDoNotScrollFlow:YES]; }
		}
		controller.selectedEntity = metric;
		if ([delegate respondsToSelector:@selector(setDoNotScrollFlow:)])
		{ [delegate setDoNotScrollFlow:NO]; }
	}
}

@end
