//
//  MBConsoleMetricWell.m
//  ModuleBuilder
//
//  Created by James Wilson on 17/08/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MBConsoleMetricWell.h"

#import "LCRoundedBezierPath.h"
#import "MBContainer.h"

@implementation MBConsoleMetricWell

#pragma mark "Drawing"

- (void) drawRect:(NSRect)rect
{
	NSImage *image;
	if (drawHighlight)
	{ image = [NSImage imageNamed:@"MetricWellBackHighlight.png"]; }
	else
	{ image = [NSImage imageNamed:@"MetricWellBack.png"]; }		
	[image drawInRect:[self bounds]
			 fromRect:NSMakeRect(0, 0, [image size].width, [image size].height)
			operation:NSCompositeSourceOver fraction:1.0];
}

#pragma mark "Layout Methods"

- (void) resetUsing:(MBContainer *)container forPosition:(int)index
{
	/* Reset view using this container for the given position */
	[self setMetricDesc:[[container consoleViewMetrics] objectForKey:[NSString stringWithFormat:@"%i", index]]];
}

#pragma mark "Metric Accessors"

- (NSString *) metricDesc
{ return metricDesc; }

- (void) setMetricDesc:(NSString *)value
{ 
	[metricDesc release];
	metricDesc = [value retain];
	if (metricDesc)
	{
		[label setStringValue:metricDesc];
		[label setTextColor:[NSColor colorWithCalibratedWhite:1.0 alpha:1.0]];
		[dotView setImage:[NSImage imageNamed:@"BlueDot.tiff"]];
	}
	else
	{ 
		[label setStringValue:@"Drag Metric Here"];
		[dotView setImage:[NSImage imageNamed:@"GreyDot.tiff"]];
		[label setTextColor:[NSColor colorWithCalibratedWhite:0.6 alpha:1.0]];		
	}	
}

#pragma mark "Drag and Drop"

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)info 
{
	[self setDrawHightlight:YES];
	return NSDragOperationCopy;
}

- (void)draggingExited:(id <NSDraggingInfo>)sender
{
	[self setDrawHightlight:NO];
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)info
{
    NSPasteboard *pboard;
    pboard = [info draggingPasteboard];
	
	/* Get Entities */
	NSArray *metricPropertiesArray = [[info draggingPasteboard] propertyListForType:@"MBMetric"];
	NSEnumerator *enumerator = [metricPropertiesArray objectEnumerator];
	NSDictionary *metricProperties;
	while (metricProperties = [enumerator nextObject])
	{
		/* Set local */
		[self setMetricDesc:[metricProperties objectForKey:@"desc"]];
	}
	
	[self setDrawHightlight:NO];
	[self setNeedsDisplay:YES];
	
    return YES;
}

#pragma mark "UI Accessors"

- (IBAction) clearMetricClicked:(id)sender
{
	[self setMetricDesc:nil];
}

#pragma mark "General Accessors"

- (BOOL) drawHighlight
{ return drawHighlight; }

- (void) setDrawHightlight:(BOOL)value
{ 
	drawHighlight = value; 
	[self setNeedsDisplay:YES];
}

@end
