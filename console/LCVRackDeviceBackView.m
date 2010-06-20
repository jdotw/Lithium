//
//  LCVRackDeviceBackView.m
//  Lithium Console
//
//  Created by James Wilson on 12/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCVRackDeviceBackView.h"

#import "LCVRackInterface.h"
#import "NSGradient-Selection.h"

@implementation LCVRackDeviceBackView

- (id) initWithFrame:(NSRect)rect
{
	self = [super initWithFrame:rect];
	if (self != nil) 
	{
		interfaces = [[NSMutableArray array] retain];
	}
	return self;
}


- (void) removeViewAndContent
{
	/* Called when the view is no longer needed */
	[interfaces removeAllObjects];
	[super removeViewAndContent];
}

- (void) dealloc
{
	[popupView release];
	[interfaces release];
	[selectedInterfaceEntity release];
	[super dealloc];
}

#pragma mark Drawing

- (void)drawRect:(NSRect)rect 
{
	[super drawRect:rect];
	
    NSSize size = [self bounds].size;
	
	/* 
	 * Draw Interfaces 
	 */
	
	/* Get interfaces */
	LCEntity *ifaceContainer = [[[rackDevice entity] childrenDictionary] objectForKey:@"iface"];
	if (!ifaceContainer)
	{ ifaceContainer = [[[rackDevice entity] childrenDictionary] objectForKey:@"xsnetwork"]; }
	if (!ifaceContainer)
	{ ifaceContainer = [[[rackDevice entity] childrenDictionary] objectForKey:@"xsinetwork"]; }
	if (!ifaceContainer)
	{ ifaceContainer = [[[rackDevice entity] childrenDictionary] objectForKey:@"xnetwork"]; }
	if (!ifaceContainer)
	{ ifaceContainer = [[[rackDevice entity] childrenDictionary] objectForKey:@"apphysical"]; }
	NSArray *sortDescArray = [NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"name" ascending:YES selector:@selector(localizedCompare:)] autorelease]];
	NSMutableArray *ifaceArray = [NSMutableArray arrayWithArray:[[ifaceContainer children] sortedArrayUsingDescriptors:sortDescArray]];
	if ([[[rackDevice entity] childrenDictionary] objectForKey:@"fcport"])
	{ [ifaceArray addObjectsFromArray:[[[[rackDevice entity] childrenDictionary] objectForKey:@"fcport"] children]]; }
	if ([[[rackDevice.entity device] vendor] isEqualToString:@"xraid"])
	{
		if ([[rackDevice entity] valueForKeyPath:@"childrenDictionary.xrhostiface_1.childrenDictionary.master"])
		{ [ifaceArray addObject:[[rackDevice entity] valueForKeyPath:@"childrenDictionary.xrhostiface_1.childrenDictionary.master"]]; }
		if ([[rackDevice entity] valueForKeyPath:@"childrenDictionary.xrhostiface_2.childrenDictionary.master"])
		{ [ifaceArray addObject:[[rackDevice entity] valueForKeyPath:@"childrenDictionary.xrhostiface_2.childrenDictionary.master"]]; }
	}
			

	/* Get enum */
	
	/* Calculate sizes */
	float ifaceMarginx = roundf(size.width * 0.35);			/* The start of the interfaces */
	float ifaceRectWidth = roundf([self pointsPerRU] * 0.25);		/* Width (&height) of iface circle NSRect */
	float ifaceSpacer = roundf(ifaceRectWidth * 0.7);		/* Space between iface circles */
	float ifacesOnRow = 0;							/* Count of interfaces on the row */
	float currentRowy;								/* Current row y value */
	int maxIfacePerRow = 24;						/* Maximum interfaces per row */
	float ifaceRow1y;								/* Row 1 y value */
	float ifaceRow2y;								/* Row 2 y value */
	float ifaceRow3y;								/* Row 3 y value */
	if ([[ifaceContainer children] count] <= maxIfacePerRow)
	{
		/* Single row */
		ifaceRow1y = [self bounds].origin.y + (0.50 * size.height);
	}
	else if ([[ifaceContainer children] count] <= (maxIfacePerRow * 2))
	{
		/* Double row */
		ifaceRow1y = [self bounds].origin.y + (0.66 * size.height);
		ifaceRow2y = [self bounds].origin.y + (0.33 * size.height);
	}
	else
	{
		/* Triple row */
		ifaceRow1y = [self bounds].origin.y + (0.75 * size.height);
		ifaceRow2y = [self bounds].origin.y + (0.50 * size.height);
		ifaceRow3y = [self bounds].origin.y + (0.25 * size.height);
	}
	currentRowy = ifaceRow1y;
	
	/* Draw interface circles */
	NSRect ifaceRect = NSMakeRect (roundf(ifaceMarginx), roundf(currentRowy - (0.5 * ifaceRectWidth)), roundf(ifaceRectWidth), roundf(ifaceRectWidth));
	LCEntity *iface;
	[interfaces removeAllObjects];
	for (iface in ifaceArray)
	{
		/* Increment x coord */
		ifaceRect.origin.x += ifaceRectWidth;			/* Iface circle width */
		ifaceRect.origin.x += ifaceSpacer;				/* Spacer */
		
		/* Create/draw path */
		NSBezierPath *ifacePath = [NSBezierPath bezierPathWithOvalInRect:ifaceRect];
		[[iface opStateColorWithAlpha:0.8] setStroke];
		if (iface == rackController.selectedEntity)
		{ [ifacePath setLineWidth:3.0]; }
		else
		{ [ifacePath setLineWidth:1.0]; }
		[ifacePath stroke];

		/* If zoomed, draw in fake-cable */
		if (isZoomView)
		{
			NSMutableArray *cableList = [rackController cablesForInterface:iface];
			if ([cableList count] > 0)
			{
				/* Draw fake-cable */
				LCVRackCable *cable = [cableList objectAtIndex:0];
				[[cable colourWithAlpha:1.0] setFill];
				[ifacePath fill];
			}
		}

		/* Draw utilisation arcs */
		LCEntity *utilInMetric = [[iface childrenDictionary] objectForKey:@"utilpc_in"];
		NSString *utilValue = [[utilInMetric properties] objectForKey:@"valstr_raw"];
		if (utilInMetric && utilValue)
		{
			NSBezierPath *utilPath;
			utilPath = [NSBezierPath bezierPath];
			float utilFactor = [utilValue floatValue] / 100;
			if (utilFactor > 1) utilFactor = 1.00;
			[utilPath appendBezierPathWithArcWithCenter:NSMakePoint(ifaceRect.origin.x + (0.5 * ifaceRect.size.width),
																	ifaceRect.origin.y + (0.5 * ifaceRect.size.height))
												 radius:(0.3 * ifaceRect.size.width)
											 startAngle:90.00 + (180 - (180 * utilFactor))
											   endAngle:270];
			[[utilInMetric opStateColor] setStroke];
			[utilPath stroke];
		}
		LCEntity *utilOutMetric = [[iface childrenDictionary] objectForKey:@"utilpc_out"];
		utilValue = [[utilOutMetric properties] objectForKey:@"valstr_raw"];
		if (utilOutMetric && utilValue)
		{
			NSBezierPath *utilPath;
			utilPath = [NSBezierPath bezierPath];
			float utilFactor = [utilValue floatValue] / 100;
			if (utilFactor > 1) utilFactor = 1.00;
			if (utilFactor <= 0.5)
			{ 
				/* 1-Part arc */
				float endAngle = 270 + (utilFactor * 180); 
				[utilPath appendBezierPathWithArcWithCenter:NSMakePoint(ifaceRect.origin.x + (0.5 * ifaceRect.size.width),
																		ifaceRect.origin.y + (0.5 * ifaceRect.size.height))
													 radius:(0.3 * ifaceRect.size.width)
												 startAngle:270
												   endAngle:endAngle];
				[[utilOutMetric opStateColor] setStroke];
				[utilPath stroke];
			}
			else
			{ 
				/* 2-Part arc */
				[utilPath appendBezierPathWithArcWithCenter:NSMakePoint(ifaceRect.origin.x + (0.5 * ifaceRect.size.width),
																		ifaceRect.origin.y + (0.5 * ifaceRect.size.height))
													 radius:(0.3 * ifaceRect.size.width)
												 startAngle:270
												   endAngle:359];
				float endAngle = (utilFactor * 180) - 90; 
				[utilPath appendBezierPathWithArcWithCenter:NSMakePoint(ifaceRect.origin.x + (0.5 * ifaceRect.size.width),
																		ifaceRect.origin.y + (0.5 * ifaceRect.size.height))
													 radius:(0.3 * ifaceRect.size.width)
												 startAngle:0
												   endAngle:endAngle];
				[[utilOutMetric opStateColor] setStroke];
				[utilPath stroke];				
			}
		}
		
		/* Create interface object */
		LCVRackInterface *rackIface = [[[LCVRackInterface alloc] initWithEntity:iface atRect:ifaceRect] autorelease];
		[rackIface setDeviceBackView:self];
		[interfaces addObject:rackIface];
		
		/* Increment iface counter */
		ifacesOnRow += 1;
		if (ifacesOnRow == maxIfacePerRow)
		{
			/* Row maximum hit, set new y coord */
			if (currentRowy == ifaceRow1y)
			{ currentRowy = ifaceRow2y; }
			else if (currentRowy == ifaceRow2y)
			{ currentRowy = ifaceRow3y; }

			/* Configure ifaceRect */
			ifaceRect.origin.y = currentRowy - (0.5 * ifaceRectWidth);
			ifaceRect.origin.x = ifaceMarginx;
			
			/* Reset counter */
			ifacesOnRow = 0;
		}
	}
}

#pragma mark Mouse Event Handling

- (void) mouseDown:(NSEvent *)event
{
	mouseDown = YES;
	
	/* Super class must run first, as it will select the device */
	[super mouseDown:event];
	
	/* Locate interface */
	LCVRackInterface *rackIface = nil;
	for (rackIface in interfaces)
	{
		if ([[rackIface path] containsPoint:[self convertPoint:[event locationInWindow] fromView:nil]])
		{
			break;
		}
	}

	/* Set selected */
	if (rackIface) rackController.selectedObject = rackIface;
	else rackController.selectedObject = rackDevice;

	/* See if option is held */
	if ([event modifierFlags] & NSCommandKeyMask)
	{
		/* Drawing a cable */
		if ([rackController.rack editing]) 
		{ [rackController cableEndPointNominated:[rackIface entity]]; }
	}
	
	/* Check to see if cable draw in progress */
	if (rackController.cableEndPointA && rackIface)
	{
		/* Cable draw in progress, nominate this iface as the end point */
		[rackController cableEndPointNominated:rackIface.entity];
	}
	
	[self setNeedsDisplay:YES];
}

- (void) mouseUp:(NSEvent *)theEvent
{
	mouseDown = NO;
}

- (void) mouseExited:(NSEvent *)event
{
	/* Remove popup */
	if (self.popupView)
	{
		[self.popupView removeFromSuperview];
		self.popupView = nil;
	}
	/* Check mouse state */
	if (mouseDown)
	{
		/* Draw-Drawing a cable */
		if ([rackController.rack editing]) 
		{ [rackController cableEndPointNominated:[hoverInterface entity]]; }
	}

	[super mouseExited:event];
}

- (void) mouseMoved:(NSEvent *)event
{
	/* Look for interfaces */
	LCVRackInterface *currentHover = nil;
	for (LCVRackInterface *iface in interfaces)
	{
		if ([[iface path] containsPoint:[self convertPoint:[event locationInWindow] fromView:nil]])
		{
			/* Set current hover */
			currentHover = iface;
			break;
		}
	}

	/* Check for change in hover */
	if (currentHover != hoverInterface)
	{
		/* Set hover */
		self.hoverInterface = currentHover;	
		
		/* Remove old popup */
		[self.popupView removeFromSuperview];
		self.popupView = nil;
		
		/* Create new popup */
		if (self.hoverInterface)
		{
			self.popupView = [self popupViewForInterface:self.hoverInterface];
			NSRect convertedRect = [self convertRect:[self.popupView frame] toView:[[self window] contentView]];
			convertedRect.size.width = roundf(convertedRect.size.width);
			convertedRect.size.height = roundf(convertedRect.size.height);			
			[self.popupView setFrame:convertedRect];
			[[[self window] contentView] addSubview:popupView];
		}
	}
	else 
	{
		/* No Change, Just move popup view */
		if (self.popupView)
		{
			NSRect rect = [self popupRectForInterface:self.hoverInterface];
			NSRect convertedRect = [self convertRect:rect toView:[[self window] contentView]];
			convertedRect.size.width = roundf(convertedRect.size.width);
			convertedRect.size.height = roundf(convertedRect.size.height);			
			[self.popupView setFrame:convertedRect];
		}
	}	

	/* Check for option key being held */
	if ([event modifierFlags] & NSCommandKeyMask)
	{
		[rackController setCableHighlightDevice:[rackDevice entity]];
		cableHighlightDeviceSet = YES;
	}
	else if (cableHighlightDeviceSet)
	{ 
		[rackController setCableHighlightDevice:nil]; 
		cableHighlightDeviceSet = NO;
	}
	
	[super mouseMoved:event];
}

#pragma mark "Popup Methods"

- (NSRect) popupRectForInterface:(LCVRackInterface *)interface
{
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
						  [NSFont boldSystemFontOfSize:11.0], NSFontAttributeName,
						  nil];
	CGFloat popupWidth = [interface.entity.desc sizeWithAttributes:attr].width + 40;
	CGFloat popupHeight = 40.0;
	NSPoint popupOrigin = NSMakePoint(roundf(NSMidX(interface.rect) - (popupWidth * 0.5)), roundf(NSMaxY(interface.rect)));
	NSRect popupRect = NSMakeRect(roundf(popupOrigin.x), roundf(popupOrigin.y), popupWidth, popupHeight);
	return popupRect;
}

- (LCPopupView *) popupViewForInterface:(LCVRackInterface *)interface
{
	LCPopupView *view = [[[LCPopupView alloc] initWithFrame:[self popupRectForInterface:interface]] autorelease];
	[view.textField setStringValue:[interface.entity displayString]];
	return view;
}

#pragma mark Interface Methods

- (NSRect) rectForInterface:(LCEntity *)entity
{
	LCVRackInterface *iface;
	for (iface in interfaces)
	{
		if ([iface entity] == entity)
		{
			return [iface rect]; 
		}
	}
	
	return NSMakeRect(0,0,0,0);
}

#pragma mark "Menu Events"

- (NSMenu *) menuForEvent:(NSEvent *)event
{	
	if (hoverInterface) 
	{
		rackController.selectedObject = hoverInterface;
		return [rackController interfaceMenu];
	}
	else return [super menuForEvent:event];
}

#pragma mark Properties

@synthesize hoverInterface;
@synthesize popupView;
@synthesize selectedInterfaceEntity;
- (void) setSelectedInterfaceEntity:(LCEntity *)value
{
	[selectedInterfaceEntity release];
	selectedInterfaceEntity = [value retain];
	[self setNeedsDisplay:YES];
}

@end
