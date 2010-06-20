//
//  LCDeviceView.m
//  Lithium Console
//
//  Created by James Wilson on 23/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCDeviceView.h"


@implementation LCDeviceView

- (BOOL) drawWarnings
{
	/* 
	 * This is always called AFTER the subclass has done its drawing 
	 */
	
	NSString *overlayMessage = nil;
	
	/* Check for license */
	if (![device isLicensed])
	{
		overlayMessage = @"Device is not licensed...";
	}
	
	/* Check for process */
	else if (![device resourceStarted])
	{
		overlayMessage = @"Monitoring process starting up...";
	}

	/* Check for recent restart */
	else if ([device recentlyRestarted])
	{
		overlayMessage = @"Gathering initial monitoring data...";
	}	
	
	/* Draw overlay */
	if (overlayMessage)
	{
		[NSGraphicsContext saveGraphicsState];
		NSShadow* theShadow = [[NSShadow alloc] init]; 
		[theShadow setShadowOffset:NSMakeSize(3.0, -3.0)]; 
		[theShadow setShadowBlurRadius:5.0];
		[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.45]];
		[theShadow set];			
		NSImage *box = [NSImage imageNamed:@"devwarningbox.png"];
		NSRect boxRect = NSMakeRect(NSMidX([self bounds]) - ([box size].width * 0.5),
									NSMidY([self bounds]) - ([box size].height * 0.5) +5,
									[box size].width, [box size].height);
		[box drawInRect:boxRect
			   fromRect:NSMakeRect(0,0,[box size].width, [box size].height)
			  operation:NSCompositeSourceOver
			   fraction:1.0];
		
		[NSGraphicsContext restoreGraphicsState];
		[theShadow release];

		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithCalibratedRed:0.98 green:0.98 blue:0.98 alpha:1.0], NSForegroundColorAttributeName,
			[NSFont fontWithName:@"Lucida Grande" size:16.0], NSFontAttributeName,
			nil];
		NSSize textSize = [overlayMessage sizeWithAttributes:attr];
		NSRect textRect = NSMakeRect(NSMidX([self bounds]) - (textSize.width * 0.5), 
									 NSMidY([self bounds]) - (textSize.height * 0.5) + 5,
									 textSize.width, textSize.height);
		[overlayMessage drawInRect:textRect withAttributes:attr];		
		
		return YES;
	}
	else
	{ return NO; }
}

- (BOOL) hasWarnings
{
	if (![device isLicensed]) return YES;
	else if (![device resourceStarted]) return YES;
	else if ([device recentlyRestarted]) return YES;
	else return NO;
}

- (void) dealloc
{ 
	[device release];
	[super dealloc];
}

@synthesize device;

@end
