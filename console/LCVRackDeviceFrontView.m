//
//  LCVRackDeviceFrontView.m
//  Lithium Console
//
//  Created by James Wilson on 12/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCVRackDeviceFrontView.h"

@implementation LCVRackDeviceFrontView

#pragma mark Drawing

- (void)drawRect:(NSRect)rect 
{
	[super drawRect:rect];
	
    NSSize size = [self bounds].size;
	
	/* 
	 * CPU Load indicator 
	 */
	
	/* Calculate sizes */
	float cpuMarginX = size.width * 0.4;		/* Margin before start of cpu control */
	float cpuWidthPercent = 0.025;
	float cpuHeightPercent = 0.85;
	NSRect cpuRect = NSMakeRect([self bounds].origin.x + cpuMarginX, 
								[self bounds].origin.y + (size.height - [self pointsPerRU]) + (0.5 * ((1 - cpuHeightPercent) * [self pointsPerRU])),
								size.width * cpuWidthPercent, 
								[self pointsPerRU] * cpuHeightPercent);
	
	/* Get CPU and Metric */
	LCEntity *cpuContainer = [[[rackDevice entity] childrenDictionary] objectForKey:@"cpu"];
	if (!cpuContainer)
	{ cpuContainer = [[[rackDevice entity] childrenDictionary] objectForKey:@"hrcpu"]; }
	if (!cpuContainer)
	{ cpuContainer = [[[rackDevice entity] childrenDictionary] objectForKey:@"xcpu"]; }
	if (!cpuContainer)
	{ cpuContainer = [[[rackDevice entity] childrenDictionary] objectForKey:@"xscpu"]; }
	NSEnumerator *cpuEnum = [[cpuContainer childrenDictionary] objectEnumerator];
	LCEntity *cpu = [cpuEnum nextObject];
	LCEntity *cpuMetric = [[cpu childrenDictionary] objectForKey:@"onemin_pc"];
	if (!cpuMetric)
	{ cpuMetric = [[cpu childrenDictionary] objectForKey:@"load_pc"]; }
	if (!cpuMetric)
	{ cpuMetric = [[cpu childrenDictionary] objectForKey:@"usage"]; }
	if (cpuMetric && [[cpuMetric properties] objectForKey:@"valstr_raw"])
	{
		/* Get Value */
		NSString *valueStr = [[cpuMetric properties] objectForKey:@"valstr_raw"];
		float cpuPercent = [valueStr floatValue] / 100;

		/* Draw outline */
		[[NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.8 alpha:0.8] setStroke];
		NSBezierPath *cpuPath = [NSBezierPath bezierPathWithRect:cpuRect];
		[cpuPath setLineWidth:0.1];
		[cpuPath stroke];
		
		/* Create and fill the indicator */
		NSRect cpuIndicatorRect = NSMakeRect(cpuRect.origin.x, cpuRect.origin.y, cpuRect.size.width, cpuRect.size.height * cpuPercent);
		[[cpuMetric opStateColor] setFill];
		NSRectFill(cpuIndicatorRect);
		
		/* Text */
		NSString *cpuLabel = @"CPU";
		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.8], NSForegroundColorAttributeName,
			[NSFont boldSystemFontOfSize: [self pointsPerRU]*0.25], NSFontAttributeName,
			nil];
		[cpuLabel drawAtPoint:NSMakePoint(cpuRect.origin.x + cpuRect.size.width + (0.4 * cpuRect.size.width), 
										  cpuRect.origin.y + (0.5 * cpuRect.size.height) - (0.5 * ([self pointsPerRU]*0.25)))
			   withAttributes:attr];
		
		/* Update Total Size (including label) */
		cpuRect.size.width += [cpuLabel sizeWithAttributes:attr].width;
	}
	
	/* 
	 * Storage Resources 
	 */
	
	LCEntity *store1 = nil;
	LCEntity *store2 = nil;
	LCEntity *store3 = nil;
	LCEntity *store4 = nil;
	if ([[[rackDevice entity] childrenDictionary] objectForKey:@"mempool"])
	{
		/* Device has mempools */
		/* Set up the entities */
		int i = 0;
		NSEnumerator *objEnum = [[[[[rackDevice entity] childrenDictionary] objectForKey:@"mempool"] children] objectEnumerator];
		LCEntity *obj;
		while ((obj=[objEnum nextObject])!=nil)
		{
			switch (i)
			{
				case 0:
					store1 = obj;
					break;
				case 1:
					store2 = obj;
					break;
				case 2:
					store3 = obj;
					break;
				case 3:
					store4 = obj;
					break;
			}
			i++;
		}
	}
	else if ([[[rackDevice entity] childrenDictionary] objectForKey:@"storage"])
	{
		/* Device has storage resource */
		/* Set up the entities 
		 * FIX ... We probably want to be more selective here 
		 */
		int i = 0;
		NSEnumerator *objEnum = [[[[[rackDevice entity] childrenDictionary] objectForKey:@"storage"] children] objectEnumerator];
		LCEntity *obj;
		while ((obj=[objEnum nextObject])!=nil)
		{
			switch (i)
			{
				case 0:
					store1 = obj;
					break;
				case 1:
					store2 = obj;
					break;
				case 2:
					store3 = obj;
					break;
				case 3:
					store4 = obj;
					break;
			}
			i++;
		}
	}
	else if ([[[rackDevice entity] childrenDictionary] objectForKey:@"xvolumes"])
	{
		/* Device has storage resource */
		/* Set up the entities 
		* FIX ... We probably want to be more selective here 
		*/
		int i = 0;
		NSEnumerator *objEnum = [[[[[rackDevice entity] childrenDictionary] objectForKey:@"xvolumes"] children] objectEnumerator];
		LCEntity *obj;
		while ((obj=[objEnum nextObject])!=nil)
		{
			switch (i)
			{
				case 0:
					store1 = obj;
					break;
				case 1:
					store2 = obj;
					break;
				case 2:
					store3 = obj;
					break;
				case 3:
					store4 = obj;
					break;
			}
			i++;
		}
	}
	
	/* Set variables */
	float storeRow1MarginX = size.width * 0.50;		/* Margin before start of storage controls */
	float storeRow2MarginX = size.width * 0.73;		/* Margin before start of storage controls */
	
	/* Draw Store 1 */
	if (store1)
	{
		/* Text */
		NSMutableString *storeLabel = [NSMutableString stringWithString:[store1 displayString]];
		if ([storeLabel length] > 4)
		{
			NSRange trimRange = NSMakeRange(4, [storeLabel length]-4);
			[storeLabel deleteCharactersInRange:trimRange];
		}
		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.8], NSForegroundColorAttributeName,
			[NSFont boldSystemFontOfSize: [self pointsPerRU] * 0.25], NSFontAttributeName,
			nil];
		[storeLabel drawAtPoint:NSMakePoint(storeRow1MarginX, 
											[self bounds].origin.y + (size.height - (0.50 * [self pointsPerRU])) + (0.1 * [self pointsPerRU])) 
				 withAttributes:attr];
		
		/* Gauge rect */
		NSRect gaugeRect = NSMakeRect(storeRow1MarginX + (size.width * 0.04), 
									  [self bounds].origin.y + (size.height - (0.50 * [self pointsPerRU]) + (0.1 * [self pointsPerRU])), 
									  storeRow2MarginX - (storeRow1MarginX + (size.width * 0.05)), 
									  (0.24 * [self pointsPerRU]));
		[[NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.8 alpha:0.8] setStroke];
		NSBezierPath *gaugePath = [NSBezierPath bezierPathWithRect:gaugeRect];
		[gaugePath setLineWidth:0.1];
		[gaugePath stroke];
		
		/* Draw indicator rect */
		LCEntity *utilMetric = [[store1 childrenDictionary] objectForKey:@"used_pc"];
		float utilFactor = 0.00;
		if (utilMetric && [[utilMetric properties] objectForKey:@"valstr_raw"])
		{
			utilFactor = [[[utilMetric properties] objectForKey:@"valstr_raw"] floatValue] / 100.00;
			if (utilFactor > 1) utilFactor = 1.00;
		}
		NSRect indicRect = NSMakeRect (gaugeRect.origin.x, gaugeRect.origin.y, gaugeRect.size.width * utilFactor, gaugeRect.size.height);
		[[store1 opStateColor] setFill];
		NSRectFill(indicRect);
	}

	/* Draw store 2 */
	if (store2)
	{
		/* Text */
		NSMutableString *storeLabel = [NSMutableString stringWithString:[store2 desc]];
		if ([storeLabel length] > 4)
		{
			NSRange trimRange = NSMakeRange(4, [storeLabel length]-4);
			[storeLabel deleteCharactersInRange:trimRange];
		}
		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.8], NSForegroundColorAttributeName,
			[NSFont boldSystemFontOfSize: [self pointsPerRU]*0.25], NSFontAttributeName,
			nil];
		[storeLabel drawAtPoint:NSMakePoint(storeRow1MarginX, 
											[self bounds].origin.y + (size.height - (0.75 * [self pointsPerRU])) - (0.1 * [self pointsPerRU])) 
				 withAttributes:attr];
		
		/* Gauge rect */
		NSRect gaugeRect = NSMakeRect(storeRow1MarginX + (size.width * 0.04), 
									  [self bounds].origin.y + (size.height - (0.75 * [self pointsPerRU]) - (0.1 * [self pointsPerRU])), 
									  storeRow2MarginX - (storeRow1MarginX + (size.width * 0.05)), 
									  (0.24 * [self pointsPerRU]));
		[[NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.8 alpha:0.8] setStroke];
		NSBezierPath *gaugePath = [NSBezierPath bezierPathWithRect:gaugeRect];
		[gaugePath setLineWidth:0.1];
		[gaugePath stroke];

		/* Draw indicator rect */
		LCEntity *utilMetric = [[store2 childrenDictionary] objectForKey:@"used_pc"];
		float utilFactor = 0.00;
		if (utilMetric && [[utilMetric properties] objectForKey:@"valstr_raw"])
		{
			utilFactor = [[[utilMetric properties] objectForKey:@"valstr_raw"] floatValue] / 100.00;
			if (utilFactor > 1) utilFactor = 1.00;
		}
		NSRect indicRect = NSMakeRect (gaugeRect.origin.x, gaugeRect.origin.y, gaugeRect.size.width * utilFactor, gaugeRect.size.height);
		[[store2 opStateColor] setFill];
		NSRectFill(indicRect);
		
	}
	
	/* Draw Store 3 */
	if (store3)
	{
		/* Text */
		NSMutableString *storeLabel = [NSMutableString stringWithString:[store3 desc]];
		if ([storeLabel length] > 4)
		{
			NSRange trimRange = NSMakeRange(4, [storeLabel length]-4);
			[storeLabel deleteCharactersInRange:trimRange];
		}
		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.8], NSForegroundColorAttributeName,
			[NSFont boldSystemFontOfSize: [self pointsPerRU] * 0.25], NSFontAttributeName,
			nil];
		[storeLabel drawAtPoint:NSMakePoint(storeRow2MarginX, 
											[self bounds].origin.y + (size.height - (0.50 * [self pointsPerRU])) + (0.1 * [self pointsPerRU])) 
				 withAttributes:attr];
		
		/* Gauge rect */
		NSRect gaugeRect = NSMakeRect(storeRow2MarginX + (size.width * 0.04), 
									  [self bounds].origin.y + (size.height - (0.50 * [self pointsPerRU]) + (0.1 * [self pointsPerRU])), 
									  (size.width - storeRow2MarginX - (size.width * 0.08)), 
									  (0.24 * [self pointsPerRU]));
		[[NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.8 alpha:0.8] setStroke];
		NSBezierPath *gaugePath = [NSBezierPath bezierPathWithRect:gaugeRect];
		[gaugePath setLineWidth:0.1];
		[gaugePath stroke];
		
		/* Draw indicator rect */
		LCEntity *utilMetric = [[store3 childrenDictionary] objectForKey:@"used_pc"];
		float utilFactor = 0.00;
		if (utilMetric && [[utilMetric properties] objectForKey:@"valstr_raw"])
		{
			utilFactor = [[[utilMetric properties] objectForKey:@"valstr_raw"] floatValue] / 100.00;
			if (utilFactor > 1) utilFactor = 1.00;
		}
		NSRect indicRect = NSMakeRect (gaugeRect.origin.x, gaugeRect.origin.y, gaugeRect.size.width * utilFactor, gaugeRect.size.height);
		[[store3 opStateColor] setFill];
		NSRectFill(indicRect);
	}
	
	/* Draw store 4 */
	if (store4)
	{
		/* Text */
		NSMutableString *storeLabel = [NSMutableString stringWithString:[store4 desc]];
		if ([storeLabel length] > 4)
		{
			NSRange trimRange = NSMakeRange(4, [storeLabel length]-4);
			[storeLabel deleteCharactersInRange:trimRange];
		}
		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.8], NSForegroundColorAttributeName,
			[NSFont boldSystemFontOfSize: [self pointsPerRU]*0.25], NSFontAttributeName,
			nil];
		[storeLabel drawAtPoint:NSMakePoint(storeRow2MarginX, 
											[self bounds].origin.y + (size.height - (0.75 * [self pointsPerRU])) - (0.1 * [self pointsPerRU])) 
				 withAttributes:attr];
		
		/* Gauge rect */
		NSRect gaugeRect = NSMakeRect(storeRow2MarginX + (size.width * 0.04), 
									  [self bounds].origin.y + (size.height - (0.75 * [self pointsPerRU]) - (0.1 * [self pointsPerRU])), 
									  (size.width - storeRow2MarginX - (size.width * 0.08)), 
									  (0.24 * [self pointsPerRU]));
		[[NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.8 alpha:0.8] setStroke];
		NSBezierPath *gaugePath = [NSBezierPath bezierPathWithRect:gaugeRect];
		[gaugePath setLineWidth:0.1];
		[gaugePath stroke];
		
		/* Draw indicator rect */
		LCEntity *utilMetric = [[store4 childrenDictionary] objectForKey:@"used_pc"];
		float utilFactor = 0.00;
		if (utilMetric && [[utilMetric properties] objectForKey:@"valstr_raw"])
		{
			utilFactor = [[[utilMetric properties] objectForKey:@"valstr_raw"] floatValue] / 100.00;
			if (utilFactor > 1) utilFactor = 1.00;
		}
		NSRect indicRect = NSMakeRect (gaugeRect.origin.x, gaugeRect.origin.y, gaugeRect.size.width * utilFactor, gaugeRect.size.height);
		[[store4 opStateColor] setFill];
		NSRectFill(indicRect);
	}
}

#pragma mark Mouse Event Handling

- (void) mouseDown:(NSEvent *)event
{
	[super mouseDown:event];
}

#pragma mark "Properties"
@synthesize drawThumbnail;

@end
