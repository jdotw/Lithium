//
//  LCSSceneView.m
//  Lithium Console
//
//  Created by James Wilson on 27/08/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCSSceneView.h"
#import "LCSSceneOverlayView.h"
#import "LCEntityDescriptor.h"

@interface LCSSceneView (private)

- (void) resetGeometry;
- (NSRect) translateOverlayRect:(NSRect)original;
- (NSRect) restoreOverlayRect:(NSRect)translated;

@end

@implementation LCSSceneView

#pragma mark "Constructors"

- (id) initWithFrame:(NSRect)frame
{
	[super initWithFrame:frame];

	/* Setup drag'n'drop */
	[self registerForDraggedTypes:[NSArray arrayWithObjects:@"LCEntityDescriptor", NSFilenamesPboardType, nil]];

	/* Create views array */
	overlayViews = [[NSMutableArray array] retain];
	overlayViewDict = [[NSMutableDictionary dictionary] retain];
	
	return self;
}

- (void) dealloc
{
	[sceneDocument removeObserver:self forKeyPath:@"backgroundImage"];
	[sceneDocument removeObserver:self forKeyPath:@"editing"];
	[sceneDocument removeObserver:self forKeyPath:@"overlays"];
	[sceneDocument removeObserver:self forKeyPath:@"xmlOperationInProgress"];
	[sceneDocument release];
	[overlayViews release];
	[super dealloc];
}

#pragma mark "Geometry Methods"

- (void) updateTrackingAreas
{
	[super updateTrackingAreas];
	
	if (trackingArea)
	{
		[self removeTrackingArea:trackingArea];
		[trackingArea release];
	}
	trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
												options:NSTrackingMouseEnteredAndExited|NSTrackingMouseMoved|NSTrackingActiveInActiveApp
												  owner:self
											   userInfo:nil];
	[self addTrackingArea:trackingArea];
}

- (void) setFrame:(NSRect)frame
{
	[super setFrame:frame];
	[self resetGeometry];
}

- (void) resetGeometry
{
	/* 
	 * Calculates the rect for the image 
	 */
	
	/* Load image (default to blank) */
	NSImage *image = sceneDocument.backgroundImage;
	if (!image)
	{ image = [NSImage imageNamed:@"statesceneblank.png"]; }
	
	/* Scale dimensions */
	imageRect = NSMakeRect(0.0, 0.0, [image size].width, [image size].height);
	if (NSWidth(imageRect) > NSWidth([self bounds]))
	{
		float scale = NSWidth([self bounds]) / NSWidth(imageRect);
		imageRect.size.width = roundf(imageRect.size.width * scale);
		imageRect.size.height = roundf(imageRect.size.height * scale);
	}
	if (NSHeight(imageRect) > NSHeight([self bounds]))
	{
		float scale = NSHeight([self bounds]) / NSHeight(imageRect);
		imageRect.size.width = roundf(imageRect.size.width * scale);
		imageRect.size.height = roundf(imageRect.size.height * scale);
	}
	
	/* Center */
	if (NSWidth(imageRect) < NSWidth([self bounds]))
	{
		imageRect.origin.x = roundf(NSMidX([self bounds]) - (NSWidth(imageRect) * 0.5));
	}		
	if (NSHeight(imageRect) < NSHeight([self bounds]))
	{
		imageRect.origin.y = roundf(NSMidY([self bounds]) - (NSHeight(imageRect) * 0.5));
	}	
	
	/* Scale */
	xScale = NSWidth(imageRect) / [image size].width;
	yScale= NSHeight(imageRect) / [image size].height;
	
	/* Reset overlays */
	[self resetOverlayViews];
	
	/* Redraw */
	[self setNeedsDisplay:YES];
}

- (NSRect) translateOverlayRect:(NSRect)original
{
	/* Returns the overlay rect in terms of the
	 * scaled backgroundImage rect
	 */
	
	return NSMakeRect(roundf(NSMinX(imageRect) + (NSMinX(original) * xScale)),
					  roundf(NSMinY(imageRect) + (NSMinY(original) * yScale)), 
					  roundf(NSWidth(original) * xScale),
					  roundf(NSHeight(original) * yScale));
}

- (NSRect) restoreOverlayRect:(NSRect)translated
{
	/* Returns the original overlay rect in 
	 * terms of the background image
	 */
	
	return NSMakeRect((NSMinX(translated) - NSMinX(imageRect)) * (1.0 / xScale),
					  (NSMinY(translated) - NSMinY(imageRect))  * (1.0 / yScale), 
					  NSWidth(translated) * (1.0 / xScale),
					  NSHeight(translated) * (1.0 / yScale));
}

#pragma mark "Drawing Method"

- (void) drawRect:(NSRect)rect
{
	/* Initialise graphics */
	[super drawRect:rect];
	
	/* Draw image */
	if ([sceneDocument backgroundImage])
	{
		[[sceneDocument backgroundImage] drawInRect:imageRect
						   fromRect:NSMakeRect(0,0,[[sceneDocument backgroundImage] size].width,[[sceneDocument backgroundImage] size].height)
						  operation:NSCompositeSourceOver
						   fraction:1.0];
	}
	else
	{
		/* Place-holder */
		CGFloat lineWidth = 6.0;
		NSRect outlineBounds = NSMakeRect(0.0, 0.0, 520.0, 360.0);
		NSRect outlineRect = NSMakeRect(NSMidX([self bounds]) - NSMidX(outlineBounds), NSMidY([self bounds]) - NSMidY(outlineBounds), 
										NSWidth(outlineBounds), NSHeight(outlineBounds));
		CGFloat patternArray[2] = { 24.0, 8.0 };

		/* Draw under-coat */
		NSBezierPath *outlinePath = [NSBezierPath bezierPathWithRoundedRect:outlineRect
																	xRadius:10.0
																	yRadius:10.0];
		[outlinePath setLineDash:patternArray 
						   count:2
						   phase:10];
		[outlinePath setLineWidth:lineWidth];
		[[NSColor colorWithCalibratedWhite:1.0 alpha:0.3] setStroke];
		[outlinePath stroke];
		
		/* Draw over-coat */
		outlineRect.origin.x = outlineRect.origin.x + 0.0;
		outlineRect.origin.y = outlineRect.origin.y + 1.0;
		outlinePath = [NSBezierPath bezierPathWithRoundedRect:outlineRect
													  xRadius:10.0
													  yRadius:10.0];
		[outlinePath setLineDash:patternArray 
						   count:2
						   phase:10];
		[outlinePath setLineWidth:lineWidth];
		[[NSColor colorWithCalibratedWhite:0.0 alpha:0.7] setStroke];
		[outlinePath stroke];
		
		/* Draw Text */
		CGFloat fontSize = 20.0;
		NSString *statusString = nil;
		NSMutableDictionary *attr = [NSMutableDictionary dictionaryWithObjectsAndKeys:
									 [NSColor colorWithCalibratedWhite:1.0 alpha:0.2], NSForegroundColorAttributeName,
									 [NSFont boldSystemFontOfSize:fontSize], NSFontAttributeName,
									 nil];		
		
		if (sceneDocument.xmlOperationInProgress) {
			statusString = @"Loading Scene...";
		}
		else {
			if (sceneDocument.editing) { 
				statusString = @"Drop Background Image Here";
			}
			else {
				statusString = @"Click 'Edit' to Set Background and Overlays";
			}			
		}

		/* Draw under coat */
		NSPoint textPoint = NSMakePoint(NSMidX(outlineRect) - ([statusString sizeWithAttributes:attr].width * 0.5), 
									   NSMidY(outlineRect) - (fontSize * 0.5));
		[statusString drawAtPoint:textPoint withAttributes:attr];
		
		/* Draw over-coat */
		textPoint.y = textPoint.y + 1.0;
		[attr setObject:[NSColor colorWithCalibratedWhite:0.0 alpha:0.7] forKey:NSForegroundColorAttributeName];
		[statusString drawAtPoint:textPoint withAttributes:attr];
		
	}

	/* Draw the drawBox */
	if (drawBox)
	{
		NSPoint drawBoxPoint = [self drawBoxStartPoint];
		NSSize drawBoxSize = [self drawBoxSize];
		[[NSColor colorWithCalibratedRed:0.2 green:0.2 blue:0.2 alpha:0.8] setFill];
		NSBezierPath *path = [NSBezierPath bezierPathWithRect:NSMakeRect(drawBoxPoint.x,drawBoxPoint.y,drawBoxSize.width,drawBoxSize.height)];
		NSShadow* theShadow = [[NSShadow alloc] init]; 
		[theShadow setShadowOffset:NSMakeSize(4.0, -4.0)]; 
		[theShadow setShadowBlurRadius:3.0];
		[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.4]];
		[theShadow set];
		[path fill];
		[theShadow release];
	}
	
	/* Overlay shadows */
	LCSSceneOverlayView *overlay;
	for (overlay in overlayViews)
	{
		if ([[overlay overlay] autoSize] && [overlay autoExpanded]) continue; 
		NSShadow* theShadow = [[NSShadow alloc] init]; 
		[theShadow setShadowOffset:NSMakeSize(4.0, -4.0)]; 
		[theShadow setShadowBlurRadius:4.0];
		[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.2]];
		[theShadow set];
		[[NSColor colorWithDeviceRed:0.0 green:0.0 blue:0.0 alpha:0.08] setFill];
		NSBezierPath *path = [overlay outlinePath:[overlay frame]];
		[path fill];
		[theShadow release]; 		
	}	
}

#pragma mark "DrawBox Methods"

- (NSPoint) drawBoxStartPoint
{
	float x;
	float y;
	
	if (drawBoxMouseCurrent.x <= drawBoxMouseStart.x) x = drawBoxMouseCurrent.x;
	else x = drawBoxMouseStart.x;
	if (drawBoxMouseCurrent.y <= drawBoxMouseStart.y) y = drawBoxMouseCurrent.y;
	else y = drawBoxMouseStart.y;
	
	return NSMakePoint(x,y);
}

- (NSSize) drawBoxSize
{
	float x;
	float y;
	NSPoint startPoint = [self drawBoxStartPoint];
	
	if (drawBoxMouseCurrent.x >= drawBoxMouseStart.x) x = drawBoxMouseCurrent.x;
	else x = drawBoxMouseStart.x;
	if (drawBoxMouseCurrent.y >= drawBoxMouseStart.y) y = drawBoxMouseCurrent.y;
	else y = drawBoxMouseStart.y;

	return NSMakeSize(x - startPoint.x,y - startPoint.y);
}

#pragma mark "KVO"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	if ([keyPath isEqualToString:@"overlays"])
	{
		/* Reset views */
		[self resetOverlayViews];
	}
	else if ([keyPath isEqualToString:@"editing"])
	{
		/* Reset cursor rects */
		[[self window] invalidateCursorRectsForView:self];
	}
	else if ([keyPath isEqualToString:@"backgroundImage"])
	{
		/* Reset views */
		[self resetGeometry];
	}
	else if ([keyPath isEqualToString:@"xmlOperationInProgress"])
	{
		/* Redraw */
		[self setNeedsDisplay:YES];
	}
}

#pragma mark Overlay Management

- (void) removeOverlay:(LCSSceneOverlay *)overlay
{
	/* Removes the specified overlay */
	if ([[sceneDocument overlays] containsObject:overlay])
	{
		[sceneDocument removeObjectFromOverlaysAtIndex:[[sceneDocument overlays] indexOfObject:overlay]];
	}
}

- (void) resetOverlayViews
{
	/* Remove all existing views */
	LCSSceneOverlayView *overlayView;
	for (overlayView in overlayViews)
	{
		[overlayView removeViewAndContent];
	}
	[overlayViews removeAllObjects];
	[overlayViewDict removeAllObjects];
	
	/* Create new */
	for (LCSSceneOverlay *overlay in sceneDocument.overlays)
	{
		overlayView = (LCSSceneOverlayView *) [[LCSSceneOverlayView alloc] initWithOverlay:overlay inFrame:[self translateOverlayRect:[overlay frame]]];
		[self addSubview:overlayView];
		[overlayView setForceVisible:forceAllViewsVisible];
		[overlayView setNeedsDisplay:YES];
		[overlayViews addObject:overlayView];
		[overlayViewDict setObject:overlayView forKey:overlay.uuid];
	}
	
	/* Redraw */
	[self setNeedsDisplay:YES];
}

- (bool) forceAllViewsVisible
{ return forceAllViewsVisible; }

- (void) setForceAllViewsVisible:(BOOL)flag
{
	forceAllViewsVisible = flag;
	LCSSceneOverlayView *overlayView;
	for (overlayView in overlayViews)
	{ [overlayView setForceVisible:flag]; }
}

- (void) setForceUnboundVisible:(BOOL)flag
{
	LCSSceneOverlayView *overlayView;
	for (overlayView in overlayViews)
	{ 
		if (![[overlayView overlay] entity])
		{
			if (flag)
			{ [overlayView setOverlayBackColor:[NSColor colorWithDeviceRed:0.9 green:0.9 blue:1.0 alpha:0.8]]; }
			else
			{ [overlayView setOverlayBackColor:[LCSSceneOverlayView defaultBackColor]]; }
			[overlayView setForceVisible:flag]; 
		}
	}
}

#pragma mark Cursor Rectangle

- (void) resetCursorRects
{
	if (sceneDocument.editing)
	{
		NSCursor *cursor = [NSCursor crosshairCursor];
		NSRect resizeCursorRect = [self visibleRect];
		[self addCursorRect:resizeCursorRect cursor:cursor];
		[cursor setOnMouseEntered:YES];		
	}
	[super resetCursorRects];
}

#pragma mark Mouse Event Handling

- (BOOL)acceptsFirstResponder
{
	return YES; 
}

- (void)mouseDown:(NSEvent *)theEvent
{
	if (!sceneDocument.editing) return;

	drawBox = true;
	drawBoxMouseStart = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	drawBoxMouseCurrent = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	if ([theEvent clickCount] == 2)
	{
		/* Double click, add an overlay */
		NSPoint location = [self drawBoxStartPoint];
		location.x -= 0.5 * [LCSSceneOverlay minimumSize].width;
		location.y -= 0.5 * [LCSSceneOverlay minimumSize].height;
		NSRect overlayFrame = NSMakeRect(location.x,location.y,[LCSSceneOverlay minimumSize].width,[LCSSceneOverlay minimumSize].height);
		LCSSceneOverlay *overlay = [LCSSceneOverlay overlayWithFrame:[self restoreOverlayRect:overlayFrame]];
		[overlay setAutoSize:YES];
		[sceneDocument insertObject:overlay inOverlaysAtIndex:0];
	}
	[self setNeedsDisplay:YES];
}

- (void)mouseUp:(NSEvent *)theEvent
{
	if (!sceneDocument.editing) return;

	drawBox = false;
	
	/* Check to see if something was drawn */
	NSSize drawSize = [self drawBoxSize];
	if (drawSize.height >= 10 && drawSize.width >= 10)
	{
		/* Add an overlay */
		NSRect translatedFrame = NSMakeRect([self drawBoxStartPoint].x,[self drawBoxStartPoint].y,[self drawBoxSize].width,[self drawBoxSize].height);
		NSRect overlayFrame = [self restoreOverlayRect:translatedFrame];
		LCSSceneOverlay *overlay = [LCSSceneOverlay overlayWithFrame:overlayFrame];
		[sceneDocument insertObject:overlay inOverlaysAtIndex:0];
	}
	
	[self setNeedsDisplay:YES];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
	if (!sceneDocument.editing) return;

	drawBoxMouseCurrent = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	[self setNeedsDisplay:YES];
}

- (void)mouseEntered:(NSEvent *)theEvent
{
	mouseEntered = true;
	[[self window] makeFirstResponder:self];
}

- (void)mouseExited:(NSEvent *)theEvent
{
	mouseEntered = false;
}

- (void)mouseMoved:(NSEvent *)theEvent
{
}

#pragma mark "Drag and Drop"

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)info 
{
	if (!sceneDocument.editing) return NSDragOperationNone;

	[self setForceUnboundVisible:YES];
	if ([[[info draggingPasteboard] types] containsObject:NSFilenamesPboardType] || [[[info draggingPasteboard] types] containsObject:NSTIFFPboardType])
	{ return NSDragOperationCopy; }
	else
	{ return NSDragOperationCopy; }
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)info
{
	if (!sceneDocument.editing) return NO;

	/* Get Files */
	NSArray *plist = [[info draggingPasteboard] propertyListForType:NSFilenamesPboardType];
	for (id item in plist)
	{
		[sceneDocument setBackgroundImage:[[[NSImage alloc] initWithContentsOfFile:item] autorelease]];
	}

	/* Get Entities */
	NSArray *entityDescriptorArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
	NSDictionary *entityDescriptorProperties;
	for (entityDescriptorProperties in entityDescriptorArray)
	{
		/* Locate entity */
		LCEntityDescriptor *dropEntityDescriptor = [LCEntityDescriptor descriptorWithProperties:entityDescriptorProperties];
		LCEntity *dropEntity = [dropEntityDescriptor locateEntity:YES];
		if (!dropEntity) continue;
		
		/* Check entity type */
		if ([[dropEntity typeInteger] intValue] == 7) 
		{
			/* A trigger was dropped, use the object instead */
			dropEntity = [dropEntity object];
		}

		/* Add an overlay */
		NSPoint location = [self convertPoint:[info draggingLocation] fromView:nil];
		location.x -= 0.5 * [LCSSceneOverlay minimumSize].width;
		location.y -= 0.5 * [LCSSceneOverlay minimumSize].height;
		NSRect overlayFrame = NSMakeRect(location.x,location.y,[LCSSceneOverlay minimumSize].width,[LCSSceneOverlay minimumSize].height);
		LCSSceneOverlay *overlay = [LCSSceneOverlay overlayWithFrame:overlayFrame];
		[overlay setAutoSize:YES];
		[overlay setEntity:dropEntity];
		[sceneDocument insertObject:overlay inOverlaysAtIndex:0];
		[[overlay entity] highPriorityRefresh];
		
		/* Only drop the first */
		break;
	}
	
	
	[self setNeedsDisplay:YES];
	
    return YES;
}

- (void)draggingExited:(id <NSDraggingInfo>)sender
{
	[self setForceUnboundVisible:NO];	
}

#pragma mark "Properties"

@synthesize sceneDocument;
- (void) setSceneDocument:(LCSSceneDocument *)newDocument
{
	[sceneDocument removeObserver:self forKeyPath:@"backgroundImage"];
	[sceneDocument removeObserver:self forKeyPath:@"editing"];
	[sceneDocument removeObserver:self forKeyPath:@"overlays"];
	[sceneDocument removeObserver:self forKeyPath:@"xmlOperationInProgress"];
	[sceneDocument release];
	sceneDocument = [newDocument retain];
	[sceneDocument addObserver:self 
			   forKeyPath:@"backgroundImage"
				  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
				  context:NULL];	
	[sceneDocument addObserver:self 
			   forKeyPath:@"overlays"
				  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
				  context:NULL];		
	[sceneDocument addObserver:self 
					forKeyPath:@"editing"
					   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
					   context:NULL];
	[sceneDocument addObserver:self 
					forKeyPath:@"xmlOperationInProgress"
					   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
					   context:NULL];		
	[self resetOverlayViews];
}

@synthesize controller;

@synthesize overlayViews;
@synthesize overlayViewDict;

@end
