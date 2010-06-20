//
//  LCSSceneOverlayView.m
//  Lithium Console
//
//  Created by James Wilson on 27/08/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCSSceneOverlayView.h"
#import "LCSSceneView.h"
#import "LCEntity.h"
#import "LCEntityDescriptor.h"
#import "LCBrowser2Controller.h"
#import "LCFaultHistoryController.h"
#import "LCMetricHistoryWindowController.h"
#import "LCMetricGraphDocument.h"
#import "LCBrowserSceneContentController.h"
#import "NSGradient-Selection.h"
#import <Quartz/Quartz.h>

@interface LCSSceneOverlayView (Private)
- (void) resetSize;
@end


@implementation LCSSceneOverlayView

#pragma mark "Constructors"

- (LCSSceneOverlayView *) initWithOverlay:(LCSSceneOverlay *)initOverlay inFrame:(NSRect)frame
{
	[super initWithFrame:frame];
	
	/* Load our NIB */
	[NSBundle loadNibNamed:@"OverlayMenu" owner:self];
	
	/* Setup drag'n'drop */
	[self registerForDraggedTypes:[NSArray arrayWithObjects:@"LCEntityDescriptor", nil]];
	
	/* Set overlay */
	overlay = [initOverlay retain];		
	
	/* Set observedEntity */
	if ([overlay entity])
	{
		[self setObservedEntity:[overlay entity]];
	}
	
	/* Create scroll view */
	NSRect scrollFrame = NSMakeRect (15, 15, [self bounds].size.width-30, [self bounds].size.height-30); 
	scrollView = [[LCSSceneScrollerView alloc] initWithOverlay:overlay inFrame:scrollFrame];
	[self addSubview:scrollView];
	
	/* Setup Layer */
	[self setWantsLayer:YES];
	[[self layer] setShadowOpacity:0.6];
	[[self layer] setShadowOffset:CGSizeMake(3.0, -3.0)];
	[[self layer] setShadowRadius:3.0];
	
	/* Resize */
	[self resetSize];
	
	return self;
}

- (void) removeViewAndContent
{
	[self removeFromSuperview];
}

- (void) dealloc
{
	[observedEntity removeObserver:self forKeyPath:@"properties"];
	if (trackingArea)
	{
		[self removeTrackingArea:trackingArea];
		[trackingArea release];
	}
	[overlay release];
	[observedEntity release];
	[super dealloc];
}

#pragma mark Cursor Rectangle

- (void) resetCursorRects
{
	LCSSceneView *sceneView = (LCSSceneView *) [self superview];
	if (sceneView.sceneDocument.editing)
	{
		NSCursor *cursor = [NSCursor pointingHandCursor];
		resizeCursorRect = NSMakeRect([self bounds].size.width-8,0,8,8);
		[self addCursorRect:resizeCursorRect cursor:cursor];
		[cursor setOnMouseEntered:YES];		
	}
	[super resetCursorRects];
}

#pragma mark "Drawing Method"

- (void) drawRect:(NSRect)rect
{	
	NSSize size = [self frame].size;
	
	/* Draw outline */
	NSBezierPath *outlinePath = [self outlinePath:[self bounds]];
	[[NSColor colorWithCalibratedRed:0.8 green:0.8 blue:0.8 alpha:0.8] setStroke];
	[outlinePath stroke];
	
	/* Draw background/name/type */
	NSColor *textColor = [NSColor blackColor];
	if (forceVisible || [[[overlay entity] opstateInteger] intValue] > 0)
	{
		/* If we're forced visible or we have an entity and it's
		 * in a non-normal state, draw a background with entity name,
		 * and type, etc
		 */
		
		/* Draw back */
		[[self overlayBackColor] setFill];
		[outlinePath fill];
		
		/* Set text color */
		textColor = [NSColor whiteColor];		
	}
	
	/* Check for entity */
	if ([overlay entity] && (![overlay autoSize] || ([overlay autoSize] && autoExpanded)))
	{
		/* Device name */
		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
							  textColor, NSForegroundColorAttributeName,
							  [NSFont systemFontOfSize: 8], NSFontAttributeName,
							  nil];
		[[[overlay entity] displayString] drawAtPoint:NSMakePoint(16, size.height-12) withAttributes:attr];
		
		/* Type */
		attr = [NSDictionary dictionaryWithObjectsAndKeys:
				textColor, NSForegroundColorAttributeName,
				[NSFont systemFontOfSize: 6], NSFontAttributeName,
				nil];
		[[[overlay entity] typeString] drawAtPoint:NSMakePoint(8, 4) withAttributes:attr];
		
	}

	/* Selection */
	if (selected)
	{
		NSGradient *selectedGradient = [NSGradient selectionGradientWithAlpha:0.5];
		[selectedGradient drawInBezierPath:outlinePath angle:-90.0];
	}
	
	/* Status Dot */
	NSRect statusRect = NSMakeRect(4, size.height - 12, 8, 8);
	NSBezierPath *statusPath = [NSBezierPath bezierPath];
	[statusPath appendBezierPathWithOvalInRect:statusRect];
	if ([overlay entity])
	{ [[[overlay entity] opStateColor] setFill]; }
	else
	{ [[NSColor grayColor] setFill]; }
	[statusPath fill];
}

#pragma mark "Size Management"

- (void) setFrame:(NSRect)newFrame
{
	[super setFrame:newFrame];
	NSRect scrollFrame = NSMakeRect (15, 15, [self bounds].size.width-30, [self bounds].size.height-30); 	
	[scrollView setFrame:scrollFrame];
}

- (void) resetSize
{
	/* Check size */
	if ([overlay autoSize])
	{
		/* We are an auto-size.. check to see if we should be sized */
		if ((mouseEntered || [[[overlay entity] opstateInteger] intValue] > 0)
				&& (!inMove && !inResize))
		{
			/* Should expand */
			if (!autoExpanded)
			{
				/* We are autosized and need expanding */
				NSRect newFrame = [self frame];
				newFrame.origin.y -= 80;
				newFrame.size.width += 100;
				newFrame.size.height += 80;
				autoExpanded = YES;
				[self setFrame:newFrame];
			}
		}
		else if ((!mouseEntered && [[[overlay entity] opstateInteger] intValue] == 0)
					|| (inMove || inResize))
		{
			/* Should contract */
			if (autoExpanded)
			{
				/* We are auto-sized and need to shrink */
				NSRect newFrame = [self frame];
				newFrame.origin.y += 80;
				newFrame.size.width -= 100;
				newFrame.size.height -= 80;
				autoExpanded = NO;
				[self setFrame:newFrame];
			}
		}
	}				
}

#pragma mark "Outline Path"

- (NSBezierPath *) outlinePath:(NSRect)frame
{
	float arcWidth=15;
	float height = frame.size.height;
	float width = frame.size.width;
	float x = frame.origin.x;
	float y = frame.origin.y;
	NSBezierPath *path = [NSBezierPath bezierPath];
	
	/* Bottom line */
	[path moveToPoint:NSMakePoint(x+arcWidth,y)];
	[path lineToPoint:NSMakePoint(x+(width-arcWidth), y)];
	
	/* Bottom right */
	[path curveToPoint:NSMakePoint(x+width,y+arcWidth) controlPoint1:NSMakePoint(x+width,y) controlPoint2:NSMakePoint(x+width,y)];
	
	/* Re-size bar */
	[path moveToPoint:NSMakePoint(x+(width-8), y)];
	[path lineToPoint:NSMakePoint(x+width, y+8)];
	
	/* Right line */
	[path lineToPoint:NSMakePoint(x+width,y+(height-arcWidth))];
	
	/* Top right */
	[path curveToPoint:NSMakePoint(x+(width-arcWidth), y+height) controlPoint1:NSMakePoint(x+width, y+height) controlPoint2:NSMakePoint(x+width, y+height)];
	
	/* Top line */
	[path lineToPoint:NSMakePoint(x+arcWidth, y+height)];
	
	/* Top leftt */
	[path curveToPoint:NSMakePoint(x, y+(height-arcWidth)) controlPoint1:NSMakePoint(x, y+height) controlPoint2:NSMakePoint(x, y+height)];
	
	/* Left line */
	[path lineToPoint:NSMakePoint(x, y+arcWidth)];
	
	/* Bottom left */
	[path curveToPoint:NSMakePoint(x+arcWidth, y) controlPoint1:NSMakePoint(x, y) controlPoint2:NSMakePoint(x, y)];
	
	return path;
}

#pragma mark "Drag and Drop"

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)info 
{
	/* Check editing */
	LCBrowserSceneContentController *sceneController = [(LCSSceneView *) [self superview] controller];
	if (!sceneController.editing) return NSDragOperationNone;
	
	/* Check for a move operations */
	if ([info draggingSource] == self)
	{
		return NSDragOperationMove; 
	}	
	
    return NSDragOperationCopy;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)info
{
	/* Check editing */
	LCBrowserSceneContentController *sceneController = [(LCSSceneView *) [self superview] controller];
	if (!sceneController.editing) return NO;

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
		
		/* Set entity */
		[overlay setEntity:dropEntity];
		[self setObservedEntity:dropEntity];
		
		/* Refresh entity */
		[[overlay entity] highPriorityRefresh];

		/* Only drop the first */
		break;
	}
	
	[self resetSize];
	[self setNeedsDisplay:YES];
	
    return YES;
}

#pragma mark "KVO"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	[self resetSize];
	[scrollView setNeedsDisplay:YES];
	[self setNeedsDisplay:YES];
}

#pragma mark Mouse Event Handling

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

- (BOOL)acceptsFirstResponder
{
	return YES; 
}

- (void)mouseDown:(NSEvent *)theEvent
{
	LCBrowserSceneContentController *sceneController = [(LCSSceneView *) [self superview] controller];
	sceneController.selectedOverlay = self.overlay;
	
	LCSSceneView *sceneView = (LCSSceneView *) [self superview];	
	if (!sceneView.sceneDocument.editing) return;
	
	/* Check for resize/move */
	if (NSPointInRect([self convertPoint:[theEvent locationInWindow] fromView:nil],resizeCursorRect))
	{
		inResize = YES;
		inMove = NO;
	}
	else
	{
		inResize = NO; 
		inMove = YES;
	}
	
	/* Set initial moveFromPoint */
	moveFromPoint = [theEvent locationInWindow];

	[self setNeedsDisplay:YES];
}

- (void)mouseUp:(NSEvent *)theEvent
{
	LCSSceneView *sceneView = (LCSSceneView *) [self superview];
	if (!sceneView.sceneDocument.editing) return;

	inResize = NO;
	inMove = NO;
	[self setNeedsDisplay:YES];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
	LCSSceneView *sceneView = (LCSSceneView *) [self superview];
	if (!sceneView.sceneDocument.editing) return;

	NSRect newFrame;
	newFrame = [self frame];	

	/* Handle resize or move */
	if (inResize)
	{
		/* Resize */
		newFrame.size.width += [theEvent locationInWindow].x - moveFromPoint.x;
		newFrame.size.height -= [theEvent locationInWindow].y - moveFromPoint.y;
		newFrame.origin.y += [theEvent locationInWindow].y - moveFromPoint.y;
		[overlay setAutoSize:NO];
	}
	else
	{
		/* Move */
		newFrame.origin.x += [theEvent locationInWindow].x - moveFromPoint.x;
		newFrame.origin.y += [theEvent locationInWindow].y - moveFromPoint.y;
	}

	/* Apply changes */
	[overlay setFrame:[(LCSSceneView *)[self superview] restoreOverlayRect:newFrame]];
	[self setFrame:newFrame];
	
	/* Reset point and display */
	moveFromPoint = [theEvent locationInWindow];	
	[self setNeedsDisplay:YES];
	[[self superview] setNeedsDisplay:YES];
}

- (void)mouseEntered:(NSEvent *)theEvent
{
	mouseEntered = true;
	[self resetSize];
	[self setNeedsDisplay:YES];
}

- (void)mouseExited:(NSEvent *)theEvent
{
	mouseEntered = false;
	[self resetSize];
	[self setNeedsDisplay:YES];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
}

#pragma mark "Context Menu Actions"

- (NSMenu *) menuForEvent:(NSEvent *)event
{
	LCBrowserSceneContentController *sceneController = [(LCSSceneView *) [self superview] controller];
	sceneController.selectedOverlay = self.overlay;
	
	return [super menuForEvent:event];
}

- (IBAction) removeOverlayClicked:(id)sender
{
	LCSSceneView *sceneView = (LCSSceneView *) [self superview];
	[sceneView removeOverlay:overlay];
}

- (IBAction) removeEntityClicked:(id)sender
{
	[overlay setEntity:nil];
}

- (IBAction) browseToClicked:(id)sender
{
	[[[LCBrowser2Controller alloc] initWithEntity:[overlay entity]] autorelease];
}

- (IBAction) faultHistoryClicked:(id)sender
{
	[[LCFaultHistoryController alloc] initForEntity:[overlay entity]];
}

- (IBAction) metricHistoryClicked:(id)sender
{
	[[LCMetricHistoryWindowController alloc] initWithMetric:(LCMetric *)[overlay entity]];
}

- (IBAction) graphEntityClicked:(id)sender
{
	LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
	document.initialEntity = [overlay entity];
	[[NSDocumentController sharedDocumentController] addDocument:document];
	[document makeWindowControllers];
	[document showWindows];			
}

#pragma mark Accessors

- (BOOL) forceVisible
{ return forceVisible; }
- (void) setForceVisible:(BOOL)flag
{ 
	forceVisible = flag; 
	[self resetSize];
	[self setNeedsDisplay:YES];
}
- (LCSSceneOverlay *) overlay
{ return overlay; }

- (BOOL) autoExpanded
{ return autoExpanded; }

- (NSColor *) overlayBackColor
{ 
	if (overlayBackColor) return overlayBackColor; 
	else return [LCSSceneOverlayView defaultBackColor];
}
- (void) setOverlayBackColor:(NSColor *)newColor
{ 
	[overlayBackColor release];
	overlayBackColor = [newColor retain];
}
+ (NSColor *) defaultBackColor
{ return [NSColor colorWithDeviceRed:0.1 green:0.1 blue:0.1 alpha:0.8]; }

- (LCEntity *) observedEntity
{
	return observedEntity; 
}
- (void) setObservedEntity:(LCEntity *)newEntity
{
	[observedEntity removeObserver:self forKeyPath:@"properties"];
	[observedEntity release];
	observedEntity = [newEntity retain];
	[observedEntity addObserver:self 
			   forKeyPath:@"properties"
				  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
				  context:NULL];	
}

@synthesize overlay;
@synthesize mouseEntered;
@synthesize autoExpanded;
@synthesize inResize;
@synthesize inMove;
@synthesize scrollView;
@synthesize selected;

@end
