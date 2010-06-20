//
//  LCVRackCableView.m
//  Lithium Console
//
//  Created by James Wilson on 11/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCVRackCableView.h"
#import "LCVRackCable.h"
#import "LCVRackInterface.h"
#import "LCVRackDeviceBackView.h"
#import "LCBrowserVRackContentController.h"
#import "LCVRackCablePath.h"
#import "LCBrowserVRackContentController.h"

#define kPopupWidth 200.0
#define kPopupHeight 106.0

@implementation LCVRackCableView

#pragma mark Constructors

- (id)initWithFrame:(NSRect)initFrame 
{
    [super initWithFrame:initFrame];

	paths = [[NSMutableArray array] retain];

    return self;
}

- (void) dealloc
{
	[paths release];
	[super dealloc];
}

#pragma mark "View Management"

- (void) removeFromSuperview
{
	/* Remove observer */
	LCVRackCablePath *path;
	for (path in paths)
	{
		[[path cable] removeObserver:self forKeyPath:@"colour"];
	}
	[animationTimer invalidate];
	[self setRackController:nil];
	[self setHitView:nil];
	[cablePopup removeFromSuperview];
	[cablePopup release];
	cablePopup = nil;
	[popupTimer invalidate];
	[super removeFromSuperview];
}

#pragma mark "Drawing"

- (void)drawRect:(NSRect)rect 
{
	/* Calculate cable width */
	float cableWidth = 4.0 * (NSWidth([self bounds]) / 304);
	
	/* Check if paths are valid */
	if (pathsValid)
	{
		/* Draw cables from established paths */
		LCVRackCablePath *path;
		for (path in paths)
		{			
			[[path path] setLineWidth:cableWidth];
			[[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:[path cableAlpha]] setStroke];
			[[path path] stroke];

			NSShadow* theShadow = [[NSShadow alloc] init]; 
			[theShadow setShadowOffset:NSMakeSize(2.0, -2.0)]; 
			[theShadow setShadowBlurRadius:cableWidth*0.9];
			[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.5]];
			[theShadow set];
			
			[[path path] setLineWidth:cableWidth*0.8];
			[[path cableColor] setStroke];
			[[path path] stroke];					
			
			[theShadow release];

			/* 
			 * Speed indicators 
			 */
			LCVRackCable *cable = [path cable];
			
			/* Get values */
			NSString *aEndInUtil = [[cable aEndInterface] valueForKeyPath:@"childrenDictionary.utilpc_in.properties.valstr_raw"];
			NSString *aEndOutUtil = [[cable aEndInterface] valueForKeyPath:@"childrenDictionary.utilpc_out.properties.valstr_raw"];
			NSString *bEndInUtil = [[cable bEndInterface] valueForKeyPath:@"childrenDictionary.utilpc_in.properties.valstr_raw"];
			NSString *bEndOutUtil = [[cable bEndInterface] valueForKeyPath:@"childrenDictionary.utilpc_out.properties.valstr_raw"];
			NSString *aEndInSpeed = [[cable aEndInterface] valueForKeyPath:@"childrenDictionary.speed.properties.valstr_raw"];
			NSString *aEndOutSpeed = [[cable aEndInterface] valueForKeyPath:@"childrenDictionary.speed.properties.valstr_raw"];
			NSString *bEndInSpeed = [[cable bEndInterface] valueForKeyPath:@"childrenDictionary.speed.properties.valstr_raw"];
			NSString *bEndOutSpeed = [[cable bEndInterface] valueForKeyPath:@"childrenDictionary.speed.properties.valstr_raw"];
			
			/* Get directional utilisation values */
			NSString *abUtil;
			if ([aEndOutUtil floatValue] > [bEndInUtil floatValue])
			{ abUtil = aEndOutUtil; }
			else
			{ abUtil = bEndInUtil; }
			NSString *baUtil;
			if ([aEndInUtil floatValue] > [bEndOutUtil floatValue])
			{ baUtil = aEndInUtil; }
			else
			{ baUtil = bEndOutUtil; }			
			
			/* Get directional speed values */
			NSString *abSpeed;
			if ([aEndInSpeed floatValue] > [bEndOutSpeed floatValue])
			{ abSpeed = aEndInSpeed; }
			else
			{ abSpeed = bEndOutSpeed; }
			NSString *baSpeed;
			if ([aEndOutSpeed floatValue] > [bEndInSpeed floatValue])
			{ baSpeed = aEndOutSpeed; }
			else
			{ baSpeed = bEndInSpeed; }
			
			/* Draw a->b indicators */
			if (abSpeed && [abSpeed intValue] > 0 && ([abSpeed rangeOfString:@"ase"].location == NSNotFound))
			{ [cable setAbSpeedIndex:([abSpeed intValue] / 1000000)+10]; }
			else
			{ 
				[cable setAbSpeedIndex:100]; 
			}
			[cable incrementAbSpeedIndicatorT];
			[self drawThroughputIndicatorsForCable:cable onPath:path speed:abSpeed util:abUtil speedIndicatorT:[cable abSpeedIndicatorT] cableColor:[path cableColor] xOffset:1.5*(cableWidth/4.0) yOffset:1.5*(cableWidth/4.0) cableWidth:cableWidth];
			
			/* Draw b->a indicators */
			if (baSpeed && [baSpeed intValue] > 0 && ([baSpeed rangeOfString:@"ase"].location == NSNotFound))
			{ [cable setBaSpeedIndex:([baSpeed intValue] / 1000000)+10]; }
			else
			{ 
				[cable setBaSpeedIndex:100]; 
			}
			[cable incrementBaSpeedIndicatorT];
			
			[self drawThroughputIndicatorsForCable:cable onPath:path speed:baSpeed util:baUtil speedIndicatorT:1-[cable baSpeedIndicatorT] cableColor:[path cableColor] xOffset:0.5*(cableWidth/4.0) yOffset:0.5*(cableWidth/4.0) cableWidth:cableWidth];
		}
		
		/* Draw partial cable */
		LCEntity *nominatedA = [rackController cableEndPointA];
		if (nominatedA)
		{
			LCVRackDeviceBackView *devView = [rackController deviceBackViewForEntity:[nominatedA device]];
			NSRect ifaceRect = [devView rectForInterface:nominatedA];
			NSPoint ifacePoint = NSMakePoint(ifaceRect.origin.x + (0.5 * ifaceRect.size.width), ifaceRect.origin.y + (0.5 * ifaceRect.size.height));
			NSBezierPath *partialPath = [NSBezierPath bezierPath];
			[partialPath moveToPoint:[self convertPoint:ifacePoint fromView:devView]];
			[partialPath curveToPoint:[self convertPoint:[rackController currentMouseLocationInWindow] fromView:nil] controlPoint1:ifacePoint controlPoint2:ifacePoint];
			[[NSColor whiteColor] setStroke];
			NSShadow* theShadow = [[NSShadow alloc] init]; 
			[theShadow setShadowOffset:NSMakeSize(10.0, -10.0)]; 
			[theShadow setShadowBlurRadius:3.0];		
			[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.3]];
			[theShadow set];
			[partialPath stroke];
			[theShadow release]; 		
		}		
	}
	else
	{
		/* Paths are not valid, re-create them */
		/* Remove old observers */
		LCVRackCablePath *path;
		for (path in paths)
		{
			[[path cable] removeObserver:self forKeyPath:@"colour"];
		}
		
		/* CLear out paths if necessary */
		[paths removeAllObjects];		
			
		/* Draw cables */
		NSEnumerator *cableEnum = [[[rackController rack] cables] objectEnumerator];
		LCVRackCable *cable;
		while ((cable = [cableEnum nextObject]) != nil)
		{			
			/* Paths are not valid, we're re-creating all cable paths */
			/* Get end-point entities */
			LCEntity *ifaceA = [cable aEndInterface];
			LCEntity *ifaceB = [cable bEndInterface];
			if (!ifaceA || !ifaceB) 
			{
				[cable setOrphaned:YES];
				continue;
			}
			
			/* Get device view for interfaces */
			LCVRackDeviceBackView *devA = (LCVRackDeviceBackView *) [rackController deviceBackViewForEntity:[ifaceA device]];
			LCVRackDeviceBackView *devB = (LCVRackDeviceBackView *) [rackController deviceBackViewForEntity:[ifaceB device]];
			if (!devA || !devB) 
			{
				[cable setOrphaned:YES];
				continue;
			}

			/* Get iface rects */
			NSRect ifaceArect = [devA rectForInterface:ifaceA];
			NSRect ifaceBrect = [devB rectForInterface:ifaceB];
			
			/* Find iface points */
			NSPoint ifaceApoint = NSMakePoint(ifaceArect.origin.x + (0.5 * ifaceArect.size.width), ifaceArect.origin.y + (0.5 * ifaceArect.size.height));
			NSPoint ifaceBpoint = NSMakePoint(ifaceBrect.origin.x + (0.5 * ifaceBrect.size.width), ifaceBrect.origin.y + (0.5 * ifaceBrect.size.height));
			NSPoint startPoint = [devA convertPoint:ifaceApoint toView:self];
			NSPoint endPoint = [devB convertPoint:ifaceBpoint toView:self];

			/* Locate left/right upper/lower points */
			NSPoint leftPoint;
			NSPoint rightPoint;
			if (startPoint.x >= endPoint.x)
			{
				leftPoint = endPoint;
				rightPoint = startPoint;
			}
			else
			{
				leftPoint = startPoint;
				rightPoint = endPoint;
			}
			NSPoint upperPoint;
			NSPoint lowerPoint;
			if (startPoint.y >= endPoint.y)
			{
				upperPoint = startPoint;
				lowerPoint = endPoint;
			}
			else
			{
				upperPoint = endPoint;
				lowerPoint = startPoint;
			}
			
			/* Create control point */
			NSPoint cp1;
			cp1 = NSMakePoint(leftPoint.x, lowerPoint.y + ((upperPoint.y - lowerPoint.y)/2));
			
			/* Check for isolation */
			BOOL cableIsolated = YES;
			float cableAlpha = 0.8;
			if ([rackController cableHighlightDevice])
			{
				if ([[cable aEndInterface] device] == [rackController cableHighlightDevice] || [[cable bEndInterface] device] == [rackController cableHighlightDevice])
				{
					/* Isolate this cable */
					cableAlpha = 0.8;
					cableIsolated = YES;
				}
				else
				{
					/* Shadow this cable */
					cableAlpha = 0.1;
					cableIsolated = NO;
				}
			}

			/* Create path */
			NSBezierPath *cablePath = [NSBezierPath bezierPath];
			[cablePath setLineWidth:cableWidth];
			[cablePath moveToPoint:startPoint];
			[cablePath curveToPoint:endPoint controlPoint1:cp1 controlPoint2:cp1];
			[cablePath setLineCapStyle:NSRoundLineCapStyle];
//			[[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:cableAlpha] setStroke];
//			NSShadow* theShadow = [[NSShadow alloc] init]; 
//			[theShadow setShadowOffset:NSMakeSize(2.0, -2.0)]; 
//			[theShadow setShadowBlurRadius:cableWidth*0.9];
//			[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.02]];
//			[theShadow set];
//			[cablePath stroke];		
//			[theShadow release]; 
//			[cablePath setLineWidth:cableWidth*0.8];
//			[cablePath moveToPoint:startPoint];
//			[cablePath curveToPoint:endPoint controlPoint1:cp1 controlPoint2:cp1];
			NSColor *cableColor;
			if ([rackController showCableStatus])
			{ cableColor = [cable opStateColourWithAlpha:cableAlpha]; }
			else
			{ cableColor = [cable colourWithAlpha:cableAlpha]; }						
//			[cableColor setStroke];
//			[cablePath stroke];		
			
			/* Set flag */
			[cable setOrphaned:NO];
			
			/* Create path object */
			LCVRackCablePath *path = [[[LCVRackCablePath alloc] initWithStartPoint:startPoint endPoint:endPoint path:cablePath cable:cable] autorelease];
			[path setControlPoint1:cp1];
			[path setControlPoint2:cp1];
			[path setCableColor:cableColor];
			[path setCableAlpha:cableAlpha];
			[paths addObject:path];
			
			/* Add observer */
			[[path cable] addObserver:self forKeyPath:@"colour" options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) context:nil];
		}
		
		/* Set valid flag */
		[self setPathsValid:YES];

		/* Recursively call to draw paths etc */
		[self drawRect:rect];
	}

}

- (void) drawThroughputIndicatorsForCable:(LCVRackCable *)cable 
								   onPath:(id)path 
									speed:(NSString *)speed 
									 util:(NSString *)util 
						  speedIndicatorT:(float)speedIndicatorT 
							   cableColor:(NSColor *)cableColor 
								  xOffset:(float)xOffset 
								  yOffset:(float)yOffset
							   cableWidth:(float)cableWidth
{
	float dotDiam = cableWidth / 2.0;
	
	/* Get length */
	NSBezierPath *flatPath = [[(LCVRackCablePath *)path path] bezierPathByFlatteningPath];
	float length = [flatPath bounds].size.height;
	
	/* Get max dots */
	float maxDots = length / dotDiam;		
	
	/* Get pilot point */
//	NSPoint pilotPoint = [path pointOnPathAt:speedIndicatorT];			/* This is the 'pilot' point... */
	int pointsToDraw = maxDots * ([util floatValue] / 100.0);			/* This is the number of points to draw to represent the utilisaton */	
	if (pointsToDraw == 0 && [util floatValue] > 1) pointsToDraw = 1;	/* Keep 1 dot for util >1 */
	
	if (pointsToDraw > 0)
	{
		/* So at this point, we know the 'pilot' point and the number
		* of points to draw based on the utilisation to be shown. 
		* Basically we want to draw the pilot point in its correct
		* spot, and then have pointsToDraw-1 number of equidistant points
		* also drawn on the line
		*/
		float tIncrement = 1.0 / pointsToDraw;								/* This is how much to incrmenet T between each dot */
		
		/* Now we need to work out the initial offset before we draw the first dot */
		float intrg;
		float tq = speedIndicatorT / tIncrement;
		float fract = modff (tq, &intrg);
		float tOffset = tIncrement * fract;
		
		/* Now we can draw */
		float t = tOffset;
		int dotIndex = 0;
		while (t <= 1)
		{
			NSPoint point = [path pointOnPathAt:t];
			NSRect indicator = NSMakeRect(point.x-xOffset,point.y-yOffset,dotDiam,dotDiam);
			NSBezierPath *indicatorPath = [NSBezierPath bezierPathWithOvalInRect:indicator];
			NSColor *indicColor = [NSColor colorWithDeviceRed:[cableColor redComponent]*1.8 green:[cableColor greenComponent]*1.8 blue:[cableColor blueComponent]*1.8 alpha:[path cableAlpha]*1.5];
			[indicColor setFill];
			[indicatorPath fill];
			dotIndex++;
			t = tOffset + (dotIndex * tIncrement);												/* Advance to next dot position */
		}
	}			
}

#pragma mark "KVO Event Handling"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	[self setPathsValid:NO];
	[self setNeedsDisplay:YES];
}	

#pragma mark "Mouse Event Handlers"

- (void) updateTrackingAreas
{
	if (trackingArea)
	{
		[self removeTrackingArea:trackingArea];
		[trackingArea release];
	}
	
	trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
												 options:NSTrackingActiveInActiveApp|NSTrackingMouseEnteredAndExited|NSTrackingMouseMoved
												   owner:self
												userInfo:nil];
	[self addTrackingArea:trackingArea];
}

- (NSView *)hitTest:(NSPoint)aPoint
{
	/* Find a cable */
	if ([self cableAtPoint:aPoint]) 
	{
		LCBrowserVRackContentController *contentController = rackController;
		contentController.selectedObject = [self cableAtPoint:aPoint];
		return self;
	}
	
	/* Find clicked device */
	NSEnumerator *devViewEnum = [[rackController devBackViews] objectEnumerator];
	LCVRackDeviceBackView *devView;
	while ((devView=[devViewEnum nextObject])!=nil)
	{
		NSView *view = [devView hitTest:aPoint];
		if (view) return view;
	}
	
	return hitView;
}

- (void) mouseMoved:(NSEvent *)theEvent
{
	popupPoint = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	LCVRackCable *cable = [self cableAtPoint:popupPoint];
	if (cable != hoverCable)
	{
		/* Clear old */
		[cablePopup removeFromSuperview];
		[cablePopup release];
		cablePopup = nil;
		
		/* Set new timer */
		self.hoverCable = cable;
		[popupTimer invalidate];
		popupTimer = nil;
		if (cable)
		{
			popupTimer = [NSTimer scheduledTimerWithTimeInterval:0.5
														  target:self
														selector:@selector(popupTimerCallback:)
														userInfo:nil
														 repeats:NO];
		}
	}
	else
	{
		/* Move popup */
		LCBrowserVRackContentController *controller = (LCBrowserVRackContentController *)rackController;
		NSPoint insetViewPoint = [controller.insetView convertPoint:popupPoint fromView:self];
		[cablePopup setFrame:NSMakeRect(roundf(insetViewPoint.x - (kPopupWidth * 0.5)), insetViewPoint.y, kPopupWidth, kPopupHeight)];
	}		
}

- (void) popupTimerCallback:(NSTimer *)timer
{	
	LCBrowserVRackContentController *controller = (LCBrowserVRackContentController *)rackController;
	
	/* Cable Popup at mouse location */
	NSPoint insetViewPoint = [controller.insetView convertPoint:popupPoint fromView:self];
	cablePopup = [[LCVRackCablePopupView alloc] initWithFrame:NSMakeRect(roundf(insetViewPoint.x - (kPopupWidth * 0.5)), insetViewPoint.y, kPopupWidth, kPopupHeight)];
	[cablePopup.aEndDevLabel setStringValue:hoverCable.aEndDevice.displayString];
	[cablePopup.aEndIntLabel setStringValue:hoverCable.aEndInterface.displayString];
	[cablePopup.bEndDevLabel setStringValue:hoverCable.bEndDevice.displayString];
	[cablePopup.bEndIntLabel setStringValue:hoverCable.bEndInterface.displayString];
	[controller.insetView addSubview:cablePopup positioned:NSWindowAbove relativeTo:nil];
	
	/* Clean up */
	popupTimer = nil;
}

- (void) mouseEntered:(NSEvent *)theEvent
{
	
}

- (void) mouseExited:(NSEvent *)theEvent
{
	/* Clear popup */
	[cablePopup removeFromSuperview];
	[cablePopup release];
	cablePopup = nil;
}

#pragma mark "Menu Events"

- (NSMenu *) menuForEvent:(NSEvent *)event
{
	return [rackController cableMenu];
	
	return nil;
}

#pragma mark Cable Location

- (LCVRackCable *) cableAtPoint:(NSPoint)point
{
	for (LCVRackCablePath *path in paths)
	{
		if ([[path path] containsPoint:point])
		{
			/* Cable found! */
			
			/* Check for highlight/isolate */
			if ([rackController cableHighlightDevice])
			{
				/* Isolation is present, check if cable is part of it */
				if ([[[path cable] aEndInterface] device] == [rackController cableHighlightDevice] || 
					[[[path cable] bEndInterface] device] == [rackController cableHighlightDevice])
				{ return [path cable]; }
			}
			else
			{
				/* No isolation occurring, return cable */
				return [path cable]; 
			}
			
			/* Cable found but isnt valid */
			return nil;
		}
	}
	
	return nil;
}

#pragma mark NSResponder Methods

- (BOOL) acceptsFirstResponder
{
	return NO; 
}

- (BOOL) becomeFirstResponder
{
	return NO; 
}

#pragma mark Animation

- (void) animationTimerFired
{
	/* Increment frame */
	frame++;
	if (frame == 15) frame=0;	
	[self setNeedsDisplay:YES];
}

#pragma mark "Properties"

@synthesize rackController;
- (void) setRackController:(id)newController
{
	if (rackController) 
	{
		[rackController removeObserver:self forKeyPath:@"cables"];
		[rackController release];
	}
	rackController = [newController retain];
	[rackController addObserver:self 
					 forKeyPath:@"cables" 
						options:NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld
						context:nil];
}

@synthesize paths;
@synthesize pathsValid;
@synthesize hitView;
@synthesize hoverCable;

@end
