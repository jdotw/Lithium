//
//  LCXRDeviceView.m
//  Lithium Console
//
//  Created by James Wilson on 15/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXRDeviceView.h"
#import "LCXRDriveView.h"
#import "math.h"

static inline double radians (double degrees) {return degrees * M_PI/180;}

@implementation LCXRDeviceView

#pragma mark "Constructors"

- (LCXRDeviceView *) initWithDevice:(LCEntity *)initDevice inFrame:(NSRect)frame
{
	/* Super-class init */
	[super initWithFrame:frame];
	
	/* WWDC Play */
	[self setScale:1.0];
	
	/* Set device */
	device = [initDevice retain];
	
	/* Load images */
	xsrBackImage = [[NSImage imageNamed:@"xsrback.png"] retain];
	driveBackImage = [[NSImage imageNamed:@"xsrdriveback.png"] retain];
	driveBlankImage = [[NSImage imageNamed:@"xsrblankdrive.png"] retain];
	appleLogo = [[NSImage imageNamed:@"xsrapplelogo.png"] retain];
	boltImage = [[NSImage imageNamed:@"xsrbolt.png"] retain];
	arrayWindowImage = [[NSImage imageNamed:@"xsrarraywindow.png"] retain];
	
	/* Add observers */
	[device addObserver:self 
				forKeyPath:@"refreshInProgress" 
				   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
				   context:NULL];	
	
	/* Reset drive views */
	driveViews = [[NSMutableArray array] retain];
	[self resetDriveViews];	
	
	return self;
}

- (void) dealloc
{
	[xsrBackImage release];
	[driveBackImage release];
	[driveBlankImage release];
	[appleLogo release];
	[boltImage release];
	[arrayWindowImage release];
	[driveViews release];
	[super dealloc];
}

#pragma mark "View Management"

- (void) removeFromSuperview
{
	/* Remove observer */
	[device removeObserver:self forKeyPath:@"refreshInProgress"];
	
	/* Remove device views */
	LCXRDriveView *driveView;
	for (driveView in driveViews)
	{
		[driveView removeFromSuperview];
		[driveView autorelease];
	}
	[driveViews removeAllObjects];
	
	[super removeFromSuperview];
}

#pragma mark "Geometry"

- (float) scale
{ return scale; }
- (void) setScale:(float)val
{ scale = val; }

- (float) containerViewHight
{ return 194.0; }

- (float) xsrWidth
{ return 620.0; }

- (float) xsrHeight
{ return 155.0; }

- (NSRect) xsrRect
{ return NSMakeRect((([self bounds].size.width - [self xsrWidth])/2),(([self bounds].size.height - [self xsrHeight])/2), [self xsrWidth], [self xsrHeight]); }

- (NSRect) driveBay1Rect
{
	float driveHeight = [self xsrHeight] - 34;
	return NSMakeRect([self xsrRect].origin.x + 25, [self xsrRect].origin.y + 16, 268, driveHeight);
}

- (NSRect) driveBay2Rect
{
	float driveHeight = [self xsrHeight] - 34;
	return NSMakeRect(NSMaxX([self xsrRect]) - 268 - 25, [self xsrRect].origin.y + 16,268,driveHeight);
}

#pragma mark "Drawing"

- (void) drawRect:(NSRect)rect
{
	if ([self drawWarnings]) return;

	/* Basic dimensions */
	NSRect xsrRect = [self xsrRect];
	
	/*
	 * Draw the unit background 
	 */

	CGContextRef context = [[NSGraphicsContext currentContext] graphicsPort];
	CGContextSaveGState(context); 
	
	/* Setup the shadow */
	NSShadow *theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(3.0, -3.0)]; 
	[theShadow setShadowBlurRadius:3.0];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.6]];
	[theShadow set];	
	
	/* Draw back */
	[xsrBackImage drawInRect:xsrRect
					fromRect:NSMakeRect(0,0,[xsrBackImage size].width,[xsrBackImage size].height)
				   operation:NSCompositeSourceOver
					fraction:1.0];

	[theShadow release];
	CGContextRestoreGState(context);

	/* Draw apple logo */
	[appleLogo drawInRect:NSMakeRect(NSMidX(xsrRect)-([appleLogo size].width/2), NSMaxY(xsrRect)-20, [appleLogo size].width, [appleLogo size].height)
				 fromRect:NSMakeRect(0,0,[appleLogo size].width,[appleLogo size].height) 
				operation:NSCompositeSourceOver 
				 fraction:1.0];
	
	/* Draw left bolt */
	[boltImage drawInRect:NSMakeRect(NSMinX(xsrRect)+5,NSMidY(xsrRect) - ([boltImage size].height / 2),[boltImage size].width,[boltImage size].height)
				 fromRect:NSMakeRect(0,0,[boltImage size].width,[boltImage size].height) 
				operation:NSCompositeSourceOver 
				 fraction:1.0];

	/* Draw right bolt */
	[boltImage drawInRect:NSMakeRect(NSMaxX(xsrRect)-(5+([boltImage size].width)),NSMidY(xsrRect) - ([boltImage size].height / 2),[boltImage size].width,[boltImage size].height)
				 fromRect:NSMakeRect(0,0,[boltImage size].width,[boltImage size].height) 
				operation:NSCompositeSourceOver 
				 fraction:1.0];
	
	/* 
	 * Unit status indicators
	 */
	
	NSString *str = @"Status";
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.6], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Bank Gothic Light BT" size:10], NSFontAttributeName,
		nil];
	[str drawAtPoint:NSMakePoint(NSMinX(xsrRect)+40, NSMaxY(xsrRect)-15) withAttributes:attr];

	/* Status dot */
	NSRect statusRect = NSMakeRect (NSMinX(xsrRect)+30, NSMaxY(xsrRect)-13, 8, 8);
	NSBezierPath *statusPath = [NSBezierPath bezierPathWithOvalInRect:statusRect];
	[[device opStateColor] setFill];
	[statusPath fill];
	[[NSColor darkGrayColor] setStroke];
	[statusPath stroke];	

	/*
	 * Upper Controller
	 */

	str = @"Upper Controller";
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.6], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Bank Gothic Light BT" size:10], NSFontAttributeName,
		nil];
	[str drawAtPoint:NSMakePoint(NSMinX(xsrRect)+180, NSMaxY(xsrRect)-15) withAttributes:attr];
	
	/* Get controller */
	LCEntity *statusContainer = [device childNamed:@"xrstatus"];
	LCEntity *upperControllerObject = nil;
	if ([[statusContainer children] count] > 0)
	{ upperControllerObject = [[statusContainer children] objectAtIndex:0]; }
	
	/* Status dot */
	NSRect upperContStatusRect = NSMakeRect (NSMinX(xsrRect)+285, NSMaxY(xsrRect)-13, 8, 8);
	NSBezierPath *upperContStatusPath= [NSBezierPath bezierPathWithOvalInRect:upperContStatusRect];
	[[upperControllerObject opStateColor] setFill];
	[upperContStatusPath fill];
	[[NSColor darkGrayColor] setStroke];
	[upperContStatusPath stroke];	
	
	/*
	 * Lower controller
	 */

	str = @"Lower Controller";
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.6], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Bank Gothic Light BT" size:10], NSFontAttributeName,
		nil];
	[str drawAtPoint:NSMakePoint(NSMinX(xsrRect)+340, NSMaxY(xsrRect)-15) withAttributes:attr];
	
	/* Get controller */
	LCEntity *lowerControllerObject = nil;
	if ([[statusContainer children] count] > 1)
	{ lowerControllerObject = [[statusContainer children] objectAtIndex:1]; }	
	
	NSRect lowerContStatusRect = NSMakeRect (NSMinX(xsrRect)+328, NSMaxY(xsrRect)-13, 8, 8);
	NSBezierPath *lowerContStatusPath = [NSBezierPath bezierPathWithOvalInRect:lowerContStatusRect];
	[[lowerControllerObject opStateColor] setFill];
	[lowerContStatusPath fill];
	[[NSColor darkGrayColor] setStroke];
	[lowerContStatusPath stroke];		
	
	/*
	 * Power status
	 */
	
	str = @"Power";
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.6], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Bank Gothic Light BT" size:10], NSFontAttributeName,
		nil];
	[str drawAtPoint:NSMakePoint(NSMinX(xsrRect)+100, NSMaxY(xsrRect)-15) withAttributes:attr];

	/* Get controller */
	LCEntity *powerMetric = [upperControllerObject childNamed:@"powerstate"];
	
	/* Status dot */
	NSRect powerStatusRect = NSMakeRect (NSMinX(xsrRect)+90, NSMaxY(xsrRect)-13, 8, 8);
	NSBezierPath *powrStatusPath = [NSBezierPath bezierPathWithOvalInRect:powerStatusRect];
	[[powerMetric opStateColor] setFill];
	[powrStatusPath fill];
	[[NSColor darkGrayColor] setStroke];
	[powrStatusPath stroke];	
		
	/*
	 * Fibre channel links
	 */
	
	str = @"Upper FC Link";
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.6], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Bank Gothic Light BT" size:10], NSFontAttributeName,
		nil];
	[str drawAtPoint:NSMakePoint(NSMinX(xsrRect)+40, NSMinY(xsrRect)+3) withAttributes:attr];

	/* Get upper link object */
	LCEntity *upperFcLinkCont = [device childNamed:@"xrhostiface_1"];
	
	/* Status dot */
	NSRect upperFCRect = NSMakeRect (NSMinX(xsrRect)+30, NSMinY(xsrRect)+5, 8, 8);
	NSBezierPath *upperFCPath = [NSBezierPath bezierPathWithOvalInRect:upperFCRect];
	[[upperFcLinkCont opStateColor] setFill];
	[upperFCPath fill];
	[[NSColor darkGrayColor] setStroke];
	[upperFCPath stroke];	

	/* Label */
//	str = @"Upper Ethernet Link";
//	attr = [NSDictionary dictionaryWithObjectsAndKeys:
//		[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.6], NSForegroundColorAttributeName,
//		[NSFont fontWithName:@"Bank Gothic Light BT" size:10], NSFontAttributeName,
//		nil];
//	[str drawAtPoint:NSMakePoint(NSMinX(xsrRect)+140, NSMinY(xsrRect)+3) withAttributes:attr];
	
	/* Status dot */
//	NSRect upperEthRect = NSMakeRect (NSMinX(xsrRect)+130, NSMinY(xsrRect)+5, 8, 8);
//	NSBezierPath *upperEthPath = [NSBezierPath bezierPathWithOvalInRect:upperEthRect];
//	[[powerMetric opStateColor] setFill];
//	[upperEthPath fill];
//	[[NSColor darkGrayColor] setStroke];
//	[upperEthPath stroke];	
	
	/* Label */
	str = @"Lower FC Link";
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.6], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Bank Gothic Light BT" size:10], NSFontAttributeName,
		nil];
	[str drawAtPoint:NSMakePoint(NSMinX(xsrRect)+340, NSMinY(xsrRect)+3) withAttributes:attr];
	
	/* Get lower link container */
	LCEntity *lowerFcLinkCont = [device childNamed:@"xrhostiface_2"];
	
	/* Status dot */
	NSRect lowerFCRect = NSMakeRect (NSMinX(xsrRect)+330, NSMinY(xsrRect)+5, 8, 8);
	NSBezierPath *lowerFCPath = [NSBezierPath bezierPathWithOvalInRect:lowerFCRect];
	[[lowerFcLinkCont opStateColor] setFill];
	[lowerFCPath fill];
	[[NSColor darkGrayColor] setStroke];
	[lowerFCPath stroke];	
	
//	str = @"Lower Ethernet Link";
//	attr = [NSDictionary dictionaryWithObjectsAndKeys:
//		[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.6], NSForegroundColorAttributeName,
//		[NSFont fontWithName:@"Bank Gothic Light BT" size:10], NSFontAttributeName,
//		nil];
//	[str drawAtPoint:NSMakePoint(NSMinX(xsrRect)+440, NSMinY(xsrRect)+3) withAttributes:attr];
	
	/* Status dot */
//	NSRect lowerEthRect = NSMakeRect (NSMinX(xsrRect)+430, NSMinY(xsrRect)+5, 8, 8);
//	NSBezierPath *lowerEthPath = [NSBezierPath bezierPathWithOvalInRect:lowerEthRect];
//	[[powerMetric opStateColor] setFill];
//	[lowerEthPath fill];
//	[[NSColor darkGrayColor] setStroke];
//	[lowerEthPath stroke];	
	
	/* 
	 * Lithium Logo
	 */
	
	str = @"LITHIUM|Console";
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.6], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Bank Gothic Light BT" size:9], NSFontAttributeName,
		nil];
	[str drawAtPoint:NSMakePoint(NSMinX(xsrRect)+520, NSMaxY(xsrRect)-15) withAttributes:attr];
	
	/*
	 * Drive Bays
	 */
	
	/* Draw drive bay 1 */
	NSRect driveBay1 = [self driveBay1Rect];
	[[NSColor colorWithCalibratedRed:0.1 green:0.1 blue:0.1 alpha:1.0] setFill];
	NSRectFill(driveBay1);

	/* Draw drive bay 2 */
	NSRect driveBay2 = [self driveBay2Rect];
	[[NSColor colorWithCalibratedRed:0.1 green:0.1 blue:0.1 alpha:1.0] setFill];
	NSRectFill(driveBay2);
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	[self setNeedsDisplay:YES];
	[self resetDriveViews];
}

#pragma mark "Resizing"

- (void)setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];
	[self setNeedsDisplay:YES];
	[self resetDriveViews];
}

#pragma mark "Drive View Management"

- (void) resetDriveViews
{
	/* Clear old views */
	LCXRDriveView *driveView;
	for (driveView in driveViews)
	{
		[driveView removeFromSuperview];
		[driveView autorelease];
	}
	[driveViews removeAllObjects];

	/* Check for warnings */
	if ([self hasWarnings]) return;
	
	/* Create views */
	int i;
	for (i=0; i < 14; i++)
	{
		/* Get Drive Object */
		NSDictionary *driveDict = [[[device childrenDictionary] objectForKey:@"xrdrives"] childrenDictionary];
		NSString *key = [NSString stringWithFormat:@"drive_%i", i+1];
		LCEntity *driveObject = [driveDict objectForKey:key];
		
		/* Create drive rect */
		float xOffset = 2;
		float yOffset = 2;
		NSRect driveRect;
		if (i < 7)
		{
			float x = [self driveBay1Rect].origin.x + (xOffset * (i+1)) + (i * [driveBlankImage size].width);
			float y = [self driveBay1Rect].origin.y + yOffset;
			driveRect = NSMakeRect(x * scale, y * scale, [driveBlankImage size].width, [self driveBay1Rect].size.height-4);
		}
		else
		{
			float x = [self driveBay2Rect].origin.x + (xOffset * (i-6)) + ((i-7) * [driveBlankImage size].width);
			float y = [self driveBay2Rect].origin.y + yOffset;
			driveRect = NSMakeRect(x * scale, y * scale, [driveBlankImage size].width, [self driveBay2Rect].size.height-4);
		}
			
		/* Create drive view */
		driveView = [[LCXRDriveView alloc] initWithDrive:(LCObject *)driveObject slot:i inFrame:driveRect];
		if (driveView)
		{ 
			[driveViews addObject:driveView]; 
			[self addSubview:driveView];
		}
	}
}

- (void) driveDidRaise:(LCXRDriveView *)driveView
{
	/* Re-arrange views */
	[driveView removeFromSuperview];
	[self addSubview:driveView positioned:NSWindowAbove relativeTo:nil];	
	
	/* Disable other drives tracking rects */
	LCXRDriveView *otherView;
	for (otherView in driveViews)
	{
		if (otherView == driveView) continue;
		[otherView setTrackingRectEnabled:NO];
	}
	
	/* Set drive as first responder */
	[[self window] makeFirstResponder:driveView];	
}

- (void) driveDidLower:(LCXRDriveView *)driveView
{
	/* Enable other drives tracking rects */
	LCXRDriveView *otherView;
	for (otherView in driveViews)
	{
		if (otherView == driveView) continue;
		[otherView setTrackingRectEnabled:YES];
	}
}
	
@synthesize xsrBackImage;
@synthesize driveBackImage;
@synthesize driveBlankImage;
@synthesize appleLogo;
@synthesize boltImage;
@synthesize arrayWindowImage;
@synthesize driveViews;
@end
