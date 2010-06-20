//
//  LCXRDriveView.m
//  Lithium Console
//
//  Created by James Wilson on 27/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXRDriveView.h"
#import "LCRoundedBezierPath.h"
#import "LCEntityViewController.h"
#import "LCObjectView.h"
#import "LCXRDeviceView.h"

@implementation LCXRDriveView

#pragma mark "Constructors"

- (id)initWithDrive:(LCObject *)initDrive slot:(int)index inFrame:(NSRect)frame 
{
    [super initWithFrame:frame];

	driveObject = [initDrive retain];
	slot = index;
	arrayRect = NSZeroRect;
	trackingRectEnabled = YES;

	driveBackImage = [[NSImage imageNamed:@"xsrdriveback.png"] retain];
	driveBlankImage = [[NSImage imageNamed:@"xsrblankdrive.png"] retain];
	arrayWindowImage = [[NSImage imageNamed:@"xsrarraywindow.png"] retain];	
	popupImage = [[NSImage imageNamed:@"xsrpopupback.png"] retain];
    return self;
}

- (void) dealloc
{
	[raiseTimer invalidate];
	[lowerTimer invalidate];
	[driveObject release];
	[super dealloc];
}

#pragma mark "View Management"

- (void) removeFromSuperview
{
	[self removeTrackingRect:trackTag];
//	[driveViewController removeViewAndContent];
//	[driveViewController autorelease];
//	driveViewController = nil;
//	[arrayViewController removeViewAndContent];
//	[arrayViewController autorelease];
//	arrayViewController = nil;
	[super removeFromSuperview];
}

//- (BOOL) isOpaque
//{ return YES; }

#pragma mark "Geometry"

- (NSRect) driveRect
{
	if (slot < 7)
	{
		if (isRaised)
		{ return NSMakeRect (NSMinX([self bounds])+10, NSMinY([self bounds])+10, [driveBlankImage size].width, NSHeight([self bounds])-20); }
		else
		{ return [self bounds]; }	
	}
	else
	{
		if (isRaised)
		{ return NSMakeRect (NSMaxX([self bounds])-10-[driveBlankImage size].width, NSMinY([self bounds])+10, [driveBlankImage size].width, NSHeight([self bounds])-20); }
		else
		{ return [self bounds]; }			
	}
}

- (NSRect) popoutRect
{
	if (slot < 7)
	{
		if (isRaised)
		{ return NSMakeRect (NSMinX([self driveRect]), NSMinY([self driveRect]), NSWidth([self bounds])-NSWidth([self driveRect]), NSHeight([self driveRect])); }
		else
		{ return NSZeroRect; }	
	}
	else
	{
		if (isRaised)
		{ return NSMakeRect (NSMinX([self bounds])+10, NSMinY([self driveRect]), NSWidth([self bounds])-NSWidth([self driveRect]), NSHeight([self driveRect])); }
		else
		{ return NSZeroRect; }	
	}
	
}

#pragma mark "Drawing"

- (void)drawRect:(NSRect)rect 
{
	/* Accept mouse moves */
	[[self window] setAcceptsMouseMovedEvents:YES];

	/* Base coordinates */
	NSRect driveRect = [self driveRect];
	NSRect popoutRect = [self popoutRect];
	
	/* Add tracking rect */
	[self removeTrackingRect:trackTag];
	if (trackingRectEnabled)
	{ 
		trackTag = [self addTrackingRect:[self bounds] owner:self userData:nil assumeInside:NO]; 
	}
	
	/* 
	 * Draw Drive 
	 */
	
	NSImage *backImage;
	BOOL slotIsPopulated;
	if (!driveObject || ![driveObject valueForMetricNamed:@"smart_state"] || [[driveObject valueForMetricNamed:@"smart_state"] isEqualToString:@"slot-is-empty"])
	{
		/* Draw blank slot */
		backImage = driveBlankImage;
		slotIsPopulated = NO;
	}
	else
	{
		/* Use real back */
		backImage = driveBackImage;
		slotIsPopulated = YES;
	}

	/* Popout Back */
	if (isRaised)
	{
		/* Get rounded pop-out path */
		NSBezierPath *popoutPath = [LCRoundedBezierPath pathInRect:popoutRect];
		
		/* Create clipping path */
		NSBezierPath *clipPath= [NSBezierPath bezierPathWithRect:driveRect];
		[clipPath appendBezierPath:popoutPath];

		/* Save state */
		[[NSGraphicsContext currentContext] saveGraphicsState];
		
		/* Set Shadow */
		NSShadow *theShadow = [[NSShadow alloc] init]; 
		[theShadow setShadowOffset:NSMakeSize(5.0, -5.0)]; 
		[theShadow setShadowBlurRadius:7.0];
		[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:1.0]];		
		[theShadow set];
		
		/* Draw under */
		[[NSColor colorWithCalibratedRed:0.1 green:0.1 blue:0.1 alpha:0.8] setFill];
		[clipPath fill];

		/* Release shadow */
		[theShadow release];
		
		/* Restore context */
		[[NSGraphicsContext currentContext] restoreGraphicsState];
		
		/* Save state */
		[[NSGraphicsContext currentContext] saveGraphicsState];
				
		[clipPath addClip];
		[popupImage drawInRect:popoutRect
				   fromRect:NSMakeRect(0, 0,[popupImage size].width,[popupImage size].height)
				  operation:NSCompositeSourceOver
				   fraction:1.0];			
		
		/* Restore context */
		[[NSGraphicsContext currentContext] restoreGraphicsState];

	}
	
	/* Draw drive */
	[backImage drawInRect:driveRect
					  fromRect:NSMakeRect(0,0,[backImage size].width,[backImage size].height)
					 operation:NSCompositeSourceOver 
					  fraction:1.0];
	

	/* Drive Number */
	NSRect circleRect;
	if (slot < 7)
	{ circleRect = NSMakeRect(NSMaxX(driveRect)-14,NSMaxY(driveRect)-16,14,14); }
	else
	{ circleRect = NSMakeRect(NSMaxX(driveRect)-18,NSMaxY(driveRect)-16,14,14); }
	NSString *driveNumber = [NSString stringWithFormat:@"%i", slot+1];
	NSDictionary *attr;
	if (slotIsPopulated)
	{
		attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.8], NSForegroundColorAttributeName,
			[NSFont fontWithName:@"Bank Gothic Light BT" size:9.0], NSFontAttributeName,
			nil];
	}
	else
	{
		attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.4], NSForegroundColorAttributeName,
			[NSFont fontWithName:@"Bank Gothic Light BT" size:9.0], NSFontAttributeName,
			nil];
	}	
	if ([driveNumber length] == 1)
	{ [driveNumber drawInRect:circleRect withAttributes:attr]; }
	else
	{ [driveNumber drawInRect:circleRect withAttributes:attr]; }			
		
	/* Check for empty drive */
	if (!slotIsPopulated)
	{ return; }
	
	/* Array member window */
	arrayRect = NSMakeRect(driveRect.origin.x+7,driveRect.origin.y+10,22,20);
	[arrayWindowImage drawInRect:arrayRect 
						fromRect:NSMakeRect(0,0,[arrayWindowImage size].width,[arrayWindowImage size].height) 
					   operation:NSCompositeSourceOver 
						fraction:1.0];
	
	/* Array member number */
	NSString *arrayString = [driveObject valueForMetricNamed:@"arraynumber"];
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.45 green:0.83 blue:0.72 alpha:1.0], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"DotMatrix" size:14], NSFontAttributeName,
		nil];
	[arrayString drawAtPoint:NSMakePoint(arrayRect.origin.x+6, arrayRect.origin.y) withAttributes:attr];
	
	/* Status dot */
	NSRect driveStatusRect = NSMakeRect (NSMinX(driveRect)+7, NSMaxY(driveRect)-12, 6, 6);
	NSBezierPath *driveStatusPath = [NSBezierPath bezierPathWithOvalInRect:driveStatusRect];
	[[driveObject opStateColor] setFill];
	[driveStatusPath fill];
	[[NSColor darkGrayColor] setStroke];
	[driveStatusPath stroke];
	
	/* Size */
	NSGraphicsContext *textContext = [NSGraphicsContext currentContext];
	[textContext saveGraphicsState];
	NSAffineTransform* affine = [NSAffineTransform transform];
	[affine translateXBy:NSMinX(driveRect)+18 yBy:NSMinY(driveRect)+37];
	[affine rotateByDegrees:90];
	[affine concat];
	NSArray *sizeStrings = [[driveObject valueForMetricNamed:@"capacity"] componentsSeparatedByString:@"."];
	if (sizeStrings && [sizeStrings count] > 0)
	{
		NSString *sizeString = [NSString stringWithFormat:@"%@Gb", [sizeStrings objectAtIndex:0]];
		attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.7], NSForegroundColorAttributeName,
			[NSFont fontWithName:@"Bank Gothic Medium BT" size:10], NSFontAttributeName,
			nil];
		[sizeString drawAtPoint:NSMakePoint(0, 0) withAttributes:attr];
	}
	[textContext restoreGraphicsState];		
	
	/* Status Message */
	textContext = [NSGraphicsContext currentContext];
	[textContext saveGraphicsState];
	affine = [NSAffineTransform transform];
	[affine translateXBy:NSMinX(driveRect)+26 yBy:NSMinY(driveRect)+37];
	[affine rotateByDegrees:90];
	[affine concat];
	NSString *statusString = @"Unknown";
	if ([[driveObject rawValueForMetricNamed:@"rebuilding"] intValue] == 1)
	{ statusString = @"Rebuilding";	}
	else if ([[driveObject rawValueForMetricNamed:@"online"] intValue] == 1)
	{ 
		if ([[driveObject rawValueForMetricNamed:@"arraymember"] intValue] == 0)
		{ statusString = @"Unassigned";	}
		else
		{ statusString = @"Online";	}
	}
	else if ([[driveObject rawValueForMetricNamed:@"online"] intValue] == 0)
	{ statusString = @"Offline";	}
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.7], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Bank Gothic Light BT" size:10.0], NSFontAttributeName,
		nil];
	[statusString drawAtPoint:NSMakePoint(0, 0) withAttributes:attr];
	[textContext restoreGraphicsState];		
	
	/* Cache */
	NSString *cacheMetric = [driveObject rawValueForMetricNamed:@"diskcache_enabled"];
	if ([cacheMetric intValue] == 1)
	{
		NSRect cacheRect = NSMakeRect(NSMinX(driveRect)+2,NSMinY(driveRect)+2,NSWidth(driveRect)-4,2);
		NSColor *cacheColor = [NSColor colorWithDeviceRed:0.0 green:0.0 blue:1.0 alpha:0.8];
		[cacheColor setFill];
		NSRectFill(cacheRect);
	}	
		
}

#pragma mark "Mouse Management"

- (void)mouseEntered:(NSEvent *)theEvent
{
	if (lowerTimer)
	{ 
		[lowerTimer invalidate]; 
		lowerTimer = nil;
	}
	if (raiseTimer)
	{
		[raiseTimer invalidate];
		raiseTimer = nil;
	}
	[[self window] makeFirstResponder:self];
	raiseTimer = [[NSTimer scheduledTimerWithTimeInterval:2.5
												   target:self 
												 selector:@selector(raise)
												 userInfo:nil
												  repeats:NO] retain];
}

- (void)mouseExited:(NSEvent *)theEvent
{
	if (raiseTimer)
	{ 
		[raiseTimer invalidate]; 
		raiseTimer = nil;
	}
	if (lowerTimer)
	{ 
		[lowerTimer invalidate]; 
		lowerTimer = nil;
	}
	lowerTimer = [[NSTimer scheduledTimerWithTimeInterval:0.2
												   target:self 
												 selector:@selector(lower)
												 userInfo:nil
												  repeats:NO] retain];
}

- (void) mouseDown:(NSEvent *)theEvent
{
	if (!trackingRectEnabled) return;
	if (raiseTimer)
	{ 
		[raiseTimer invalidate]; 
		raiseTimer = nil;
	}
	if (lowerTimer)
	{ 
		[lowerTimer invalidate]; 
		lowerTimer = nil;
	}
	if (isRaised) [self lower];
	else [self raise];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	currentMouseLocation = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	if (isRaised)
	{
		NSDictionary *fadeInDict = nil;
		NSDictionary *fadeOutDict = nil;
		
		if (NSPointInRect(currentMouseLocation,arrayRect) && !arrayViewShown)
		{
			/* Fade arrayView in */
			fadeInDict = [NSDictionary dictionaryWithObjectsAndKeys:
				[arrayViewController view], NSViewAnimationTargetKey, 
				NSViewAnimationFadeInEffect,
				NSViewAnimationEffectKey,
				nil];
			
			/* Fade driveView out */
			fadeOutDict = [NSDictionary dictionaryWithObjectsAndKeys:
				[driveViewController view], NSViewAnimationTargetKey, 
				NSViewAnimationFadeOutEffect,
				NSViewAnimationEffectKey,
				nil];
			
			/* Animate */
			NSViewAnimation *animation = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObjects: fadeInDict, fadeOutDict, nil]];
			[animation setDuration:0.4];
			[animation startAnimation];			
			[animation release];

			/* Set Flag */
			arrayViewShown = YES;
		}
		else if (!NSPointInRect(currentMouseLocation,arrayRect) && arrayViewShown)
		{
			/* Fade driveView in */
			fadeOutDict = [NSDictionary dictionaryWithObjectsAndKeys:
				[arrayViewController view], NSViewAnimationTargetKey, 
				NSViewAnimationFadeOutEffect,
				NSViewAnimationEffectKey,
				nil];

			/* Fade arrayView out */
			fadeInDict = [NSDictionary dictionaryWithObjectsAndKeys:
				[driveViewController view], NSViewAnimationTargetKey, 
				NSViewAnimationFadeInEffect,
				NSViewAnimationEffectKey,
				nil];
			
			/* Animate */
			NSViewAnimation *animation = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObjects: fadeInDict, fadeOutDict, nil]];
			[animation setDuration:0.4];
			[animation startAnimation];			
			[animation release];			
			
			/* Set Flag */
			arrayViewShown = NO;
		}		
	}
}

#pragma mark "Raise/Lower Operations"

- (void) raise
{
	if (!isRaised)
	{
		/* Save old frame */
		originalFrame = [self frame];
		
		/* Calculate new size */
		NSRect newFrame;
		if (slot == 6)
		{ newFrame = NSMakeRect(NSMinX(originalFrame)-20, NSMinY(originalFrame)-10, NSWidth(originalFrame), NSHeight(originalFrame)+20); }
		else if (slot == 7)
		{ newFrame = NSMakeRect(NSMinX(originalFrame)+20, NSMinY(originalFrame)-10, NSWidth(originalFrame), NSHeight(originalFrame)+20); }
		else if (slot < 7)
		{ newFrame = NSMakeRect(NSMinX(originalFrame)-10, NSMinY(originalFrame)-10, NSWidth(originalFrame), NSHeight(originalFrame)+20); }
		else
		{ newFrame = NSMakeRect(NSMinX(originalFrame)+10, NSMinY(originalFrame)-10, NSWidth(originalFrame), NSHeight(originalFrame)+20); } 

		/* Configure resize operation */
		NSDictionary *viewResize;
		viewResize = [NSDictionary dictionaryWithObjectsAndKeys:
			self, NSViewAnimationTargetKey, 
			[NSValue valueWithRect:originalFrame],
			NSViewAnimationStartFrameKey,
			[NSValue valueWithRect:newFrame],
			NSViewAnimationEndFrameKey,
			nil];
		
		/* Perform animation */
		NSViewAnimation *animation = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObjects: viewResize, nil]];
		[animation setDuration:0.2];
		[animation setAnimationBlockingMode:NSAnimationBlocking];
		[animation setDelegate:self];
		[animation startAnimation];
		[animation release];

		/* Set flag */
		isRaised = YES;
		
		/* Set Frame */	
		if (slot < 7)
		{ newFrame = NSMakeRect(NSMinX(newFrame)-10, NSMinY(newFrame)-10, 425, NSHeight(newFrame)+20); }
		else
		{ newFrame = NSMakeRect(NSMaxX(newFrame)-400, NSMinY(newFrame)-10, 420, NSHeight(newFrame)+20); }
		[self setFrame:newFrame];
		[self setNeedsDisplay:YES];
		
		/* Obtain drive viewcontroller */
		driveViewController = [driveObject entityViewController];			
		if ([[driveViewController view] respondsToSelector:@selector(setHitView:)])
		{ [(LCObjectView *) driveViewController setHitView:self]; }
		if (slot < 7)
		{ driveObjViewRect = NSMakeRect(NSMinX([self popoutRect])+32, NSMinY([self popoutRect]), NSWidth([self popoutRect])-35, NSHeight([self popoutRect])); }
		else
		{ driveObjViewRect = NSMakeRect(NSMinX([self popoutRect])+2, NSMinY([self popoutRect]), NSWidth([self popoutRect])-20, NSHeight([self popoutRect])); }
		[[driveViewController view] setFrame:driveObjViewRect];
		
		/* Obtain array viewcontroller */
		NSString *arrayString = [driveObject valueForMetricNamed:@"arraynumber"];
		LCContainer *arrayContainer = (LCContainer *) [[driveObject device] childNamed:[NSString stringWithFormat:@"xrarray_%@", arrayString]];
		LCObject *arrayObject = (LCObject *) [arrayContainer childNamed:@"master"];
		arrayViewController = [arrayObject entityViewController];			
		if ([[arrayViewController view] respondsToSelector:@selector(setHitView:)])
		{ [(LCObjectView *) arrayViewController setHitView:self]; }
		if (slot < 7)
		{ arrayObjViewRect = NSMakeRect(NSMinX([self popoutRect])+32, NSMinY([self popoutRect])-138, NSWidth([self popoutRect])-35, NSHeight([self popoutRect])+138); }
		else
		{ arrayObjViewRect = NSMakeRect(NSMinX([self popoutRect])+2, NSMinY([self popoutRect])-138, NSWidth([self popoutRect])-20, NSHeight([self popoutRect])+138); }
		[[arrayViewController view] setFrame:arrayObjViewRect];
		[[arrayViewController view] setHidden:YES];
		
		/* Set cell's sub-view */
		[self addSubview:[driveViewController view]];
		[self addSubview:[arrayViewController view]];
		
		/* Inform superview */
		LCXRDeviceView *superview = (LCXRDeviceView *) [self superview];
		[superview driveDidRaise:self];
	}
}

- (void) lower
{
	if (isRaised)
	{
		/* Remove objView */
		[driveViewController removeViewAndContent];
		driveViewController = nil;
		[arrayViewController removeViewAndContent];
		arrayViewController = nil;
		
		/* Configure resize operation */
		NSDictionary *viewResize;
		viewResize = [NSDictionary dictionaryWithObjectsAndKeys:
			self, NSViewAnimationTargetKey, 
			[NSValue valueWithRect:[self frame]],
			NSViewAnimationStartFrameKey,
			[NSValue valueWithRect:originalFrame],
			NSViewAnimationEndFrameKey,
			nil];
		
		/* Perform animation */
		NSViewAnimation *animation = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObjects: viewResize, nil]];
		[animation setDuration:0.2];
		[animation setAnimationBlockingMode:NSAnimationBlocking];
		[animation setDelegate:self];
		[animation startAnimation];
		[animation release];
		
		/* Set flag */
		isRaised = NO;				
		
		/* Re-display */
		[self removeTrackingRect:trackTag];
		[self setNeedsDisplay:YES];
		[[self superview] setNeedsDisplay:YES];
		
		/* Inform superview */
		LCXRDeviceView *superview = (LCXRDeviceView *) [self superview];
		[superview driveDidLower:self];		
	}
}

#pragma mark "Accessors"

@synthesize trackingRectEnabled;

- (BOOL) trackingRectEnabled
{ return trackingRectEnabled; }

- (void) setTrackingRectEnabled:(BOOL)value
{ 
	trackingRectEnabled = value;
	[self setNeedsDisplay:YES];
}

@end
