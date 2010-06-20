//
//  LCXSCPUView.m
//  Lithium Console
//
//  Created by James Wilson on 4/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXSCPUView.h"


@implementation LCXSCPUView

#pragma mark "Constructors"

- (id) initWithCPU:(LCObject *)initCPU inFrame:(NSRect)frame 
{
    [super initWithFrame:frame];
	
	cpuObject = [initCPU retain];
	cpuImage = [[NSImage imageNamed:@"xscpu.png"] retain];
	
    return self;
}

- (void) dealloc
{
	[cpuObject release];
	[cpuImage release];
	[super dealloc];
}

#pragma mark "View Management"

- (void) removeFromSuperview
{
	[super removeFromSuperview];
}

#pragma mark "Drawing"

- (void) drawRect:(NSRect)rect
{
	/* Draw drive image */
	[cpuImage drawInRect:NSMakeRect(0,0,NSWidth([self bounds]),NSHeight([self bounds]))
				fromRect:NSMakeRect(0,0,[cpuImage size].width,[cpuImage size].height)
			   operation:NSCompositeSourceOver
				fraction:1.0];

	/* Status Dot */
	NSRect statusRect = NSMakeRect(4, NSMaxY([self bounds])-10, 6, 6);
	NSBezierPath *statusPath = [NSBezierPath bezierPathWithOvalInRect:statusRect];
	[[cpuObject opStateColor] setFill];
	[statusPath fill];
	[[NSColor grayColor] setStroke];
	[statusPath stroke];
	
	/* Default text attributes */
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.7], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Bank Gothic Light BT" size:9.0], NSFontAttributeName,
		nil];
	
	/* Desc */
	NSString *description = [cpuObject desc];
	[description drawAtPoint:NSMakePoint(12,NSMaxY([self bounds])-12) withAttributes:attr];
	
	/* Utilisation bar */
	NSString *str = @"Utilisation";
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.2 green:0.2 blue:0.2 alpha:1.0], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Bank Gothic Light BT" size:9.0], NSFontAttributeName,
		nil];
	[str drawAtPoint:NSMakePoint(8, NSMaxY([self bounds])-40) withAttributes:attr];
	NSRect utilRect = NSMakeRect(8, NSMaxY([self bounds])-55, NSWidth([self bounds])-16, 12);
	[[NSColor colorWithDeviceRed:0.1 green:0.1 blue:0.1 alpha:1.0] setFill];
	NSRectFill(utilRect);
	
	if ([[[cpuObject container] name] isEqualToString:@"xsicpu"])
	{
		float value1 = [[cpuObject rawValueForMetricNamed:@"usage_1"] floatValue];
		NSRect gaugeRect1 = NSMakeRect (NSMinX(utilRect)+2, NSMinY(utilRect)+7, NSWidth(utilRect)-4, 3);
		[self fillGaugeRect:gaugeRect1 amount:value1];
		float value2 = [[cpuObject rawValueForMetricNamed:@"usage_2"] floatValue];
		NSRect gaugeRect2 = NSMakeRect (NSMinX(utilRect)+2, NSMinY(utilRect)+2, NSWidth(utilRect)-4, 3);
		[self fillGaugeRect:gaugeRect2 amount:value2];
	}
	else
	{
		float value = [[cpuObject rawValueForMetricNamed:@"usage"] floatValue];
		NSRect gaugeRect = NSMakeRect (NSMinX(utilRect)+2, NSMinY(utilRect)+2, NSWidth(utilRect)-4, NSHeight(utilRect)-4);
		[self fillGaugeRect:gaugeRect amount:value];
	}
	
	/* Temperature Gauges */
	str = @"Temperature";
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.2 green:0.2 blue:0.2 alpha:1.0], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Bank Gothic Light BT" size:9.0], NSFontAttributeName,
		//		[NSFont fontWithName:@"Arial" size:9.0], NSFontAttributeName,
		nil];
	[str drawAtPoint:NSMakePoint(8, NSMaxY([self bounds])-65) withAttributes:attr];
	utilRect = NSMakeRect(8, NSMaxY([self bounds])-77, NSWidth([self bounds])-16, 12);
	[[NSColor colorWithDeviceRed:0.1 green:0.1 blue:0.1 alpha:1.0] setFill];
	NSRectFill(utilRect);

	if ([[[cpuObject container] name] isEqualToString:@"xsicpu"])
	{
		/* Inlet Temp */
		NSRect gaugeRect = NSMakeRect (NSMinX(utilRect)+2, NSMinY(utilRect)+7, NSWidth(utilRect)-4, 3);
		float value = [[cpuObject rawValueForMetricNamed:@"temp_inlet"] floatValue];
		[self fillGaugeRect:gaugeRect amount:(value / 80.0) * 100.0];
		
		/* Heatsink Temp */
		gaugeRect = NSMakeRect (NSMinX(utilRect)+2, NSMinY(utilRect)+2, NSWidth(utilRect)-4, 3);
		value = [[cpuObject rawValueForMetricNamed:@"temp_heatsink"] floatValue];
		[self fillGaugeRect:gaugeRect amount:(value / 80.0) * 100.0];
	}
	else
	{
		/* Ambient Temp */
		NSRect gaugeRect = NSMakeRect (NSMinX(utilRect)+2, NSMinY(utilRect)+7, NSWidth(utilRect)-4, 3);
		float value = [[cpuObject rawValueForMetricNamed:@"temp_ambient"] floatValue];
		[self fillGaugeRect:gaugeRect amount:(value / 80.0) * 100.0];
		
		/* Internal Temp */
		gaugeRect = NSMakeRect (NSMinX(utilRect)+2, NSMinY(utilRect)+2, NSWidth(utilRect)-4, 3);
		value = [[cpuObject rawValueForMetricNamed:@"temp_internal"] floatValue];
		[self fillGaugeRect:gaugeRect amount:(value / 80.0) * 100.0];
	}
	
	
	/* Power Gauges */
	str = @"Power";
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.2 green:0.2 blue:0.2 alpha:1.0], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Bank Gothic Light BT" size:9.0], NSFontAttributeName,
		//		[NSFont fontWithName:@"Arial" size:9.0], NSFontAttributeName,
		nil];
	[str drawAtPoint:NSMakePoint(8, NSMaxY([self bounds])-87) withAttributes:attr];
	utilRect = NSMakeRect(8, NSMaxY([self bounds])-99, NSWidth([self bounds])-16, 12);
	[[NSColor colorWithDeviceRed:0.1 green:0.1 blue:0.1 alpha:1.0] setFill];
	NSRectFill(utilRect);

	/* Watts */
	NSRect gaugeRect = NSMakeRect (NSMinX(utilRect)+2, NSMinY(utilRect)+2, NSWidth(utilRect)-4, 3);
	float value = [[cpuObject rawValueForMetricNamed:@"power_watts"] floatValue];
	[self fillGaugeRect:gaugeRect amount:(value / 150.0) * 100.0];
	
	/* Vcore */
	gaugeRect = NSMakeRect (NSMinX(utilRect)+2, NSMinY(utilRect)+7, NSWidth(utilRect)-4, 3);
	value = [[cpuObject rawValueForMetricNamed:@"power_vcore"] floatValue];
	[self fillGaugeRect:gaugeRect amount:(value / 2.3) * 100.0];
	
}

- (void) fillGaugeRect:(NSRect)rect amount:(float)amount
{
	int i;
	int barcount = 64;
	float q = NSWidth(rect) / 64.0;
	for (i=0; i < barcount; i++)
	{
		NSRect barRect = NSMakeRect(NSMinX(rect) + (i * q), NSMinY(rect), q * 0.8, NSHeight(rect));
		if (i % 2 == 0)
		{
			if ((((float) i / (float) barcount) * 100.0) <= amount)
			{
				[[NSColor colorWithDeviceRed:119/256.0 green:231/256.0 blue:255/256.0 alpha:1.0] setFill];
				NSRectFill(barRect);
			}
			else
			{
				[[NSColor colorWithDeviceRed:37/256.0 green:48/256.0 blue:55/256.0 alpha:0.7] setFill];
				NSRectFill(barRect);
			}
		}
	}
}

@end
