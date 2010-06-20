//
//  LCUPSDeviceView.m
//  Lithium Console
//
//  Created by James Wilson on 15/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCUPSDeviceView.h"


@implementation LCUPSDeviceView

#pragma mark "Constructors"

- (LCUPSDeviceView *) initWithDevice:(LCEntity *)initDevice inFrame:(NSRect)frame
{
	[super initWithFrame:frame];
	[self setDevice:initDevice];
	animationTimer = [[NSTimer scheduledTimerWithTimeInterval:1/10.0 target:self selector:@selector(nextFrame) userInfo:nil repeats:YES] retain];
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark "View Management"

- (void) removeFromSuperview
{
	/* Remove bindings */
	[self unbind:@"mainsVoltage"];
	[self unbind:@"mainsFrequency"];
	[self unbind:@"batteryCapacity"];
	[self unbind:@"batteryTemp"];
	[self unbind:@"batteryTime"];
	[self unbind:@"outputStatus"];
	[self unbind:@"outputVoltage"];
	[self unbind:@"outputFrequency"];
	[self unbind:@"outputLoad"];
	[self unbind:@"outputCurrent"];
	[self unbind:@"outputStatusInteger"];
	
	/* Invalidate timer */
	[animationTimer invalidate];
	[animationTimer release];
	
	[super removeFromSuperview];
}

#pragma mark "Geometry Methods"

- (float) visualWidth
{ return 421.0; }

- (float) visualHeight
{ return 171.0; }

- (NSRect) visualRect
{
	return NSMakeRect(NSMidX([self bounds]) - (0.5 * [self visualWidth]),
			   NSMidY([self bounds]) - (0.5 * [self visualHeight]),
			   [self visualWidth], [self visualHeight]);
}

- (NSRect) mainsRect
{
	return NSMakeRect(NSMinX([self visualRect]) + 4,
					  NSMinY([self visualRect]) + 105,
					  260,62);
}

- (NSRect) batteryRect
{
	return NSMakeRect(NSMinX([self visualRect]) + 4,
					  NSMinY([self visualRect]) + 4,
					  260, 90);
}

- (NSRect) outputRect
{
	return NSMakeRect(NSMinX([self visualRect]) + 275,
					  NSMinY([self visualRect]) + 4,
					  142, 162);
}

#pragma mark "Drawing"

- (void) drawRect:(NSRect)rect
{
	if ([self drawWarnings]) return;

	/* Draw template */
	[[NSGraphicsContext currentContext] saveGraphicsState];
	NSShadow *shadow = [[NSShadow alloc] init];
	[shadow setShadowOffset:NSMakeSize(3.0, -3.0)];
	[shadow setShadowBlurRadius:5.0];
	[shadow setShadowColor:[NSColor colorWithCalibratedWhite:0.0 alpha:0.4]];
	[shadow set];
	NSImage *backImage = [NSImage imageNamed:@"apcupstemplate.png"];
	[backImage drawInRect:[self visualRect]
				 fromRect:NSMakeRect(0, 0, [backImage size].width, [backImage size].height)
				operation:NSCompositeSourceOver
				 fraction:1.0];
	[shadow release];
	[[NSGraphicsContext currentContext] restoreGraphicsState];

	/* Draw Battery */
	if ([device valueForKeyPath:@"childrenDictionary.apcbattery.childrenDictionary.master"])
	{ [self drawBattery]; }
	
	/* Draw mains */
	if ([device valueForKeyPath:@"childrenDictionary.apcinput.childrenDictionary.master"])	
	{ [self drawMains]; }
	
	/* Draw output */
	if ([device valueForKeyPath:@"childrenDictionary.apcoutput.childrenDictionary.master"])
	{ [self drawOutput]; }
}

#pragma mark "Battery Drawing"

- (void) drawBattery
{
	/* Label */
	NSString *label = @"Battery";
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.8], NSForegroundColorAttributeName,
		[NSFont boldSystemFontOfSize:9.0], NSFontAttributeName,
		nil];
	[label drawAtPoint:NSMakePoint(NSMinX([self batteryRect]) + 4, NSMaxY([self batteryRect]) - 10) withAttributes:attr];

	/* Get Level */
	float level;
	if ([self batteryCapacity])
	{ level = [[self batteryCapacity] floatValue]; }
	else
	{ level = 0.0; }
	
	/* Establish geometry */
	NSRect gaugeRect = [self batteryRect];
	gaugeRect.size.height -= 12.0;
	NSRect gaugeFill = gaugeRect;
	gaugeFill.size.height = gaugeRect.size.height * (level / 100.0);
	
	/* Draw basic fill path */
	NSBezierPath *fillPath = [NSBezierPath bezierPath];
	[fillPath moveToPoint:NSMakePoint(NSMinX(gaugeFill),NSMaxY(gaugeFill))];
	[fillPath lineToPoint:NSMakePoint(NSMinX(gaugeFill),NSMinY(gaugeFill))];
	[fillPath lineToPoint:NSMakePoint(NSMaxX(gaugeFill),NSMinY(gaugeFill))];
	[fillPath lineToPoint:NSMakePoint(NSMaxX(gaugeFill),NSMaxY(gaugeFill))];	
	
	/* Draw wave top */
	int crestCount = 10;
	int i;
	float waveLength = NSWidth(gaugeFill) / (float) crestCount;
	float curX = NSMaxX(gaugeFill);
	for (i=0; i < crestCount; i++)
	{
		[fillPath curveToPoint:NSMakePoint(curX - waveLength,NSMaxY(gaugeFill))
				 controlPoint1:NSMakePoint(curX - (0.2 * waveLength), NSMaxY(gaugeFill) - (0.3 * waveLength))
				 controlPoint2:NSMakePoint(curX - (0.8 * waveLength), NSMaxY(gaugeFill) - (0.3 * waveLength))];
		curX = curX - waveLength;
	}
	
	/* Fill */
	[[NSColor colorWithCalibratedRed:72.0/256.0 green:150.0/256.0 blue:176.0/256.0 alpha:0.8] setFill];
	[fillPath fill];
	
	/* Text */
	NSString *text = [NSString stringWithFormat:@"%@ - %@ - %@", [self batteryCapacity], [self batteryTime], [self batteryTemp]];
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.8], NSForegroundColorAttributeName,
		[NSFont boldSystemFontOfSize:11.0], NSFontAttributeName,
		nil];
	NSSize textSize = [text sizeWithAttributes:attr];
	NSRect textRect = NSMakeRect(NSMidX([self batteryRect]) - (0.5 * textSize.width),
								 NSMinY([self batteryRect])+4,
								 textSize.width,textSize.height);
	[text drawInRect:textRect withAttributes:attr];
}

#pragma mark "Mains Drawing"

- (void) drawMains
{
	/* Label */
	NSString *label = @"Mains";
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.8], NSForegroundColorAttributeName,
		[NSFont boldSystemFontOfSize:9.0], NSFontAttributeName,
		nil];
	[label drawAtPoint:NSMakePoint(NSMinX([self mainsRect]) + 4, NSMaxY([self mainsRect]) - 10) withAttributes:attr];
	
	/* Establish geometry */
	NSRect waveRect = [self mainsRect];
	waveRect.size.height -= 12.0;
	waveRect.origin.y += 6;
	
	/* Set clip */
	[[NSGraphicsContext currentContext] saveGraphicsState];
	float clipWidth = 25;
	NSRect fullClip = NSMakeRect(NSMinX(waveRect)-clipWidth,NSMinY(waveRect)+8, NSWidth(waveRect) + (clipWidth), NSHeight(waveRect)-16);
	NSRect clipRect1 = NSMakeRect(NSMinX(fullClip),NSMinY(fullClip), NSWidth(fullClip) * animationT, NSHeight(fullClip));
	NSRect clipRect2 = NSMakeRect(NSMaxX(clipRect1) + clipWidth, NSMinY(fullClip), 
								  (NSWidth(fullClip) * (1 - animationT))-clipWidth, NSHeight(fullClip));
	NSBezierPath *clipPath = [NSBezierPath bezierPathWithRect:clipRect1];
	[clipPath appendBezierPathWithRect:clipRect2];
	[[NSColor clearColor] setFill];
	[clipPath fill];
	[clipPath addClip];
	
	NSBezierPath *wavePath = [NSBezierPath bezierPath];
	[wavePath setLineWidth:3.0];
	[wavePath moveToPoint:NSMakePoint(NSMinX(waveRect), NSMidY(waveRect))];
	if ([[self mainsVoltage] intValue] > 0)
	{
		/* Draw wave diagram */
		int crestCount = 5;
		int i;
		float waveLength = NSWidth(waveRect) / (float) crestCount;
		float curX = NSMinX(waveRect);
		for (i=0; i < crestCount; i++)
		{
			[wavePath curveToPoint:NSMakePoint(curX + waveLength, NSMidY(waveRect))
					 controlPoint1:NSMakePoint(curX + (0.5 * waveLength), NSMaxY(waveRect) + 20)
					 controlPoint2:NSMakePoint(curX + (0.5 * waveLength), NSMinY(waveRect) - 20)];
			curX = curX + waveLength;
		}
		[[NSColor colorWithCalibratedRed:36.0/256.0 green:203.0/256.0 blue:222.0/256.0 alpha:1.0] setStroke];
	}
	else
	{
		/* Draw flatline */
		[[NSColor colorWithCalibratedRed:223.0/256.0 green:35.0/256.0 blue:35.0/256.0 alpha:1.0] setStroke];
		[wavePath lineToPoint:NSMakePoint(NSMaxX(waveRect),NSMidY(waveRect))];
	}
	
	/* Fill */
	[wavePath stroke];
	[[NSGraphicsContext currentContext] restoreGraphicsState];

	/* Text */
	NSString *text = [NSString stringWithFormat:@"%@ - %@", [self mainsVoltage], [self mainsFrequency]];
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.8], NSForegroundColorAttributeName,
		[NSFont boldSystemFontOfSize:11.0], NSFontAttributeName,
		nil];
	NSSize textSize = [text sizeWithAttributes:attr];
	NSRect textRect = NSMakeRect(NSMidX([self mainsRect]) - (0.5 * textSize.width),
								 NSMinY([self mainsRect]),
								 textSize.width,textSize.height);
	[text drawInRect:textRect withAttributes:attr];	
}

#pragma mark "Output"

- (void) drawOutput
{
	/* Label */
	NSString *label = @"Output";
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.8], NSForegroundColorAttributeName,
		[NSFont boldSystemFontOfSize:9.0], NSFontAttributeName,
		nil];
	[label drawAtPoint:NSMakePoint(NSMinX([self outputRect]) + 4, NSMaxY([self outputRect]) - 10) withAttributes:attr];
	
	/* Load */
	NSString *loadPercent = [self outputLoad];
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.8], NSForegroundColorAttributeName,
		[NSFont boldSystemFontOfSize:24.0], NSFontAttributeName,
		nil];
	NSSize textSize = [loadPercent sizeWithAttributes:attr];
	NSRect textRect = NSMakeRect(NSMidX([self outputRect]) - (0.5 * textSize.width),
								 NSMinY([self outputRect]) + 22,
								 textSize.width,textSize.height);	
	[loadPercent drawInRect:textRect withAttributes:attr];	

	NSString *loadInfo = [NSString stringWithFormat:@"Load - %@", [self outputCurrent]];
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.8], NSForegroundColorAttributeName,
		[NSFont boldSystemFontOfSize:10.0], NSFontAttributeName,
		nil];
	textSize = [loadInfo sizeWithAttributes:attr];
	textRect = NSMakeRect(NSMidX([self outputRect]) - (0.5 * textSize.width),
						  NSMinY([self outputRect]) + 8,
						  textSize.width,textSize.height);	
	[loadInfo drawInRect:textRect withAttributes:attr];	

	NSString *outputInfo = [NSString stringWithFormat:@"%@ - %@", [self outputVoltage], [self outputFrequency]];
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.8], NSForegroundColorAttributeName,
		[NSFont boldSystemFontOfSize:10.0], NSFontAttributeName,
		nil];
	textSize = [outputInfo sizeWithAttributes:attr];
	textRect = NSMakeRect(NSMidX([self outputRect]) - (0.5 * textSize.width),
						  NSMinY([self outputRect]) + 80,
						  textSize.width,textSize.height);	
	[outputInfo drawInRect:textRect withAttributes:attr];	

	NSString *outputMessage = [self outputStatus];
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.8], NSForegroundColorAttributeName,
		[NSFont boldSystemFontOfSize:10.0], NSFontAttributeName,
		nil];
	textSize = [outputMessage sizeWithAttributes:attr];
	textRect = NSMakeRect(NSMidX([self outputRect]) - (0.5 * textSize.width),
						  NSMinY([self outputRect]) + 68,
						  textSize.width,textSize.height);	
	[outputMessage drawInRect:textRect withAttributes:attr];	
	
	/* Output Icon */
	NSRect iconRect = NSMakeRect(NSMidX([self outputRect]) - 40, NSMinY([self outputRect]) + 106, 80, 45);
	if ([[self outputStatusInteger] intValue] == 2)
	{ [self drawOnMainsIconInRect:iconRect]; }
	else if ([[self outputStatusInteger] intValue] == 3)
	{ [self drawBatteryIconInRect:iconRect]; }
	else if ([[self outputStatusInteger] intValue] == 12)
	{ [self drawOnSmartTrimIconInRect:iconRect]; }
	else if ([[self outputStatusInteger] intValue] == 4)
	{ [self drawOnSmartBoostIconInRect:iconRect]; }
}

#pragma mark "Output Icons"

- (void) drawBatteryIconInRect:(NSRect)rect
{
	[[NSColor colorWithCalibratedRed:223.0/256.0 green:159.0/256.0 blue:35.0/256.0 alpha:0.9] setStroke];
//	[[NSColor colorWithCalibratedRed:72.0/256.0 green:150.0/256.0 blue:176.0/256.0 alpha:0.9] setStroke];
//	[[NSColor colorWithCalibratedRed:72.0/256.0 green:150.0/256.0 blue:176.0/256.0 alpha:0.9] setFill];

	NSRect battRect = NSMakeRect(NSMidX(rect)-30,NSMinY(rect),60,30);
	NSBezierPath *battOutline = [NSBezierPath bezierPathWithRect:battRect];
	[battOutline setLineWidth:3.0];
	[battOutline stroke];
	
	NSBezierPath *terminalPath = [NSBezierPath bezierPath];
	[terminalPath moveToPoint:NSMakePoint(NSMidX(rect)-24, NSMinY(rect) + 34)];
	[terminalPath lineToPoint:NSMakePoint(NSMidX(rect)-16, NSMinY(rect) + 34)];
	[terminalPath setLineWidth:3.0];
	[terminalPath stroke];

	terminalPath = [NSBezierPath bezierPath];
	[terminalPath moveToPoint:NSMakePoint(NSMidX(rect)+16, NSMinY(rect) + 34)];
	[terminalPath lineToPoint:NSMakePoint(NSMidX(rect)+24, NSMinY(rect) + 34)];
	[terminalPath setLineWidth:3.0];
	[terminalPath stroke];	
	
	NSBezierPath *wavePath = [NSBezierPath bezierPath];
	[wavePath moveToPoint:NSMakePoint(NSMinX(battRect)+5,NSMidY(battRect))];
	[wavePath curveToPoint:NSMakePoint(NSMaxX(battRect)-5,NSMidY(battRect))
			 controlPoint1:NSMakePoint(NSMidX(battRect),NSMaxY(battRect) + 20)
			 controlPoint2:NSMakePoint(NSMidX(battRect),NSMinY(battRect) - 20)];
	[wavePath setLineWidth:2.5];
	[wavePath stroke];	
}

- (void) drawOnMainsIconInRect:(NSRect)rect
{
	[[NSColor colorWithCalibratedRed:36.0/256.0 green:203.0/256.0 blue:222.0/256.0 alpha:0.4] setStroke];
	NSBezierPath *dottedPath = [NSBezierPath bezierPath];
	CGFloat array[2];
	array[0] = 5.0; //segment painted with stroke color
	array[1] = 2.0; //segment not painted with a color
	[dottedPath setLineDash: array count: 2 phase: 0.0];	
	[dottedPath moveToPoint:NSMakePoint(NSMinX(rect), NSMidY(rect))];
	[dottedPath lineToPoint:NSMakePoint(NSMaxX(rect), NSMidY(rect))];
	[dottedPath setLineWidth:2.0];
	[dottedPath stroke];

	[[NSColor colorWithCalibratedRed:36.0/256.0 green:203.0/256.0 blue:222.0/256.0 alpha:0.9] setStroke];
	NSBezierPath *wavePath = [NSBezierPath bezierPath];
	[wavePath moveToPoint:NSMakePoint(NSMinX(rect)+4,NSMidY(rect))];
	[wavePath curveToPoint:NSMakePoint(NSMaxX(rect)-4,NSMidY(rect))
			 controlPoint1:NSMakePoint(NSMidX(rect),NSMaxY(rect)+20)
			 controlPoint2:NSMakePoint(NSMidX(rect),NSMinY(rect)-20)];
	[wavePath setLineWidth:3.0];
	[wavePath stroke];
}

- (void) drawOnSmartTrimIconInRect:(NSRect)rect
{
	[[NSColor colorWithCalibratedRed:223.0/256.0 green:159.0/256.0 blue:35.0/256.0 alpha:0.4] setStroke];
	NSBezierPath *dottedPath = [NSBezierPath bezierPath];
	CGFloat array[2];
	array[0] = 5.0; //segment painted with stroke color
	array[1] = 2.0; //segment not painted with a color
	[dottedPath setLineDash: array count: 2 phase: 0.0];	
	[dottedPath moveToPoint:NSMakePoint(NSMinX(rect), NSMidY(rect))];
	[dottedPath lineToPoint:NSMakePoint(NSMaxX(rect), NSMidY(rect))];
	[dottedPath setLineWidth:2.0];
	[dottedPath stroke];
	
	[[NSColor colorWithCalibratedRed:223.0/256.0 green:159.0/256.0 blue:35.0/256.0 alpha:0.9] setStroke];
	NSBezierPath *wavePath = [NSBezierPath bezierPath];
	[wavePath moveToPoint:NSMakePoint(NSMinX(rect)+4,NSMidY(rect)+2)];
	[wavePath lineToPoint:NSMakePoint(NSMidX(rect),NSMidY(rect)+10)];
	[wavePath lineToPoint:NSMakePoint(NSMaxX(rect)-4,NSMidY(rect)-15)];
	[wavePath setLineWidth:3.0];
	[wavePath stroke];
}

- (void) drawOnSmartBoostIconInRect:(NSRect)rect
{
	[[NSColor colorWithCalibratedRed:223.0/256.0 green:159.0/256.0 blue:35.0/256.0 alpha:0.4] setStroke];
	NSBezierPath *dottedPath = [NSBezierPath bezierPath];
	CGFloat array[2];
	array[0] = 5.0; //segment painted with stroke color
	array[1] = 2.0; //segment not painted with a color
	[dottedPath setLineDash: array count: 2 phase: 0.0];	
	[dottedPath moveToPoint:NSMakePoint(NSMinX(rect), NSMidY(rect))];
	[dottedPath lineToPoint:NSMakePoint(NSMaxX(rect), NSMidY(rect))];
	[dottedPath setLineWidth:2.0];
	[dottedPath stroke];
	
	[[NSColor colorWithCalibratedRed:223.0/256.0 green:159.0/256.0 blue:35.0/256.0 alpha:0.9] setStroke];
	NSBezierPath *wavePath = [NSBezierPath bezierPath];
	[wavePath moveToPoint:NSMakePoint(NSMinX(rect)+4,NSMidY(rect)-2)];
	[wavePath lineToPoint:NSMakePoint(NSMidX(rect),NSMidY(rect)-10)];
	[wavePath lineToPoint:NSMakePoint(NSMaxX(rect)-4,NSMidY(rect)+15)];
	[wavePath setLineWidth:3.0];
	[wavePath stroke];
}

#pragma mark "Animation Timer"

- (void) nextFrame
{
	animationT += 0.015;
	if (animationT > 1.0)
	{ animationT = 0; }
	[self setNeedsDisplay:YES];
}

#pragma mark "Device"

- (LCEntity *) device
{ return device; }

- (void) setDevice:(LCEntity *)newDevice
{
	device = [newDevice retain];
	
	[self bind:@"mainsVoltage" 
	  toObject:device 
   withKeyPath:@"childrenDictionary.apcinput.childrenDictionary.master.childrenDictionary.voltage.properties.valstr"
	   options:nil];
	[self bind:@"mainsFrequency" 
	  toObject:device 
   withKeyPath:@"childrenDictionary.apcinput.childrenDictionary.master.childrenDictionary.frequency.properties.valstr"
	   options:nil];

	[self bind:@"batteryCapacity" 
	  toObject:device 
   withKeyPath:@"childrenDictionary.apcbattery.childrenDictionary.master.childrenDictionary.capacity.properties.valstr"
	   options:nil];
	[self bind:@"batteryTemp" 
	  toObject:device 
   withKeyPath:@"childrenDictionary.apcbattery.childrenDictionary.master.childrenDictionary.temp.properties.valstr"
	   options:nil];
	[self bind:@"batteryTime" 
	  toObject:device 
   withKeyPath:@"childrenDictionary.apcbattery.childrenDictionary.master.childrenDictionary.runtime_remaining.properties.valstr"
	   options:nil];

	[self bind:@"outputStatus" 
	  toObject:device 
   withKeyPath:@"childrenDictionary.apcoutput.childrenDictionary.master.childrenDictionary.status.properties.valstr"
	   options:nil];
	[self bind:@"outputStatusInteger" 
	  toObject:device 
   withKeyPath:@"childrenDictionary.apcoutput.childrenDictionary.master.childrenDictionary.status.properties.valstr_raw"
	   options:nil];
	[self bind:@"outputVoltage" 
	  toObject:device 
   withKeyPath:@"childrenDictionary.apcoutput.childrenDictionary.master.childrenDictionary.voltage.properties.valstr"
	   options:nil];
	[self bind:@"outputFrequency" 
	  toObject:device 
   withKeyPath:@"childrenDictionary.apcoutput.childrenDictionary.master.childrenDictionary.frequency.properties.valstr"
	   options:nil];
	[self bind:@"outputLoad" 
	  toObject:device 
   withKeyPath:@"childrenDictionary.apcoutput.childrenDictionary.master.childrenDictionary.load.properties.valstr"
	   options:nil];
	[self bind:@"outputCurrent" 
	  toObject:device 
   withKeyPath:@"childrenDictionary.apcoutput.childrenDictionary.master.childrenDictionary.current.properties.valstr"
	   options:nil];
}

#pragma mark "Bound Accesors"

- (NSString *) mainsVoltage
{ return mainsVoltage; }
- (void) setMainsVoltage:(NSString *)string
{ [mainsVoltage release]; mainsVoltage = [string retain]; [self setNeedsDisplay:YES]; }

- (NSString *) mainsFrequency
{ return mainsFrequency; }
- (void) setMainsFrequency:(NSString *)string
{ [mainsFrequency release]; mainsFrequency = [string retain]; [self setNeedsDisplay:YES]; }

- (NSString *) batteryCapacity
{ return batteryCapacity; }
- (void) setBatteryCapacity:(NSString *)string
{ [batteryCapacity release]; batteryCapacity = [string retain]; [self setNeedsDisplay:YES]; }

- (NSString *) batteryTime
{ return batteryTime; }
- (void) setBatteryTime:(NSString *)string
{ [batteryTime release]; batteryTime = [string retain]; [self setNeedsDisplay:YES]; }

- (NSString *) batteryTemp
{ return batteryTemp; }
- (void) setBatteryTemp:(NSString *)string
{ [batteryTemp release]; batteryTemp = [string retain]; [self setNeedsDisplay:YES]; }

- (NSString *) outputStatus
{ return outputStatus; }
- (void) setOutputStatus:(NSString *)string
{ [outputStatus release]; outputStatus = [string retain]; [self setNeedsDisplay:YES]; }

- (NSString *) outputStatusInteger
{ return outputStatusInteger; }
- (void) setOutputStatusInteger:(NSString *)string
{ [outputStatusInteger release]; outputStatusInteger = [string retain]; [self setNeedsDisplay:YES]; }

- (NSString *) outputVoltage
{ return outputVoltage; }
- (void) setOutputVoltage:(NSString *)string
{ [outputVoltage release]; outputVoltage = [string retain]; [self setNeedsDisplay:YES]; }

- (NSString *) outputFrequency
{ return outputFrequency; }
- (void) setOutputFrequency:(NSString *)string
{ [outputFrequency release]; outputFrequency = [string retain]; [self setNeedsDisplay:YES]; }

- (NSString *) outputLoad
{ return outputLoad; }
- (void) setOutputLoad:(NSString *)string
{ [outputLoad release]; outputLoad = [string retain]; [self setNeedsDisplay:YES]; }

- (NSString *) outputCurrent
{ return outputCurrent; }
- (void) setOutputCurrent:(NSString *)string
{ [outputCurrent release]; outputCurrent = [string retain]; [self setNeedsDisplay:YES]; }

@synthesize animationT;
@synthesize animationTimer;
@end
