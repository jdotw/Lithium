//
//  LCWAPView.m
//  Lithium Console
//
//  Created by James Wilson on 7/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCWAPView.h"
#import "LCWAPViewClient.h"
#import "LCObject.h"

@implementation LCWAPView

#pragma mark "Constructors"

- (LCWAPView *) initWithDevice:(LCEntity *)initDevice inFrame:(NSRect)frame
{
	/* Super-class init */
	[super initWithFrame:frame];
	
	/* Set device */
	device = [initDevice retain];
		
	/* Animation timer */
	animationTimer = [[NSTimer scheduledTimerWithTimeInterval:0.060 target:self selector:@selector(animationTimerFired) userInfo:nil repeats:YES] retain];	

	/* Create client dict */
	clientDictionary = [[NSMutableDictionary dictionary] retain];
	
	/* Add observers */
	[device addObserver:self 
			 forKeyPath:@"refreshInProgress" 
				options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
				context:NULL];	
	
	return self;
}

- (void) dealloc
{
	[animationTimer release];
	[super dealloc];
}

#pragma mark "View Management"

- (void) removeFromSuperview
{
	/* Remove observer */
	[animationTimer invalidate];
	[device removeObserver:self forKeyPath:@"refreshInProgress"];
	[super removeFromSuperview];
}

- (void)setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];
}

#pragma mark "Drawing"

- (void) drawRect:(NSRect)rect
{	
	if ([self drawWarnings]) return;

	/* Draw center */
	NSRect centerRect = NSMakeRect(NSMidX([self bounds])-2, NSMidY([self bounds])-2, 4, 4);
	NSBezierPath *centerPath = [NSBezierPath bezierPathWithOvalInRect:centerRect];
	[[NSColor blackColor] setFill];
	[centerPath fill];
	
	/* Concentric circles */
	int lineCount = 4;
	float radius = (NSWidth([self bounds]) * 0.5) / (float) lineCount;
	int i;
	for (i=1; i <= lineCount; i++)
	{
		float x = NSMidX([self bounds]) - (i * radius);
		float y = NSMidY([self bounds]) - (i * radius);
		float height = (i * radius * 2);
		float width = (i * radius * 2);
		NSRect circleRect = NSMakeRect(x,y,width,height);
		NSBezierPath *circlePath = [NSBezierPath bezierPathWithOvalInRect:circleRect];
		[[NSColor colorWithDeviceRed:0.3 green:1.0 blue:0.3 alpha:0.1] setStroke];
		[circlePath stroke];
	}	
	
	/* 
	 * Draw sweep and logo text
	 */

	/* Main line */
	NSBezierPath *sweepPath = [NSBezierPath bezierPath];	
	float sweepR = NSWidth([self bounds]) * 0.5;
	float sweepX = sweepR * cos (sweepAngle);
	float sweepY = sweepR * sin (sweepAngle);
	[sweepPath moveToPoint:NSMakePoint(NSMidX([self bounds]), NSMidY([self bounds]))];
	[sweepPath lineToPoint:NSMakePoint(sweepX + NSMidX([self bounds]),sweepY + NSMidY([self bounds]))];
	[sweepPath setLineWidth:1.0];
	[[NSColor colorWithDeviceRed:0.3 green:0.8 blue:0.3 alpha:0.4] setStroke];

	{
		CGContextRef context = [[NSGraphicsContext currentContext] graphicsPort];
		CGContextSaveGState(context); 
		NSShadow *theShadow = [[NSShadow alloc] init]; 
		[theShadow setShadowOffset:NSMakeSize(-2.0, 2.0)]; 
		[theShadow setShadowBlurRadius:12.0];
		[theShadow setShadowColor:[[NSColor greenColor] colorWithAlphaComponent:0.8]];
		[theShadow set];

		/* Draw sweep */
		[[NSColor colorWithDeviceRed:0.5 green:0.9 blue:0.5 alpha:0.5] setStroke];
		[sweepPath stroke];

		[theShadow release];
		CGContextRestoreGState(context);				
	}
		
	/* Increment sweep angle */
	sweepAngle += 0.02;
	if (sweepAngle >= 2 * M_PI) sweepAngle = 0.0;
	
	/* Find clients array */
	NSArray *clients = [device valueForKeyPath:@"childrenDictionary.wapclient.children"];
	
	/* Get min/max strength */
	LCObject *clientObject;	
	NSEnumerator *clientEnum = [clients objectEnumerator];
	clientObject= [clientEnum nextObject];
	float minStrength = [[clientObject valueForMetricNamed:@"strength"] floatValue];
	float maxStrength = [[clientObject valueForMetricNamed:@"strength"] floatValue];
	while (clientObject = [clientEnum nextObject])
	{
		if ([clientObject valueForMetricNamed:@"strength"])
		{
			float sigStrength = [[clientObject valueForMetricNamed:@"strength"] floatValue];
			if (sigStrength < minStrength) minStrength = sigStrength;
			if (sigStrength > maxStrength) maxStrength = sigStrength;
		}
	}
	
	/* Process clients */
	i = 0;
	int notDrawn = 0;
	clientEnum = [clients objectEnumerator];
	while (clientObject = [clientEnum nextObject])
	{
		/* Attempt to find client object */
		LCWAPViewClient *client = [clientDictionary objectForKey:[clientObject desc]];
		if (!client)
		{ 
			client = [LCWAPViewClient clientWithObject:clientObject];
			[clientDictionary setObject:client forKey:[clientObject desc]];
		}

		/* Get signal strengths and determine distance (hypot) */
		if (![clientObject valueForMetricNamed:@"strength"] || [[clientObject valueForMetricNamed:@"strength"] floatValue] == 0)
		{
			notDrawn++;
			continue;
		}
		float sigStrength = [[clientObject valueForMetricNamed:@"strength"] floatValue];
		if (sigStrength != [client lastSignalStrength])
		{
			/* Signal strength has changed, re-calculate rect, etc */
			[client setLastSignalStrength:sigStrength];
		
			/* Determine distace (i.e radius) */
			float q;
			if ([clients count] > 1)
			{ 
				/* Scale the distance against all clients */
				q = (sigStrength - minStrength) / (maxStrength - minStrength); }
			else
			{
				/* Single client, no scaling */
				q = 0.5; 
			}
			float maxDistance = NSWidth([self bounds]) * 0.3;
			float distance = maxDistance * (1 - q);
			[client setDistance:distance];
		
			/* Determine (sudo-random) y */
			float y;
			if (distance > (NSHeight([self bounds]) * 0.5))
			{ y = ((0.5 * NSHeight([self bounds])) * (rand() / (RAND_MAX + 1.0))); }
			else
			{ y = (distance * (rand() / (RAND_MAX + 1.0))); }
		
			/* Determine x using pythag */
			float x = sqrtf((distance * distance) - (y * y));
			
			/* Adjust for no-draw zone */
			x = x + (0.1 * NSWidth([self bounds]));
			
			/* Adjust for quadrant */
			switch (i % 4)
			{
				case 0:
					/* Top right */
					x += NSMidX ([self bounds]);
					y += NSMidY ([self bounds]);
					break;
				case 1:
					/* Bottom right */
					x += NSMidX ([self bounds]);
					y = NSMidY ([self bounds]) - y;
					break;
				case 2:
					/* Bottom left */
					x = NSMidX ([self bounds]) - x;
					y = NSMidY ([self bounds]) - y;
					break;
				case 3:
					/* Top left */
					x = NSMidX ([self bounds]) - x;
					y += NSMidY ([self bounds]);
					break;
			}
		
			/* Set client rect */
			[client setClientRect:NSMakeRect(x,y,8,8)];
			
			/* Create client path */
			[client setClientPath:[NSBezierPath bezierPathWithOvalInRect:[client clientRect]]];

			/* Create connector path */
			NSBezierPath *linkPath = [NSBezierPath bezierPath];
			[client setStartPoint:NSMakePoint(NSMidX([client clientRect]),NSMidY([client clientRect]))];
			[client setEndPoint:NSMakePoint(NSMidX([self bounds]),NSMidY([self bounds]))];
			[client setControlPoint1:NSMakePoint(NSMidX([client clientRect]),NSMidY([client clientRect]))];
			[client setControlPoint2:NSMakePoint(NSMidX([self bounds]),NSMidY([self bounds]))];
			[linkPath moveToPoint:[client startPoint]];
			[linkPath curveToPoint:[client endPoint] controlPoint1:[client controlPoint1]  controlPoint2:[client controlPoint2]];
			[linkPath setLineWidth:0.2];
			[client setConnectorPath:linkPath];
		}
		
		/* Draw connector */
		[[NSColor colorWithDeviceRed:0.3 green:0.7 blue:0.3 alpha:0.6] setStroke];
		[[client connectorPath] stroke];

		/* Draw throughput */
		NSString *fps = [clientObject rawValueForMetricNamed:@"rx_rate"];
		NSString *speed = [clientObject rawValueForMetricNamed:@"rate"];
		[client incrementSpeedIndicatorT];
		[self drawThroughputIndicatorsForClient:client
										  speed:speed
										   fps:fps
								speedIndicatorT:[client speedIndicatorT]
										xOffset:2
										yOffset:2
									 cableWidth:1.0];
		fps = [clientObject rawValueForMetricNamed:@"tx_rate"];
		[self drawThroughputIndicatorsForClient:client
										  speed:speed
										   fps:fps
								speedIndicatorT:1-[client speedIndicatorT]
										xOffset:1
										yOffset:1
									 cableWidth:1.0];

		/* Draw client path */		
		CGContextRef context = [[NSGraphicsContext currentContext] graphicsPort];
		CGContextSaveGState(context); 
		NSShadow *theShadow = [[NSShadow alloc] init]; 
		[theShadow setShadowOffset:NSMakeSize(-1.0, 1.0)]; 
		[theShadow setShadowBlurRadius:8.0];
		[theShadow setShadowColor:[[NSColor greenColor] colorWithAlphaComponent:0.8]];
		[theShadow set];
		[[NSColor colorWithDeviceRed:0.5 green:0.9 blue:0.5 alpha:0.9] setFill];
		[[client clientPath] fill];
		[theShadow release];
		CGContextRestoreGState(context);			
		
		/* Draw string */
		NSString *mac = [clientObject desc];
		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
//			[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:1.0], NSForegroundColorAttributeName,
			[NSColor colorWithDeviceRed:0.5 green:0.9 blue:0.5 alpha:0.9], NSForegroundColorAttributeName,
			[NSFont labelFontOfSize:9.0], NSFontAttributeName,
			nil];		
		NSSize macSize = [mac sizeWithAttributes:attr];
		float stringX;
		float stringY;
		if ((NSMaxX([client clientRect]) + macSize.width) >= NSMaxX([self bounds]))
		{
			/* Draw string approaching dot */
			stringX = NSMinX([client clientRect]) - macSize.width;
		}
		else
		{
			/* Draw string moving away from dot */
			stringX = NSMaxX([client clientRect]);			
		}
		if ((NSMaxY([client clientRect]) + macSize.height) >= NSMaxY([self bounds]))
		{
			/* Draw string under dot */
			stringY = NSMinY([client clientRect]) - macSize.height;
		}
		else
		{
			/* Draw string above dot */
			stringY = NSMaxY([client clientRect]);
		}
		context = [[NSGraphicsContext currentContext] graphicsPort];
		CGContextSaveGState(context); 
		theShadow = [[NSShadow alloc] init]; 
		[theShadow setShadowOffset:NSMakeSize(2.0, -2.0)]; 
		[theShadow setShadowBlurRadius:3.0];
		[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.8]];
		[theShadow set];
		[mac drawAtPoint:NSMakePoint(stringX,stringY) withAttributes:attr];
		[theShadow release];
		CGContextRestoreGState(context);					
		
		/* Increment */
		i++;
	}

	/* Not drawn count */
	{
		CGContextRef context = [[NSGraphicsContext currentContext] graphicsPort];
		CGContextSaveGState(context); 
		NSShadow *theShadow = [[NSShadow alloc] init]; 
		[theShadow setShadowOffset:NSMakeSize(-2.0, 2.0)]; 
		[theShadow setShadowBlurRadius:12.0];
		[theShadow setShadowColor:[[NSColor greenColor] colorWithAlphaComponent:0.8]];
		[theShadow set];
		
		/* Logo text */
		NSString *notDrawnString = [NSString stringWithFormat:@"%i clients not shown", notDrawn];
		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithDeviceRed:0.5 green:0.9 blue:0.5 alpha:0.9], NSForegroundColorAttributeName,
			[NSFont labelFontOfSize:9.0], NSFontAttributeName,
			nil];
		NSSize stringSize = [notDrawnString sizeWithAttributes:attr];
		[notDrawnString drawAtPoint:NSMakePoint(NSMaxX([self bounds]) - stringSize.width - 10, NSMaxY([self bounds])-10)
		   withAttributes:attr];		
		
		/* Draw sweep */
		[[NSColor colorWithDeviceRed:0.5 green:0.9 blue:0.5 alpha:0.5] setStroke];
		[sweepPath stroke];
		
		[theShadow release];
		CGContextRestoreGState(context);				
	}
}

- (void) drawThroughputIndicatorsForClient:(LCWAPViewClient *)client 
									speed:(NSString *)speed 
									 fps:(NSString *)fps 
						  speedIndicatorT:(float)speedIndicatorT
								  xOffset:(float)xOffset 
								  yOffset:(float)yOffset
							   cableWidth:(float)cableWidth
{
	float dotDiam = 2.0;
	
	/* Get max dots */
	float maxDots = ([client distance] + NSWidth([self bounds]) * 0.2)/ dotDiam;		
	
	/* Get pilot point */
	int pointsToDraw = maxDots * ([fps floatValue] / 10000.0);					/* This is the number of points to draw to represent the utilisaton */	
	if (pointsToDraw > maxDots) pointsToDraw = maxDots;
	if (pointsToDraw == 0 && [fps floatValue] > 10) pointsToDraw = 1;
	
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
			NSPoint point = [client pointOnConnectorPathAt:t];
			NSRect indicator = NSMakeRect(point.x-xOffset,point.y-yOffset,dotDiam,dotDiam);
			NSBezierPath *indicatorPath = [NSBezierPath bezierPathWithOvalInRect:indicator];
			NSColor *indicColor = [NSColor colorWithCalibratedRed:0.2 green:0.8 blue:0.2 alpha:0.5];
			[indicColor setFill];
			[indicatorPath fill];
			dotIndex++;
			t = tOffset + (dotIndex * tIncrement);												/* Advance to next dot position */
		}
	}			
}

#pragma mark Animation

- (void) animationTimerFired
{
	[self setNeedsDisplay:YES];
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	[self setNeedsDisplay:YES];
}


@synthesize animationTimer;
@synthesize clientDictionary;
@synthesize sweepAngle;
@end
