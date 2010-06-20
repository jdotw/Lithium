//
//  LCMetricLevelIndicator.m
//  Lithium Console
//
//  Created by James Wilson on 15/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMetricLevelIndicator.h"
#import "LCTriangleBezierPath.h"
#import "LCTrigger.h"

@implementation LCMetricLevelIndicator

- (void) awakeFromNib
{
	NSRect newFrame = NSMakeRect([self frame].origin.x,[self frame].origin.y-6,[self frame].size.width,[self frame].size.height+6);
	[self setFrame:newFrame];
}

- (void) dealloc
{
	[super dealloc];
}

#pragma Binding Intervention

- (void)bind:(NSString *)binding 
	toObject:(id)observable
 withKeyPath:(NSString *)keyPath 
	 options:(NSDictionary *)options
{
	if ([binding isEqualToString:@"value"] )
	{
		[self bind:@"metric" toObject:observable withKeyPath:keyPath options:options];
		[super bind:binding toObject:observable withKeyPath:[NSString stringWithFormat:@"%@.currentValue.rawValueString", keyPath] options:options];
	}
	else
	{ [super bind:binding toObject:observable withKeyPath:keyPath options:options]; }
}

#pragma mark Drawing

- (void) drawRect:(NSRect)rect
{	
	/* Save context and set shadow */
	[NSGraphicsContext saveGraphicsState];
	NSShadow *theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(2.0, -2.0)]; 
	[theShadow setShadowBlurRadius:2];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.3]];	
	[theShadow set];
	
	/* Draw back */
	NSImage *bgImage = [NSImage imageNamed:@"level_slice_blank.png"];	
	[bgImage drawInRect:NSMakeRect(0 ,6, [self frame].size.width-4, [self frame].size.height-6)
			   fromRect:NSMakeRect(0,0,[bgImage size].width,[bgImage size].height)
			  operation:NSCompositeSourceOver
			   fraction:0.4];
	
	/* Restore */
	[NSGraphicsContext restoreGraphicsState];
	[theShadow release];
	
	/* Get max value */
	float maxValue = 100.0;
	if ([self maxValue] > 100.0) maxValue = [self maxValue];
	
	/* Draw gauge indicator */
	NSImage *gaugeImage = [NSImage imageNamed:@"level_slice.png"];	
	NSRect gaugeRect = NSMakeRect(0 ,6, ([self frame].size.width - 4) * ([self floatValue] / maxValue), [self frame].size.height-6);
	[gaugeImage drawInRect:gaugeRect
			   fromRect:NSMakeRect(0,0,[gaugeImage size].width,[gaugeImage size].height)
			  operation:NSCompositeSourceOver
			   fraction:0.8];
	
	/* Draw trigger triangles */
	for (LCTrigger *trg in [metric children])
	{
		NSColor *trgColor = [NSColor grayColor];
		switch (trg.effect)
		{
			case 1:
				trgColor = [NSColor yellowColor];
				break;
			case 2:
				trgColor = [NSColor orangeColor];
				break;
			case 3:
				trgColor = [NSColor redColor];
				break;
		}
		if (trg.triggerType == 5)
		{
			/* 
			 * Range trigger 
			 */
			/* X Val */
			NSBezierPath *triPath = [LCTriangleBezierPath rightTriangleInRect:NSMakeRect(NSMaxX([self bounds]) * (trg.xValue/100.0), 0, 5, 5)];
			[trgColor setFill];
			[triPath fill];
			
			/* Y Val */
			triPath = [LCTriangleBezierPath leftTriangleInRect:NSMakeRect((NSMaxX([self bounds]) * (trg.yValue/100.0))-5, 0, 5, 5)];
			[trgColor setFill];
			[triPath fill];				
		}
		else
		{
			/* GT/LT/Equal Trigger */
			NSBezierPath *triPath = nil;
			NSRect triRect = NSMakeRect((NSMaxX([self bounds]) * (trg.xValue/100)), 0, 5, 5);
			switch (trg.triggerType)
			{
				case 1:
					/* Equal */
					triPath = [LCTriangleBezierPath upTriangleInRect:triRect];
					break;
				case 2:
					/* Less */
					triPath = [LCTriangleBezierPath leftTriangleInRect:triRect];
					break;
				case 3:
					/* Great */
					triPath = [LCTriangleBezierPath rightTriangleInRect:triRect];
					break;
			}
			if (triPath)
			{
				[trgColor setFill];
				[triPath fill];
			}
		}
	}
	
	/* 
	 * History curve 
	 */
	
	if ([metric.metricValues count] > 2)
	{
		/* Get first and last values */
		LCMetricValue *firstValue = [metric.metricValues objectAtIndex:0];
		float firstValueFloat = firstValue.value / 100.0;
		if (firstValueFloat > 1.0) firstValueFloat = 1.0;
		LCMetricValue *lastValue = [metric.metricValues objectAtIndex:[metric.metricValues count]-1];
		float lastValueFloat = lastValue.value / 100.0;
		if (lastValueFloat > 1.0) lastValueFloat = 1.0;

		/* Get CP values */
		int cp1Index = ((float) [metric.metricValues count]) * 0.33;
		int cp2Index = ((float) [metric.metricValues count]) * 0.66;
		LCMetricValue *cp1Value = [metric.metricValues objectAtIndex:cp1Index];
		float cp1ValueFloat = cp1Value.value / 100.0;
		if (cp1ValueFloat > 1.0) cp1ValueFloat = 1.0;
		LCMetricValue *cp2Value = [metric.metricValues objectAtIndex:cp2Index];
		float cp2ValueFloat;
		if (cp2Value)
		{ cp2ValueFloat = cp2Value.value / 100.0; }
		else
		{ cp2ValueFloat = cp1Value.value / 100.0; }
		if (cp2ValueFloat > 1.0) cp2ValueFloat = 1.0;

		/* Create point */
		NSPoint startPoint = NSMakePoint(NSMinX(gaugeRect),NSMinY(gaugeRect) + (NSHeight(gaugeRect) * firstValueFloat));
		NSPoint endPoint = NSMakePoint(NSMaxX(gaugeRect),NSMinY(gaugeRect) + (NSHeight(gaugeRect) * lastValueFloat));
		NSPoint cp1Point = NSMakePoint(NSMinX(gaugeRect) + (0.33 * NSWidth(gaugeRect)),NSMinY(gaugeRect) + (NSHeight(gaugeRect) * cp1ValueFloat));
		NSPoint cp2Point = NSMakePoint(NSMinX(gaugeRect) + (0.66 * NSWidth(gaugeRect)),NSMinY(gaugeRect) + (NSHeight(gaugeRect) * cp2ValueFloat));
		
		/* Create Path */
		NSBezierPath *historyPath = [NSBezierPath bezierPath];
		[historyPath moveToPoint:startPoint];
		[historyPath curveToPoint:endPoint controlPoint1:cp1Point controlPoint2:cp2Point];
		[[metric opStateColorWithAlpha:1.0] setStroke];
		[historyPath setLineWidth:2.0];
		[historyPath stroke];
	}
}

- (void)mouseEntered:(NSEvent *)theEvent
{
	[super mouseEntered:theEvent];
}

- (void)mouseExited:(NSEvent *)theEvent
{
	[super mouseExited:theEvent];
}

@synthesize trackTag;
@synthesize metric;

@end
