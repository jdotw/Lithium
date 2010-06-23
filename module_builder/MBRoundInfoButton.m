//
//  MBRoundInfoButton.m
//  ModuleBuilder
//
//  Created by James Wilson on 8/03/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MBRoundInfoButton.h"

#import "MBEntity.h"

@implementation MBRoundInfoButton

- (void) drawRect:(NSRect)rect
{
	MBEntity *entity = (MBEntity *) [viewController representedObject];

	/* Draw Background */
	NSBezierPath *backPath = [NSBezierPath bezierPathWithOvalInRect:self.bounds];
	if ([entity infoViewVisible] == 0)
	{ [[NSColor colorWithCalibratedWhite:0.0 alpha:0.1] setFill]; }
	else
	{ [[NSColor colorWithCalibratedRed:84/256.0 green:132/256.0 blue:221.0/256.0 alpha:0.3] setFill]; }	
	[backPath fill];
	
	/* Draw Clicked over-lay */
	if (mouseIsDown)
	{
		NSBezierPath *mouseDownPath = [NSBezierPath bezierPathWithOvalInRect:self.bounds];
		[[NSColor colorWithCalibratedWhite:0.0 alpha:0.1] setFill];
		[mouseDownPath fill];
	}		
	
	/* Draw Arrow */
	NSRect arrowFrame = NSMakeRect(4.0, 4.0, self.bounds.size.width - 8.0, self.bounds.size.height - 8.0);
	NSBezierPath *path = [NSBezierPath bezierPath];
	if ([entity infoViewVisible])
	{
		/* Up */
		[path moveToPoint:NSMakePoint(NSMinX(arrowFrame),NSMaxY(arrowFrame))];
		[path lineToPoint:NSMakePoint(NSMaxX(arrowFrame),NSMaxY(arrowFrame))];
		[path lineToPoint:NSMakePoint(NSMinX(arrowFrame)+(arrowFrame.size.width/2.0), NSMinY(arrowFrame))];
		[path lineToPoint:NSMakePoint(NSMinX(arrowFrame),NSMaxY(arrowFrame))];	
	}
	else
	{
		/* Down */
		[path moveToPoint:NSMakePoint(NSMinX(arrowFrame),NSMinY(arrowFrame))];
		[path lineToPoint:NSMakePoint(NSMaxX(arrowFrame),NSMinY(arrowFrame))];
		[path lineToPoint:NSMakePoint(NSMinX(arrowFrame)+(arrowFrame.size.width/2.0), NSMaxY(arrowFrame))];
		[path lineToPoint:NSMakePoint(NSMinX(arrowFrame),NSMinY(arrowFrame))];	
	}
	[[NSColor colorWithCalibratedWhite:0.0 alpha:0.6] setFill];
	[path fill];
}

@end
