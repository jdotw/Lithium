//
//  LCVRackView.m
//  Lithium Console
//
//  Created by James Wilson on 11/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCVRackView.h"
#import "LCVRackCable.h"
#import "LCVRackUnit.h"
#import "LCBrowserVRackContentController.h"
#import "LCEntity.h"
#import "LCEntityDescriptor.h"
#import "LCVRackDeviceView.h"

@implementation LCVRackView

#pragma mark Constructors

- (id)initWithFrame:(NSRect)frame 
{
    [super initWithFrame:frame];
	
	[self registerForDraggedTypes:[NSArray arrayWithObjects:@"LCEntityDescriptor", @"LCVRackDeviceHostRUIndex", nil]];
	
    return self;
}

- (void) dealloc
{
	if (trackingArea)
	{
		[self removeTrackingArea:trackingArea];
		[trackingArea release];
	}
	[super dealloc];
}

- (void) updateTrackingAreas
{
	[super updateTrackingAreas];
	if (trackingArea)
	{
		[self removeTrackingArea:trackingArea];
		[trackingArea release];
	}
	trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds] 
												options:NSTrackingActiveAlways|NSTrackingMouseMoved|NSTrackingMouseEnteredAndExited 
												  owner:self
											   userInfo:nil];
	[self addTrackingArea:trackingArea];
}	

#pragma mark Drawing

- (void) drawRect:(NSRect)drawInRect
{
    NSSize size = [self bounds].size;

	/* Draw outside border */
	[[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:1.0] setFill];
	
	/* Draw rails */
	[[NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.8 alpha:0.8] setStroke];
	NSBezierPath *rails;
	rails = [NSBezierPath bezierPath];
	[rails moveToPoint:NSMakePoint(railX1, margin)];
	[rails lineToPoint:NSMakePoint(railX1, size.height-margin)];
	[rails moveToPoint:NSMakePoint(railX1+5, margin)];
	[rails lineToPoint:NSMakePoint(railX1+5, size.height-margin)];
	[rails moveToPoint:NSMakePoint(railX2, margin)];
	[rails lineToPoint:NSMakePoint(railX2, size.height-margin)];
	[rails moveToPoint:NSMakePoint(railX2-5, margin)];
	[rails lineToPoint:NSMakePoint(railX2-5, size.height-margin)];
	[rails setLineWidth:1.0 * (NSWidth([self bounds]) / 304)];
	[rails stroke];
	
	/* Determine where mouse is */
	NSPoint mouseLocation = [[NSApp currentEvent] locationInWindow];
	LCVRackUnit *mouseOverRU = [self rackUnitAtPoint:mouseLocation];

	/* Draw RUs */
	int i;
	for (i=0; i < 48; i++)
	{
		LCVRackUnit *ru = [[[controller rack] rackUnits] objectAtIndex:47-i];
		
		/* Set RU's rect */
		NSRect ruRect = NSMakeRect(railX1, roundf(margin + (i * pointsPerRU)), railX2 - railX1, roundf(pointsPerRU));
		[ru setRect:ruRect];
		
		NSBezierPath *ruGuide = [NSBezierPath bezierPath];
		[[NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.8 alpha:0.2] setStroke];
		[ruGuide moveToPoint:NSMakePoint(railX1, margin + (i * pointsPerRU))];
		[ruGuide lineToPoint:NSMakePoint(railX2, margin + (i * pointsPerRU))];
		[ruGuide stroke];
		NSString *ruStr = [NSString stringWithFormat:@"%i", ru.ruIndex];
		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.8 alpha:0.8], NSForegroundColorAttributeName,
			[NSFont boldSystemFontOfSize: pointsPerRU*0.5], NSFontAttributeName,
			nil];
		[ruStr drawAtPoint:NSMakePoint(margin+(3 * (NSWidth([self bounds]) / 304)), margin + (i * pointsPerRU) + (0.25 * pointsPerRU)) withAttributes:attr];
		[ruStr drawAtPoint:NSMakePoint(size.width - margin - (14 * (NSWidth([self bounds]) / 304)), margin + (i * pointsPerRU) + (0.25 * pointsPerRU)) withAttributes:attr];
		
		/* Check mouse over */
		if (ru == mouseOverRU && draggingEntered && ((LCBrowserVRackContentController *)controller).editing)
		{
			NSColor *alternateSelectedControlColor = [NSColor alternateSelectedControlColor];
			CGFloat hue, saturation, brightness, alpha;
			[[alternateSelectedControlColor colorUsingColorSpaceName:NSDeviceRGBColorSpace] getHue:&hue saturation:&saturation brightness:&brightness alpha:&alpha];
			alpha=0.8;
			
			// Create synthetic darker and lighter versions
			NSColor *lighterColor = [NSColor colorWithDeviceHue:hue
													 saturation:MAX(0.0, saturation-.12) brightness:MIN(1.0,
																										brightness+0.30) alpha:alpha];
			NSColor *darkerColor = [NSColor colorWithDeviceHue:hue
													saturation:MIN(1.0, (saturation > .04) ? saturation+0.12 :
																   0.0) brightness:MAX(0.0, brightness-0.045) alpha:alpha];
			NSGradient *gradient = [[[NSGradient alloc] initWithStartingColor:lighterColor endingColor:darkerColor] autorelease];
			
			NSRect highlightRect = NSMakeRect(NSMinX(ruRect), NSMinY(ruRect) - (NSHeight(ruRect) * (dropDeviceSize-1)), 
											  NSWidth(ruRect), NSHeight(ruRect) * dropDeviceSize);
			NSBezierPath *dropHighlightPath = [NSBezierPath bezierPathWithRect:NSMakeRect(NSMinX(highlightRect)-1.0, NSMinY(highlightRect)-1.0, 
																						  NSWidth(highlightRect)+2.0, NSHeight(highlightRect)+2.0)];
			[gradient drawInBezierPath:dropHighlightPath angle:90];
		}
	}
	
	/* Check for live resize */
	if ([self inLiveResize]) 
	{
		[controller resetDeviceViews];
		[controller resizeCableViews];
	}
}

- (void) setFrame:(NSRect)rect
{
	[super setFrame:rect];
	[self updateSizes];
	[controller resetDeviceViews];
	[controller resizeCableViews];
	[self setNeedsDisplay:YES];
}

#pragma mark Sizes

- (void) updateSizes
{
    NSSize size = [self frame].size;
	margin = roundf(0 * (NSWidth([self bounds]) / 304));
	railX1 = roundf(margin+(18 * (NSWidth([self bounds]) / 304)));
	railX2 = roundf(size.width-margin-(18 * (NSWidth([self bounds]) / 304)));
	pointsPerRU = roundf((size.height - 2*margin) / 48);
}

#pragma mark Rack Unit Methods

- (LCVRackUnit *) rackUnitAtPoint:(NSPoint)locationInWindow
{
	/* Get point */
	NSPoint ruPoint = [self convertPoint:locationInWindow fromView:nil];
	
	/* Check bounds */
	NSSize size = [self bounds].size;
	if (ruPoint.x < (0 + margin) || ruPoint.x > (size.width - margin)) return nil;
	if (ruPoint.y < (0 + margin) || ruPoint.y > (size.height - margin)) return nil;
	
	/* Find RU */
	LCVRackUnit *ru;
	int rackUnitIndex = (48 - ((ruPoint.y - margin) / pointsPerRU));
	if (rackUnitIndex >= 0 && rackUnitIndex < [[[(LCBrowserVRackContentController *)controller rack] rackUnits] count])
	{ ru = [[[(LCBrowserVRackContentController *)controller rack] rackUnits] objectAtIndex:rackUnitIndex]; }
	else
	{ ru = nil; }

	return ru;
}

#pragma mark Selection Methods 

- (void) calculateSelection:(NSPoint)locationInWindow
{
	LCBrowserVRackContentController *rackController = controller;

	/* Check contents of clicked RU */
	if ([self rackUnitAtPoint:locationInWindow])
	{
		/* RU was selected */
		LCVRackUnit *selectedRU = [self rackUnitAtPoint:locationInWindow];
		LCVRackDevice *hostedDevice = [selectedRU hostedDevice];
		if (!hostedDevice)
		{
			/* Empty RU is selected -- clear selection */
			rackController.selectedObject = nil;
		}
	}
	else
	{
		/* No selection */
		rackController.selectedObject = nil;
	}
}

#pragma mark Mouse Event Handling

- (BOOL)acceptsFirstResponder
{
	return YES; 
}

- (void)mouseDown:(NSEvent *)theEvent
{
	/* Set selection */
	[self calculateSelection:[theEvent locationInWindow]];
	
	/* Check bounds */
	NSPoint hitPoint = [[controller cableView] convertPoint:[theEvent locationInWindow] fromView:nil];
	NSSize size = [[controller cableView] bounds].size;
	if (hitPoint.x < 0 || hitPoint.x > size.width) return;
	if (hitPoint.y < 0 || hitPoint.y > size.height) return;

	[super mouseDown:theEvent];
}

- (void)mouseEntered:(NSEvent *)theEvent
{
	mouseEntered = true;
	[[self window] makeFirstResponder:self];
	[super mouseEntered:theEvent];
}

- (void)mouseExited:(NSEvent *)theEvent
{
	mouseEntered = false;
	[super mouseExited:theEvent];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	if (mouseEntered) 
	{
		/* The device views will take care of setting
		 * the hover entity for device/metric/interface. 
		 * We just need to check if something else is hovered
		 * over like a cable
		 */
		
		/* Set location */
		LCBrowserVRackContentController *contentController = controller;
		[contentController setCurrentMouseLocationInWindow:[theEvent locationInWindow]];		
	}

	[controller setCurrentMouseLocationInWindow:[theEvent locationInWindow]];
	
	[self setNeedsDisplay:YES];
}

#pragma mark "Key Event"

- (void) keyDown:(NSEvent *)event
{
	[controller setCableEndPointA:nil];
	[controller setCableHighlightDevice:nil];
}

#pragma mark "Menu Events"

- (NSMenu *) menuForEvent:(NSEvent *)event
{
//		return [controller rackUnitMenu];
	
	return nil;
}

#pragma mark "Drag and Drop"

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)info 
{
	/* Check document is locked for edit */
	if (![[controller rack] editing]) return NSDragOperationNone;
		
	/* Check for a move operations */
	if ([[[info draggingSource] class] isSubclassOfClass:[LCVRackDeviceView class]])
	{
		/* Device view drag/drop */
		if ([[info draggingSource] superview] == self) 
		{
			/* Get Rack Device Moves */
			if ([[info draggingPasteboard] propertyListForType:@"LCVRackDeviceHostRUIndex"])
			{
				for (NSNumber *sourceRUIndex in [[info draggingPasteboard] propertyListForType:@"LCVRackDeviceHostRUIndex"])
				{
					LCBrowserVRackContentController *rackController = (LCBrowserVRackContentController *)controller;
					LCVRackDocument *rack = rackController.rack;
					LCVRackDevice *movedDevice = nil;
					for (LCVRackDevice *device in rack.devices)
					{
						if (device.hostRUindex == [sourceRUIndex intValue])
						{ 
							movedDevice = device;
							break;
						}
					}
					if (movedDevice) dropDeviceSize = movedDevice.size;
					else dropDeviceSize = 1;
				}
			}
			
			dragOperation = NSDragOperationMove;
		}
	}
	else if ([[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"])
	{
		dragOperation = NSDragOperationCopy;
		dropDeviceSize = 1;
	}
	else
	{
		dragOperation = NSDragOperationNone;
	}
	
	draggingEntered = YES;
	return dragOperation;
}

- (void) draggingExited:(id <NSDraggingInfo>)sender
{
	draggingEntered = NO;
	[self setNeedsDisplay:YES];	
}

- (NSDragOperation)draggingUpdated:(id < NSDraggingInfo >)sender
{
	draggingEntered = YES;
	[self setNeedsDisplay:YES];
	return dragOperation;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)info
{
	/* Reset dragging state and redraw */
	draggingEntered = NO;
	[self setNeedsDisplay:YES];	

	/* Check document is locked for edit */
	if (![[controller rack] editing]) return NO;

	/* Locate drop RU */
	LCVRackUnit *ru = [self rackUnitAtPoint:[info draggingLocation]];
	if (!ru) return NO;
	
	/* Check nothing is present */
	if ([ru hostedDevice])
	{ return NO; }
	
	/* Get Entities */
	if ([[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"])
	{
		NSArray *entityDescriptorArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
		for (NSDictionary *entityDescriptorProperties in entityDescriptorArray)
		{
			LCEntityDescriptor *entityDescriptor = [LCEntityDescriptor descriptorWithProperties:entityDescriptorProperties];
			LCEntity *entity = [entityDescriptor locateEntity:NO];
			
			if (!entity || [[entity typeInteger] intValue]!= 3) continue;
			
			/* Create rack device */
			LCVRackDevice *rackDev = [LCVRackDevice rackDeviceWithEntity:entity];
			[rackDev setHostRU:ru];
			[rackDev setSize:1];
			[[controller rack] insertObject:rackDev inDevicesAtIndex:0];
			[ru setHostedDevice:rackDev];
			
			/* Refresh entity */
			[[rackDev entity] highPriorityRefresh];
		}
	}
	
	/* Get Rack Device Moves */
	if ([[info draggingPasteboard] propertyListForType:@"LCVRackDeviceHostRUIndex"])
	{
		for (NSNumber *sourceRUIndex in [[info draggingPasteboard] propertyListForType:@"LCVRackDeviceHostRUIndex"])
		{
			LCBrowserVRackContentController *rackController = (LCBrowserVRackContentController *)controller;
			LCVRackDocument *rack = ((LCBrowserVRackContentController *)controller).rack;
			LCVRackDevice *movedDevice = nil;
			for (LCVRackDevice *device in rack.devices)
			{
				if (device.hostRUindex == [sourceRUIndex intValue])
				{ 
					movedDevice = device;
					break;
				}
			}
			if (movedDevice)
			{
				/* Move from document */
				[movedDevice.hostRU setHostedDevice:nil];
				movedDevice.hostRUindex = ru.ruIndex;
				movedDevice.hostRU = [rack.rackUnits objectAtIndex:movedDevice.hostRUindex-1];
				[(LCVRackUnit *) movedDevice.hostRU setHostedDevice:movedDevice];
				[rackController resetDeviceViews];
				[rackController resetCableViews];
			}
		}
	}
	
	draggingEntered = NO;
	
    return YES;
}

#pragma mark Rectangle for Device Method

- (NSRect) rectForDevice:(LCVRackDevice *)rDev
{
	/* Returns the rect for the devices view to 
	 * sit in. This is taken by finding the first
	 * host rack unit; calculating the rect for it;
	 * then extrapolating the full size of the devices
	 * rect depending on how many RU it is 
	 */
	
	/* Get host RU */
	LCVRackUnit *hostRU = [rDev hostRU];

	/* Create RU rect */
	NSRect ruRect = NSMakeRect(roundf(railX1), roundf(margin + ((49-hostRU.ruIndex) * pointsPerRU)), roundf(railX2 - railX1), roundf(pointsPerRU));
	
	/* Create dev rect */
	NSRect devRect = NSMakeRect (ruRect.origin.x, roundf(ruRect.origin.y - (pointsPerRU * [rDev size])), ruRect.size.width, roundf(ruRect.size.height * [rDev size]));
	
	return devRect;
}

#pragma mark Rectangle for Rack 

- (NSRect) rectForRack
{
	return NSMakeRect(roundf([self bounds].origin.x+margin), roundf([self bounds].origin.y+margin), 
					  roundf([self bounds].size.width - (2 * margin)), roundf([self bounds].size.height - (2 * margin)));
}

#pragma mark General Accessors
@synthesize pointsPerRU;
@synthesize controller;

@end
