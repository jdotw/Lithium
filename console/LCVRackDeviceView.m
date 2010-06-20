//
//  LCVRackDeviceView.m
//  Lithium Console
//
//  Created by James Wilson on 30/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCVRackDeviceView.h"
#import "NSGradient-Selection.h"
#import <Quartz/Quartz.h>

@implementation LCVRackDeviceView

#pragma mark Constructors

- (id)initWithDevice:(LCVRackDevice *)newDevice inFrame:(NSRect)frame
{
	[self initWithFrame:frame];
	
	[self setRackDevice:newDevice];
	
	return self;
}

- (id)initWithFrame:(NSRect)frame 
{
    [super initWithFrame:frame];
	
	[self registerForDraggedTypes:[NSArray arrayWithObject:@"LCVRackDeviceHostRUIndex"]];
	
    return self;
}

- (void) removeViewAndContent
{
	/* Called when the view is no longer needed */
	[self setRackDevice:nil];
	[self setRackView:nil];
	[self setRackController:nil];
	[self removeFromSuperview];
}

- (void) dealloc
{
	[self removeTrackingArea:trackingArea];
	[trackingArea release];
	[super dealloc];
}

#pragma mark "Drawing"

- (void) drawRect:(NSRect)rect
{
    NSSize size = [self bounds].size;
	NSColor *deviceColour = [rackDevice colourWithAlpha:0.8];
	
	/* Draw background */
	NSBezierPath *outlinePath = [NSBezierPath bezierPathWithRect:[self bounds]];
	NSGradient *backGradient = [[[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedWhite:0.2 alpha:1.0]
															  endingColor:[NSColor colorWithCalibratedWhite:0.1 alpha:1.0]] autorelease];
	[backGradient drawInBezierPath:outlinePath angle:-90.0];
	[[NSColor blackColor] setStroke];
	[outlinePath stroke];
		
	/* Draw Selection Highlight */
	if (selected)
	{
		NSGradient *selectGradient = [NSGradient selectionGradientWithAlpha:0.6];
		[selectGradient drawInBezierPath:outlinePath angle:-90.0];
	}

	
	/* Main status light */
	NSRect statusRect = NSMakeRect([self bounds].origin.x + 3, 
								   [self bounds].origin.y + size.height - (0.75 * [self pointsPerRU]), 
								   (0.5 * [self pointsPerRU]), 
								   (0.5 * [self pointsPerRU]));
	NSBezierPath *statusPath = [NSBezierPath bezierPath];
	[statusPath appendBezierPathWithOvalInRect:statusRect];
	[[[rackDevice entity] opStateColor] setFill];
	[statusPath fill];
	
	// Device name 
	NSMutableString *ruStr = [NSMutableString stringWithString:[[rackDevice entity] displayString]];
	if ([ruStr length] > 16)
	{
		NSRange trimRange = NSMakeRange(16, [ruStr length]-16);
		[ruStr deleteCharactersInRange:trimRange];
		[ruStr appendString:@"..."];
	}
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
						  deviceColour, NSForegroundColorAttributeName,
						  [NSFont boldSystemFontOfSize: [self pointsPerRU]*0.5], NSFontAttributeName,
						  nil];
	[ruStr drawAtPoint:NSMakePoint(roundf([self bounds].origin.x + (2 * statusRect.size.width)), 
								   roundf([self bounds].origin.y + (size.height - (0.75 * [self pointsPerRU])))-1) 
		withAttributes:attr];

}

#pragma mark "Mouse Tracking"

- (void)updateTrackingAreas
{
	[super updateTrackingAreas];
	if (trackingArea)
	{
		[self removeTrackingArea:trackingArea];
		[trackingArea release];
	}		
	trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
												options:NSTrackingMouseEnteredAndExited|NSTrackingMouseMoved|NSTrackingActiveAlways
												  owner:self
											   userInfo:nil];
	[self addTrackingArea:trackingArea];
}

- (BOOL) acceptsFirstResponder
{ return YES; }

- (BOOL)resignFirstResponder
{ 
	if (isZoomView) return NO;
	return YES;
}

- (void) mouseEntered:(NSEvent *)event
{
	mouseEntered = YES;
}

- (void) mouseExited:(NSEvent *)event
{
	mouseEntered = NO;
	
	/* Re-draw */
	[self setNeedsDisplay:YES];
	
	/* Zoom handling */
	if ([self isZoomView])
	{
		[self removeViewAndContent];
	}
}

- (void) mouseMoved:(NSEvent *)event
{
	/* Set controllers location */
	[rackController setCurrentMouseLocationInWindow:[event locationInWindow]];
}

- (void) mouseDown:(NSEvent *)event
{
	/* Set Selection */
	rackController.selectedObject = rackDevice;
	
	/* See if option is held */
	if ([event clickCount] > 1 && !isZoomView)
	{
		/* Zoom the device view */
		float targetZoomFactor = 2.5;
		NSPoint devOriginInsetView = [self convertPoint:NSMakePoint(NSMinX([self bounds]), NSMidY([self bounds])) toView:rackController.insetView];
		NSSize zoomSize = NSMakeSize([self bounds].size.width * targetZoomFactor, [self bounds].size.height * targetZoomFactor);
		NSRect zoomRect = NSMakeRect(NSMidX([rackController.insetView bounds]) - (zoomSize.width * 0.5),
									 devOriginInsetView.y - (zoomSize.height * 0.5),
									 zoomSize.width, zoomSize.height);
		//		zoomRect = [self convertRect:zoomRect toView:[[self window] contentView]];
		LCVRackDeviceView *zoomView = [[[self class] alloc] initWithDevice:[self rackDevice] inFrame:zoomRect];
		[zoomView setRackController:[self rackController]];
		[zoomView setIsZoomView:YES];
		[zoomView setWantsLayer:YES];
		[[zoomView layer] setShadowRadius:5.0];
		[[zoomView layer] setShadowOffset:CGSizeMake(8.0, -8.0)];
		[[zoomView layer] setShadowOpacity:0.4];
		[rackController.insetView addSubview:zoomView positioned:NSWindowAbove relativeTo:nil];
		[zoomView setNeedsDisplay:YES];
		[zoomView autorelease];
	}	
}

- (void) mouseDragged:(NSEvent *)event
{
	/* Set Drag */
	if (rackController.editing && !isZoomView)
	{
		NSImage *dragImage = [[[NSImage alloc] initWithSize:[self frame].size] autorelease];
		[dragImage lockFocus];
		[self drawRect:[self bounds]];
		[dragImage unlockFocus];
		
		NSImage *transparentImage = [[[NSImage alloc] initWithSize:[dragImage size]] autorelease];
		[transparentImage lockFocus];
		[dragImage drawInRect:NSMakeRect(0.0, 0.0, [dragImage size].width, [dragImage size].height)
					 fromRect:NSMakeRect(0.0, 0.0, [dragImage size].width, [dragImage size].height)
					operation:NSCompositeSourceOver
					 fraction:0.5];
		[transparentImage unlockFocus];
		
		NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSDragPboard];
		[pboard declareTypes:[NSArray arrayWithObject:@"LCVRackDeviceHostRUIndex"] owner:[self superview]];
		[pboard setPropertyList:[NSArray arrayWithObject:[NSNumber numberWithInt:rackDevice.hostRUindex]] forType:@"LCVRackDeviceHostRUIndex"];
			
		[self retain];
		[self dragImage:transparentImage
					 at:NSMakePoint(NSMinX([self bounds]), NSMinY([self bounds]))
				 offset:NSMakeSize(0.0, 0.0)
				  event:event pasteboard:pboard source:self slideBack:YES];		
		[self release];
	}
}

#pragma mark "Menu Events"

- (NSMenu *) menuForEvent:(NSEvent *)event
{	
	rackController.selectedObject = rackDevice;
	return [rackController deviceMenu];
}

#pragma mark "KVO and Notification Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	[self setNeedsDisplay:YES];
	[[self rackController] resetCableViews];
}

- (void) deviceRefreshFinished:(NSNotification *)note
{
	[self setNeedsDisplay:YES];
	[[self rackController] resetCableViews];
}

#pragma mark "Properties"

@synthesize rackDevice;
- (void) setRackDevice:(LCVRackDevice *)value
{
	if (rackDevice)
	{
		[rackDevice removeObserver:self forKeyPath:@"colour"];
		[[NSNotificationCenter defaultCenter] removeObserver:self
														name:@"EntityRefreshFinished"
													  object:rackDevice.entity];
	}
	
	[rackDevice release];
	rackDevice = [value retain];
	
	if (rackDevice)
	{
		[rackDevice addObserver:self
					 forKeyPath:@"colour"
						options:NSKeyValueObservingOptionNew
						context:nil];
		
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(deviceRefreshFinished:)
													 name:@"EntityRefreshFinished"
												   object:rackDevice.entity];
	}
}

@synthesize rackController;
@synthesize rackView;
@synthesize isZoomView;

- (float) pointsPerRU
{
	return [self bounds].size.height / [rackDevice size];
}

@synthesize selected;
- (void) setSelected:(BOOL)value
{
	selected = value;
	[self setNeedsDisplay:YES];
}

@end
