//
//  LCInspXRaidLUNMiniView.m
//  Lithium Console
//
//  Created by James Wilson on 18/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspXRaidLUNMiniView.h"
#import "LCBrowser2Controller.h"
#import "LCMetric.h"

@implementation LCInspXRaidLUNMiniView

- (void) drawRect:(NSRect)rect
{
	/* Border */
	NSBezierPath *backPath = [NSBezierPath bezierPathWithRect:[self bounds]];
	[[NSColor colorWithCalibratedWhite:0.6 alpha:1.0] setFill];
	[backPath fill];
	backPath = [NSBezierPath bezierPathWithRect:NSMakeRect(NSMinX([self bounds]) + 1, NSMinY([self bounds]) + 1, NSWidth([self frame])-2, NSHeight([self frame])-2)];
	[[NSColor colorWithCalibratedWhite:0.1 alpha:1.0] setFill];
	[backPath fill];
	
	/* Use super-class to draw RAID */
	[super drawRaidInRect:NSMakeRect(NSMinX([self bounds])+18, NSMinY([self bounds]),
									 (NSWidth([self frame]) * 0.5) - 4, NSHeight([self frame]))
				  device:[[raid1Arrays objectAtIndex:0] device]
				  arrays:raid1Arrays
				   image:raid1Image
				   scale:0.44 * 0.5];
//	NSRect raidRect = NSMakeRect(NSMinX([self bounds])+2, NSMinY([self bounds]),
//								 (NSWidth([self frame]) * 0.5) - 4, NSHeight([self frame]));
	
	/*
	 * Draw StoragePool / LUN Usage 
	 */

	/* Define spRect */
	NSRect spRect = NSMakeRect(NSMidX([self bounds])+2+(NSWidth([self frame])*0.1), NSMinY([self bounds]),
							   (NSWidth([self frame]) * 0.4) - 4, NSHeight([self frame]));
	
	/* Draw Name */
	LCEntity *sp = nil;
	if ([[lun uses] count] > 0) sp = [[lun uses] objectAtIndex:0];
	if (sp)
	{	
		/* SP Name */
		NSString *spName = [sp displayString];
		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithCalibratedRed:0.98 green:0.98 blue:0.98 alpha:0.6], NSForegroundColorAttributeName,
			[NSFont fontWithName:@"Lucida Grande Bold" size:9.0], NSFontAttributeName,
			nil];
		NSSize nameSize = [spName sizeWithAttributes:attr];
		NSRect nameRect = NSMakeRect(NSMidX(spRect) - (nameSize.width * 0.5), NSMinY(spRect) + 3,  
									 nameSize.width,nameSize.height);
		[spName drawInRect:nameRect withAttributes:attr];

		/* Status Dot */
		NSImage *image;
		switch ([[sp opstateInteger] intValue])
		{
			case 0:
				image = [NSImage imageNamed:@"GreenDot.tiff"];
				break;
			case 1:
				image = [NSImage imageNamed:@"YellowDot.tiff"];
				break;
			case 2:
				image = [NSImage imageNamed:@"YellowDot.tiff"];
				break;
			case 3:
				image = [NSImage imageNamed:@"RedDot.tiff"];
				break;
			default:
				image = [NSImage imageNamed:@"GreyDot.tiff"];	
		}
		NSRect dotRect = NSMakeRect(NSMinX(nameRect)-13,NSMinY(nameRect)-1,12,12);
		[image drawInRect:dotRect
				 fromRect:NSMakeRect(0,0,[image size].width,[image size].height)
				operation:NSCompositeSourceOver
				 fraction:0.9];
		
		/* Draw Disk Icon */
		NSRect outlineRect = NSMakeRect(NSMidX(spRect)-25, NSMinY(spRect) + nameSize.height + 5,
									 40,32);
		NSImage *diskImage = [NSImage imageNamed:@"diskstack_40.tif"];
		[diskImage drawInRect:outlineRect
					 fromRect:NSMakeRect(0,0,40,32)
					operation:NSCompositeSourceOver
					 fraction:1.0];
		
		/* Draw Outline */
//		NSRect outlineRect = NSMakeRect(NSMidX(spRect) - 45, NSMinY(spRect) + nameSize.height + 5, 
//										80, 33);
//		NSBezierPath *outlinePath = [NSBezierPath bezierPathWithRect:outlineRect];
//		[[NSColor colorWithCalibratedWhite:0.2 alpha:1.0] setFill];
//		[[NSColor colorWithCalibratedWhite:0.8 alpha:1.0] setStroke];
//		[outlinePath fill];
//		[outlinePath stroke];
		
		/* Draw Pool Usage */
		LCMetric *usedMetric = (LCMetric *) [sp childNamed:@"used_pc"];
		if (usedMetric)
		{
			float level;
			if ([usedMetric currentValue]) level = [[[usedMetric currentValue] rawValueString] floatValue];
			else level = 0.0;
			
			NSRect gaugeRect = NSMakeRect(NSMinX(outlineRect)+2,NSMinY(outlineRect)+1,NSWidth(outlineRect)-2,NSHeight(outlineRect)-2);
			NSRect gaugeFill = gaugeRect;
			gaugeFill.size.height = gaugeRect.size.height * (level / 100.0);
			
			/* Draw basic fill path */
			NSBezierPath *fillPath = [NSBezierPath bezierPath];
			[fillPath moveToPoint:NSMakePoint(NSMinX(gaugeFill),NSMaxY(gaugeFill))];
			[fillPath lineToPoint:NSMakePoint(NSMinX(gaugeFill),NSMinY(gaugeFill))];
			[fillPath lineToPoint:NSMakePoint(NSMaxX(gaugeFill),NSMinY(gaugeFill))];
			[fillPath lineToPoint:NSMakePoint(NSMaxX(gaugeFill),NSMaxY(gaugeFill))];	
			
			/* Draw wave top */
			int crestCount = 6;
			int i;
			float waveLength = NSWidth(gaugeFill) / (float) crestCount;
			float curX = NSMaxX(gaugeFill);
			for (i=0; i < crestCount; i++)
			{
				[fillPath curveToPoint:NSMakePoint(curX - waveLength,NSMaxY(gaugeFill))
						 controlPoint1:NSMakePoint(curX - (0.2 * waveLength), NSMaxY(gaugeFill) - (0.3 * waveLength))
						 controlPoint2:NSMakePoint(curX - (0.8 * waveLength), NSMaxY(gaugeFill) - (0.3 * waveLength))];
				curX = curX - waveLength;
			}
			
			/* Fill */
			[[NSColor colorWithCalibratedRed:72.0/256.0 green:150.0/256.0 blue:176.0/256.0 alpha:0.4] setFill];
			[fillPath fill];
		}
		
		/* Draw Connector */
//		NSRect triangle
//		NSBezierPath *connPath = [NSBezierPath bezierPath];
//		[connPath moveToPoint:NSMakePoint(NSMaxX(raidRect)-1,NSMidY(raidRect)+(0.1 * NSHeight(raidRect)))];
//		[connPath curveToPoint:NSMakePoint(NSMinX(outlineRect),NSMidY(outlineRect)-(0.25 * NSHeight(outlineRect)))
//				   controlPoint1:NSMakePoint(NSMaxX(raidRect)+10, NSMinY(raidRect))
//				   controlPoint2:NSMakePoint(NSMinX(outlineRect)-10,NSMaxY(outlineRect))];
//		[connPath lineToPoint:NSMakePoint(NSMinX(outlineRect),NSMidY(raidRect)+(0.1 * NSHeight(raidRect)))];
//		[[NSColor colorWithCalibratedRed:72.0/256.0 green:150.0/256.0 blue:176.0/256.0 alpha:1.0] setStroke];
//		[[NSColor colorWithCalibratedRed:100.0/256.0 green:150.0/256.0 blue:256.0/256.0 alpha:0.5] setStroke];
//		[connPath setLineWidth:2.0];
//		[connPath stroke];
	}
}

- (void) drawRaidInRect:(NSRect)rect device:(LCEntity *)device arrays:(NSArray *)arrays image:(NSImage *)raidImage scale:(float)scaleRatio
{
	/* 
	* Draws scaled RAID unit and name in rect 
	 */
	
	LCEntity *array = nil;
	if ([arrays count] > 0) array = [arrays objectAtIndex:0];
	
	/* RAID Name */
	NSString *raidName = [[array parent] displayString];
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.98 green:0.98 blue:0.98 alpha:0.6], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Lucida Grande Bold" size:9.0], NSFontAttributeName,
		nil];
	NSSize nameSize = raidName ? [raidName sizeWithAttributes:attr] : NSZeroSize;
	NSRect nameRect = NSMakeRect(NSMidX(rect) - (nameSize.width * 0.5),
								 NSMinY(rect) + 3,
								 nameSize.width,nameSize.height);
	[raidName drawInRect:nameRect withAttributes:attr];
	
	/* Status Dot */
	NSImage *image;
	switch ([[[array parent] opstateInteger] intValue])
	{
		case 0:
			image = [NSImage imageNamed:@"GreenDot.tiff"];
			break;
		case 1:
			image = [NSImage imageNamed:@"YellowDot.tiff"];
			break;
		case 2:
			image = [NSImage imageNamed:@"YellowDot.tiff"];
			break;
		case 3:
			image = [NSImage imageNamed:@"RedDot.tiff"];
			break;
		default:
			image = [NSImage imageNamed:@"GreyDot.tiff"];	
	}
	NSRect dotRect = NSMakeRect(NSMinX(nameRect)-13,NSMinY(nameRect)-1,12,12);
	[image drawInRect:dotRect
			 fromRect:NSMakeRect(0,0,[image size].width,[image size].height)
			operation:NSCompositeSourceOver
			 fraction:0.9];	
	
	/* Setup graphics context */
	[NSGraphicsContext saveGraphicsState];
	[[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationHigh];
	[[NSGraphicsContext currentContext] setShouldAntialias:YES];
	
	/* Draw RAID */
	NSRect raidRect = NSMakeRect(NSMidX(rect) - (([raidImage size].width * scaleRatio) * 0.5), 
								 NSMinY(rect) + nameSize.height - 1, 
								 [raidImage size].width * scaleRatio,[raidImage size].height * scaleRatio);
	[raidImage drawInRect:raidRect
				 fromRect:NSMakeRect(0,0,[raidImage size].width,[raidImage size].height)
				operation:NSCompositeSourceOver
				 fraction:1.0];
	
	/* Restore GC */
	[NSGraphicsContext restoreGraphicsState];	
}

- (NSBezierPath *) triangleInFrame:(NSRect)frame
{
	NSBezierPath *path = [NSBezierPath bezierPath];
	[path moveToPoint:NSMakePoint(NSMinX(frame),NSMinY(frame))];
	[path lineToPoint:NSMakePoint(NSMaxX(frame),NSMinY(frame))];
	[path lineToPoint:NSMakePoint(NSMinX(frame)+(frame.size.width/2.0), NSMaxY(frame))];
	[path lineToPoint:NSMakePoint(NSMinX(frame),NSMinY(frame))];
	return path;
}

@synthesize lun;

#pragma mark "Mouse Handling"

- (void) mouseDown:(NSEvent *)event
{
	if ([event clickCount] != 2) return;
	
	LCEntity *clickTarget = nil;
	NSPoint localPoint = [self convertPoint:[event locationInWindow] fromView:nil];
	if (localPoint.x < NSMidX([self bounds]) && [raid1Arrays count] > 0)
	{
		/* Unit 1  Clicked */
		clickTarget = [[raid1Arrays objectAtIndex:0] device];
	}
	else if ([[lun uses] count] > 0)
	{
		/* SP Clicked */ 
		clickTarget = [[lun uses] objectAtIndex:0];
	}
	
	/* Open browser */
	if (clickTarget)
	{ 
		[[[LCBrowser2Controller alloc] initWithEntity:clickTarget] autorelease];
	}
}

@end
