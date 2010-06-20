//
//  LCInspXRaidMiniView.m
//  Lithium Console
//
//  Created by James Wilson on 17/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspXRaidMiniView.h"

#import "LCBrowser2Controller.h"
#import "LCMetric.h"

@implementation LCInspXRaidMiniView

#pragma mark "Drawing"

- (void) drawRect:(NSRect)rect
{
//	/* Border */
//	NSBezierPath *backPath = [NSBezierPath bezierPathWithRect:[self bounds]];
//	[[NSColor colorWithCalibratedWhite:0.6 alpha:1.0] setFill];
//	[backPath fill];
//	backPath = [NSBezierPath bezierPathWithRect:NSMakeRect(NSMinX([self bounds]) + 1, NSMinY([self bounds]) + 1, NSWidth([self frame])-2, NSHeight([self frame])-2)];
//	[[NSColor colorWithCalibratedWhite:0.1 alpha:1.0] setFill];
//	[backPath fill];
	
	/* Check draw mode */
	if (numUnits == 1)
	{
		[self drawRaidInRect:[self bounds]
					  device:[[raid1Arrays objectAtIndex:0] device]
					  arrays:raid1Arrays
					   image:raid1Image
					   scale:0.44];
	}
	else if (numUnits == 2)	
	{
		/* Dual-Unit Draw Mode */
		if (raid1Arrays)
		{
			[self drawRaidInRect:NSMakeRect(NSMinX([self bounds])+2, NSMinY([self bounds]),
											(NSWidth([self frame]) * 0.5) - 4, NSHeight([self frame]))
						  device:[[raid1Arrays objectAtIndex:0] device]
						  arrays:raid1Arrays
						   image:raid1Image
						   scale:0.44 * 0.5];
		}
		if (raid2Arrays)
		{
			[self drawRaidInRect:NSMakeRect(NSMidX([self bounds])+2, NSMinY([self bounds]),
											(NSWidth([self frame]) * 0.5) - 4, NSHeight([self frame]))
						  device:[[raid2Arrays objectAtIndex:0] device]
						  arrays:raid2Arrays
						   image:raid2Image
						   scale:0.44 * 0.5];
		}
	}
		
}

- (void) drawRaidInRect:(NSRect)rect device:(LCEntity *)device arrays:(NSArray *)arrays image:(NSImage *)raidImage scale:(float)scaleRatio
{
	/* 
	 * Draws scaled RAID unit and name in rect 
	 */

	/* RAID Name */
	NSString *raidName = [device displayString];
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.98 green:0.98 blue:0.98 alpha:0.6], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Lucida Grande Bold" size:9.0], NSFontAttributeName,
		nil];
	NSSize nameSize = [raidName sizeWithAttributes:attr];
	NSRect nameRect = NSMakeRect(NSMidX(rect) - (nameSize.width * 0.5),
								 NSMinY(rect) + 3,
								 nameSize.width,nameSize.height);
	[raidName drawInRect:nameRect withAttributes:attr];
	
	/* Status Dot */
	NSImage *image;
	switch ([[device opstateInteger] intValue])
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
	
#pragma mark "Image Update"

- (NSRect) driveBay1Rect
{
	return NSMakeRect(25, 34, 268, 124);
}

- (NSRect) driveBay2Rect
{
	return NSMakeRect(620.0 - 268.0 - 25.0, 34, 268, 124);
}

- (NSRect) rectForDrive:(int)num
{
	NSRect bayRect;
	if (num < 8)
	{
		/* Drives 1-7 */
		bayRect = [self driveBay1Rect];
	}
	else
	{
		/* Drives 7-14 */
		bayRect = [self driveBay2Rect];
		num -= 7;
	}
	NSRect driveRect = NSMakeRect(NSMinX(bayRect)+((num-1)*(NSWidth(bayRect)/7.0)), NSMinY(bayRect),
								  (NSWidth(bayRect)/7.0), NSHeight(bayRect));
	return driveRect;
}

- (void) updateImages
{
	int i;
	for (i=0; i < numUnits; i++)
	{
		NSImage *raidImage = [[NSImage alloc] initWithSize:NSMakeSize(620, 194)];
		NSView *raidView = nil;
		NSArray *raidArrays = nil;
		if (i==0 && raid1Arrays) 
		{
			[raid1Image release];
			raid1Image = raidImage; 
			raidView = raid1View;
			raidArrays = raid1Arrays;
		}
		else if (i==1 && raid2Arrays)
		{
			[raid2Image release];
			raid2Image = raidImage; 
			raidView = raid2View;
			raidArrays = raid2Arrays;
		}
		
		if (raidView)
		{
			/* Take snapshot of view */
//			NSRect r = [raidView bounds];
			NSImage *viewImage = [[NSImage alloc] initWithSize:[raidView bounds].size];
			[viewImage lockFocus];
			[raidView drawRect:[raidView bounds]];
			[viewImage unlockFocus];
			NSEnumerator *subviewEnum = [[raidView subviews] objectEnumerator];
			NSView *subview;
			while (subview=[subviewEnum nextObject])
			{
				NSImage *subviewImage = [[NSImage alloc] initWithSize:[subview bounds].size];
				[subviewImage lockFocus];
				[subview drawRect:[subview bounds]];
				[subviewImage unlockFocus];
				[viewImage lockFocus];
				[subviewImage drawInRect:[subview frame]
								fromRect:[subview bounds]
							   operation:NSCompositeSourceOver
								fraction:1.0];
				[viewImage unlockFocus];
				[subviewImage autorelease];
			}
			
			/* Draw the snapshot in the raidImage */
			[raidImage lockFocus];
			[viewImage drawInRect:NSMakeRect(0,0,[raidImage size].width, [raidImage size].height) 
						 fromRect:NSMakeRect(0,0,[viewImage size].width, [viewImage size].height) 
						operation:NSCompositeSourceOver
						 fraction:1.0];	
			[viewImage autorelease];
			
			/*
			 * Select/Hide Drive Units
			 */
			
			/* Create status array */
			NSMutableArray *statusArray = [NSMutableArray array];
			int i;
			for (i=0; i < 14; i++)
			{ [statusArray addObject:[NSNumber numberWithInt:0]]; }
			
			/* Loop through arrays */
			NSEnumerator *arrayEnum = [raidArrays objectEnumerator];
			LCEntity *arrayObj;
			while (arrayObj = [arrayEnum nextObject])
			{
				int x;
				for (x=1; x < 8; x++)
				{
					/* Loop through each "member" */
					NSString *metricName = [NSString stringWithFormat:@"member%i_slot", x];
					LCMetric *metric = [[arrayObj childrenDictionary] objectForKey:metricName];
					NSString *value = [[metric currentValue] rawValueString];
					if (metric && value && [value intValue] < 15 && [value intValue] > 0)
					{
						if ([metadataArrays containsObject:arrayObj])
						{
							[statusArray replaceObjectAtIndex:([value intValue] - 1)
												   withObject:[NSNumber numberWithInt:2]];
						}
						else
						{
							[statusArray replaceObjectAtIndex:([value intValue] - 1)
												   withObject:[NSNumber numberWithInt:1]];
						}
					}
				}
			}
			
			/* Loop through drives */
			int z;
			for (z=0; z < 14; z++)
			{
				NSNumber *status = [statusArray objectAtIndex:z];
				if ([status intValue] == 0)
				{
					/* Blank-out */
					[[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.3] setFill];
				}
				else if ([status intValue] == 1)
				{
					/* Highlight */
					[[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:1.0 alpha:0.3] setFill];
				}
				else if ([status intValue] == 2)
				{
					/* Highlight as metadata */
					[[NSColor colorWithCalibratedRed:1.0 green:0.0 blue:1.0 alpha:0.3] setFill];
				}
				NSBezierPath *drivePath = [NSBezierPath bezierPathWithRect:[self rectForDrive:z+1]];
				[drivePath fill];
			}
			
			/* Unlock */
			[raidImage unlockFocus];
		}
		else
		{ [raidImage release]; }
	}
	
	[self setNeedsDisplay:YES];
}

#pragma mark "Array Management"

@synthesize numUnits;

@synthesize raid1Arrays;
- (void) setRaid1Arrays:(NSArray *)array
{ 
	[raid1Arrays release];
	raid1Arrays = [array retain]; 
	if ([raid1Arrays count] > 0)
	{
		raid1View = [[LCXRDeviceView alloc] initWithDevice:[[raid1Arrays objectAtIndex:0] device] inFrame:NSMakeRect(0,0,620,194)];
	}
}

@synthesize raid2Arrays;
- (void) setRaid2Arrays:(NSArray *)array
{ 
	[raid2Arrays release];
	raid2Arrays = [array retain]; 
	if ([raid2Arrays count] > 0)
	{
		raid2View = [[LCXRDeviceView alloc] initWithDevice:[[raid2Arrays objectAtIndex:0] device] inFrame:NSMakeRect(0,0,620,194)];
	}
}

@synthesize metadataArrays;

#pragma mark "Mouse Handling"

- (void) mouseDown:(NSEvent *)event
{
	if ([event clickCount] != 2) return;
	
//	LCEntity *clickTarget = nil;
//	if (numUnits == 1 && [raid1Arrays count] > 0)
//	{
//		/* Single-unit */
//		clickTarget = [[raid1Arrays objectAtIndex:0] device];
//	}
//	else
//	{
//		NSPoint localPoint = [self convertPoint:[event locationInWindow] fromView:nil];
//		if (localPoint.x < NSMidX([self bounds]) && [raid1Arrays count] > 0)
//		{
//			/* Unit 1  Clicked */
//			clickTarget = [[raid1Arrays objectAtIndex:0] device];
//		}
//		else if ([raid2Arrays count] > 0)
//		{
//			/* Unit 2 Clicked */ 
//			clickTarget = [[raid2Arrays objectAtIndex:0] device];
//		}
//	}
	
}

#pragma mark "Controller"

@synthesize controller;

@end
