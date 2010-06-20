//
//  LCMetricGraphView.m
//  Lithium Console
//
//  Created by James Wilson on 1/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMetricGraphView.h"

#import "LCEntity.h"
#import "LCCustomer.h"
#import "LCMetricGraphController.h"
#import "LCMetricGraphDocument.h"
#import "LCEntityDescriptor.h"
#import "CALayer-PDF.h"

#import <QuartzCore/QuartzCore.h>

@implementation LCMetricGraphView

#pragma mark "Graph manipulation"

- (id) initWithFrame:(NSRect)frame
{
	[super initWithFrame:frame];
	
	[self setWantsLayer:YES];
	[self setAutoresizesSubviews:YES];
	self.layer.needsDisplayOnBoundsChange = YES;
	self.fontSize = 11.0;
	self.dragAndDropEnabled = YES;
	self.showDates = YES;

	/* Register for file and entity dragging */
    [self registerForDraggedTypes:[NSArray arrayWithObjects:NSPDFPboardType, NSFileContentsPboardType, @"LCEntityDescriptor", nil]];

	/* Create root layer */
	rootLayer = [CALayer layer];
	rootLayer.layoutManager = [CAConstraintLayoutManager layoutManager];
	rootLayer.name = @"root";
	rootLayer.delegate = self;
	rootLayer.frame = NSRectToCGRect([self bounds]);
	[rootLayer setNeedsDisplayOnBoundsChange:YES];
	
	/* Create Container Layer */
	CALayer *containerLayer = [CALayer layer];
	containerLayer.name = @"container";
	[containerLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMidX relativeTo:@"superlayer" attribute:kCAConstraintMidX]];
	[containerLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintWidth relativeTo:@"superlayer" attribute:kCAConstraintWidth offset:0]];
	[containerLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintHeight relativeTo:@"superlayer" attribute:kCAConstraintHeight offset:0]];
	[containerLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMinY relativeTo:@"superlayer" attribute:kCAConstraintMaxY offset:0]];
	[containerLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMaxY relativeTo:@"superlayer" attribute:kCAConstraintMinY offset:0]];
	[rootLayer addSublayer:containerLayer];
	
	// the central scrolling layer; this will contain the images
	bodyLayer = [CAScrollLayer layer];
	bodyLayer.name = @"body";
	bodyLayer.scrollMode = kCAScrollHorizontally;
	bodyLayer.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
	bodyLayer.layoutManager = self;
	
	[containerLayer addSublayer:bodyLayer];		

	/* Create Image layers */
	primaryImageLayer = [CALayer new];
	primaryImageLayer.layoutManager = [CAConstraintLayoutManager layoutManager];
	primaryImageLayer.name = @"primaryImage";
	primaryImageLayer.frame = CGRectMake(0.0, 0.0, self.frame.size.width, self.frame.size.height);
	primaryImageLayer.delegate = self;
	[bodyLayer addSublayer:primaryImageLayer];
	baselineImageLayer = [CALayer new];
	baselineImageLayer.layoutManager = [CAConstraintLayoutManager layoutManager];
	baselineImageLayer.name = @"baselineImage";
	baselineImageLayer.frame = CGRectMake(0.0, 0.0, self.frame.size.width, self.frame.size.height);
	baselineImageLayer.delegate = self;
	[baselineImageLayer setOpacity:0.3];
	[bodyLayer addSublayer:baselineImageLayer];
	
	[self setLayer:rootLayer];
	
	return self;
}

- (NSView *) hitTest:(NSPoint)point
{
	return self;
}

- (void) awakeFromNib
{
	
}

- (void) blankView
{
	/* Removes the graph image */
	[baselineImageView release];
	[primaryImageView release];
	[graphController.graphPDFRep release];
	graphController.graphPDFRep = nil;
	[self setNeedsDisplay:YES];
}

- (void) dealloc
{
	[graphController removeObserver:self forKeyPath:@"graphPDFRep"];
	[graphController removeObserver:self forKeyPath:@"baselinePDFRep"];
	[graphController release];
	[overlayLabel release];
	[primaryImageView release];
	[baselineImageView release];
	
	[super dealloc];
}

#pragma mark "KVO Method"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	if ([keyPath isEqualToString:@"graphPDFRep"])
	{
		/* Update primary image and redraw */
		[primaryImageLayer setNeedsDisplay];
	}
	if ([keyPath isEqualToString:@"baselinePDFRep"])
	{
		/* Update baseline image and scale to suit Y-axis */
		[baselineImageLayer setNeedsDisplay];

	}
	[rootLayer setNeedsDisplay];
}

#pragma mark "Layer Management"

- (void) layoutSublayersOfLayer:(CALayer *)layer
{
	[CATransaction begin];
	[CATransaction setValue:(id)kCFBooleanTrue
					 forKey:kCATransactionDisableActions];
	primaryImageLayer.frame = bodyLayer.bounds;
	[primaryImageLayer setNeedsDisplay];
	baselineImageLayer.frame = bodyLayer.bounds;
	[baselineImageLayer setNeedsDisplay];
	[CATransaction commit];
}

#pragma mark "Drawing Methods"

- (float) timeLineHeight
{ return 8.0f; }

- (float) border
{ return 4.0f; }

- (NSRect) graphRect
{
	if (self.style == GVSTYLE_STADIUM)
	{
		return [self bounds];
	}
	else
	{
		return NSMakeRect (NSMinX([self bounds]), 
						   NSMinY([self bounds]) + self.timeLineHeight + self.border, 
						   NSWidth(self.bounds), 
						   NSHeight(self.bounds) - self.timeLineHeight - (2 * self.border));
	}
}

- (NSRect) graphImageRect
{
	if (self.style == GVSTYLE_STADIUM)
	{
		return [self bounds];
	}
	else
	{
		return NSMakeRect(NSMinX(self.graphRect), 
						  NSMinY(self.graphRect) + (self.fontSize * 0.5f), 
						  NSWidth(self.graphRect), 
						  NSHeight(self.graphRect) - (self.fontSize));
	}	
}

- (NSString *) scaledValueString:(float)units
{
	float k = 1000;
	float m = 1000000;
	float g = 1000000000;
	
	if (units < k)
	{ return [NSString stringWithFormat:@"%.2f", units]; }
	else if (units < m)
	{ return [NSString stringWithFormat:@"%.2fK", units / k]; }
	else if (units < g)
	{ return [NSString stringWithFormat:@"%.2fM", units / m]; }
	else if ((units / 1024) < g)
	{ return [NSString stringWithFormat:@"%.2fG", units / g]; }
	else
	{ return [NSString stringWithFormat:@"%.2fT", (units / g) * (1 / 1000)]; }
}


- (void)drawLayer:(CALayer *)theLayer
        inContext:(CGContextRef)theContext
{
	if (theLayer == primaryImageLayer || theLayer == baselineImageLayer)
	{
		NSPDFImageRep *imageRep = nil;
		if (theLayer == primaryImageLayer) 
		{ imageRep = graphController.graphPDFRep; }
		else if (theLayer == baselineImageLayer) 
		{ 
			imageRep = graphController.baselinePDFRep; 
		}
		if (imageRep)
		{
			NSGraphicsContext *nsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:theContext flipped:NO];
			[NSGraphicsContext saveGraphicsState];
			[NSGraphicsContext setCurrentContext:nsContext];
			[imageRep drawInRect:[self graphImageRect]];
			[NSGraphicsContext restoreGraphicsState];
		}
	}
	else 
	{
		/* Background */
		NSGraphicsContext *nsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:theContext flipped:NO];
		[NSGraphicsContext saveGraphicsState];
		[NSGraphicsContext setCurrentContext:nsContext];
		
		/* String attributes */
		NSDictionary *stringAttr = [NSDictionary dictionaryWithObjectsAndKeys:
									[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.5], NSForegroundColorAttributeName,
									[NSFont systemFontOfSize:self.fontSize], NSFontAttributeName,
									nil];	
		
		/*
		 * Background and Timeline
		 */
		
		if (self.style == GVSTYLE_DEFAULT)
		{
			
			/* Timeline */
			NSRect dateLineRect = NSMakeRect(NSMinX([self bounds]), NSMinY([self bounds])+2.0, NSWidth([self bounds]), self.timeLineHeight-2.0);
			
			/* Horizontal Grid Lines */
			int horizGridCount = 8;
			int i;
			for (i=0; i <= horizGridCount; i++)
			{
				float alpha;
				if (i == 0 || i == horizGridCount || i == (horizGridCount / 2))
				{ alpha = 0.2; }
				else if (i % 2 == 0)
				{ alpha = 0.1; }
				else
				{ alpha = 0.05; }
				NSBezierPath *path = [NSBezierPath bezierPath];
				float lineY = lroundf(NSMinY(self.graphImageRect) + (i * (NSHeight(self.graphImageRect) / horizGridCount))) + 0.5f;
				[path moveToPoint:NSMakePoint(NSMinX(self.graphImageRect), lineY)];
				[path lineToPoint:NSMakePoint(NSMaxX(self.graphImageRect), lineY)];
				[path setLineWidth:1.0];
				[[NSColor colorWithCalibratedWhite:0.0 alpha:alpha] setStroke];
				[path stroke];
				path = [NSBezierPath bezierPath];
				[path moveToPoint:NSMakePoint(NSMinX(self.graphImageRect), lineY-1)];
				[path lineToPoint:NSMakePoint(NSMaxX(self.graphImageRect), lineY-1)];
				[path setLineWidth:1.0];
				[[NSColor colorWithCalibratedWhite:0.6 alpha:alpha] setStroke];
				[path stroke];			
			}
			
			/* Vertical Shading */
			
			/* Draw time line */
			NSDictionary *textAttr = [NSDictionary dictionaryWithObjectsAndKeys:
									  [NSColor colorWithCalibratedWhite:1.0 alpha:0.5], NSForegroundColorAttributeName,
									  [NSFont boldSystemFontOfSize:self.fontSize], NSFontAttributeName,
									  nil];
			NSDictionary *shadowAttr = [NSDictionary dictionaryWithObjectsAndKeys:
										[NSColor colorWithCalibratedWhite:0.0 alpha:0.2], NSForegroundColorAttributeName,
										[NSFont boldSystemFontOfSize:self.fontSize], NSFontAttributeName,
										nil];			
			if (graphController.graphPeriod == 1 || graphController.graphPeriod == 2)
			{
				/* 48 Hours til now or specific date */
				float pixelPerMinute = NSWidth(self.graphImageRect) / (48.0f * 60.0f);
				NSCalendar *calendar = [[[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar] autorelease];
				NSDate *latestDate;
				if (graphController.graphPeriod == 1)
				{ 					
					latestDate = [NSDate date]; 
				}
				else
				{ 
					NSDateComponents *referenceComponents = [calendar components:NSDayCalendarUnit|NSMonthCalendarUnit|NSYearCalendarUnit fromDate:graphController.referenceDate];
					[referenceComponents setHour:0];
					[referenceComponents setMinute:0];
					[referenceComponents setSecond:0];
					latestDate = [calendar dateFromComponents:referenceComponents];
					NSDateComponents *latestDateDelta = [[[NSDateComponents alloc] init] autorelease];
					[latestDateDelta setDay:1];
					latestDate = [calendar dateByAddingComponents:latestDateDelta toDate:latestDate options:0];
				}
				
				NSDateComponents *nowMinuteComp = [calendar components:NSMinuteCalendarUnit fromDate:latestDate];
				float minuteAhead = (float) [nowMinuteComp minute];
				[nowMinuteComp setMinute:([nowMinuteComp minute] * -1)];
				NSDate *nearestHour = [calendar dateByAddingComponents:nowMinuteComp toDate:latestDate options:0];
				NSString *nearestHourString = [NSString stringWithFormat:@"00:00"];
				NSSize stringSize = [nearestHourString sizeWithAttributes:stringAttr];
				NSRect nearestHourRect = NSMakeRect(NSMaxX(self.graphImageRect) - (minuteAhead * pixelPerMinute) - (stringSize.width * 0.5f),
													NSMinY(dateLineRect), stringSize.width, stringSize.height);
				[[NSColor colorWithCalibratedWhite:1.0 alpha:0.6] setStroke];
				[[NSColor colorWithCalibratedWhite:1.0 alpha:0.6] setFill];
				int hourIncrement;
				if ([self frame].size.width > 350.0)
				{ hourIncrement = 4; }
				else
				{ hourIncrement = 8; }				
				int verticalSections = (48 / hourIncrement);
				for (i=0; i < verticalSections; i++)
				{
					NSRect section = NSMakeRect(NSMinX(self.graphImageRect) + (((float) i) * (NSWidth(self.graphImageRect) / ((float) verticalSections))) - (minuteAhead * pixelPerMinute),
												NSMinY(self.graphImageRect), NSWidth(self.graphImageRect) / ((float) verticalSections), NSHeight(self.graphImageRect));
					NSBezierPath *sectionPath = [NSBezierPath bezierPathWithRect:section];
					if (i % 2)
					{
						[[NSColor colorWithCalibratedWhite:0.0 alpha:0.1] setFill];
					}
					else
					{
						[[NSColor colorWithCalibratedWhite:0.0 alpha:0.02] setFill];
					}
					[sectionPath fill];
				}
				int lastHour = 24;
				for (i=0; i < (48 / hourIncrement); i++)
				{
					NSDateComponents *delta = [[[NSDateComponents alloc] init] autorelease];
					[delta setHour:(((hourIncrement) * -1) * i)];
					NSDate *hourDate = [calendar dateByAddingComponents:delta toDate:nearestHour options:0];
					NSDateComponents *hourDateComp = [calendar components:NSHourCalendarUnit|NSMinuteCalendarUnit fromDate:hourDate];
					NSString *hourString = [NSString stringWithFormat:@"%.2d:00 ", [hourDateComp hour]];
					NSSize hourStringSize = [hourString sizeWithAttributes:stringAttr];
					float xOffset = (((float) i) * (pixelPerMinute * hourIncrement * 60.0f));
					NSRect hourRect = NSMakeRect (roundf(NSMinX(nearestHourRect) - xOffset), roundf(NSMinY(nearestHourRect)), 
												  hourStringSize.width, hourStringSize.height);
					
					NSRect shadowRect = NSMakeRect(NSMinX(hourRect), NSMinY(hourRect) + 0.5, NSWidth(hourRect), NSHeight(hourRect));
					[hourString drawInRect:shadowRect withAttributes:shadowAttr];
					[hourString drawInRect:hourRect withAttributes:textAttr];			
					
					/* Check for centre line */
					if (lastHour < [hourDateComp hour] || [hourDateComp hour] == 0)
					{
						/* Center line found!! */
						NSDate *midnightHourDate;
						CGFloat xOffsetQ = 0.0;
						if ([hourDateComp hour] != 0)
						{
							/* 00:00 is between this time and the previous */
							xOffsetQ = 1.0 - ((CGFloat) lastHour / (CGFloat) hourIncrement);
							NSDateComponents *midnightHourDelta = [[[NSDateComponents alloc] init] autorelease];
							[midnightHourDelta setHour:24 - [hourDateComp hour]];
							midnightHourDate = [calendar dateByAddingComponents:midnightHourDelta toDate:hourDate options:0];
						}
						else
						{
							midnightHourDate = hourDate;
						}

						/* Draw midnight line */
						NSRect midnightRect = NSMakeRect(roundf(NSMidX(hourRect) + (xOffsetQ * (pixelPerMinute * hourIncrement * 60.0f))) + 0.5, 
														 NSMinY(self.graphImageRect),
														 1.0, NSHeight(self.graphImageRect));
						NSBezierPath *midnightPath = [NSBezierPath bezierPath];
						[midnightPath moveToPoint:NSMakePoint(midnightRect.origin.x-1.0, NSMinY(midnightRect))];
						[midnightPath lineToPoint:NSMakePoint(midnightRect.origin.x-1.0, NSMaxY(midnightRect))];						
						[[NSColor colorWithCalibratedWhite:0.0 alpha:0.2] setStroke];
						[midnightPath stroke];
						midnightPath = [NSBezierPath bezierPath];
						[midnightPath moveToPoint:NSMakePoint(midnightRect.origin.x, NSMinY(midnightRect))];
						[midnightPath lineToPoint:NSMakePoint(midnightRect.origin.x, NSMaxY(midnightRect))];						
						[[NSColor colorWithCalibratedWhite:0.6 alpha:0.2] setStroke];
						[midnightPath stroke];
						
						/* 
						 * Draw Dates 
						 */

						if (self.showDates)
						{
							NSDateComponents *dayComponents = [[[NSDateComponents alloc] init] autorelease];
							CGFloat dateStringXOffset = 10.0;							
							NSDateFormatter *shortDateFormatter = [[[NSDateFormatter alloc] init] autorelease];
							[shortDateFormatter setDateStyle:NSDateFormatterShortStyle];
							[shortDateFormatter setTimeStyle:NSDateFormatterNoStyle];
							NSDate *dayDate = nil;
							
							/* Previous Day */
							[dayComponents setDay:-1];
							dayDate = [calendar dateByAddingComponents:dayComponents toDate:midnightHourDate options:0];							
							NSString *prevDateString = [shortDateFormatter stringFromDate:dayDate];
							NSDictionary *dateStringAttr = [NSDictionary dictionaryWithObjectsAndKeys:
															[NSColor colorWithCalibratedRed:0.4 green:0.4 blue:0.4 alpha:0.8], NSForegroundColorAttributeName,
															[NSFont boldSystemFontOfSize:self.fontSize + 2.0], NSFontAttributeName,
															nil];
							NSSize dateStringSize = [prevDateString sizeWithAttributes:dateStringAttr];
							NSRect prevDateRect = NSMakeRect(NSMinX(midnightRect)-dateStringSize.width - dateStringXOffset, NSMinY(midnightRect)+(NSHeight(midnightRect)*0.3)-(dateStringSize.height * 0.5),
															  dateStringSize.width, dateStringSize.height);
							[prevDateString drawInRect:prevDateRect withAttributes:dateStringAttr];
							
							/* Next Day */
							[dayComponents setDay:0];
							dayDate = [calendar dateByAddingComponents:dayComponents toDate:midnightHourDate options:0];
							NSString *nextDateString = [shortDateFormatter stringFromDate:dayDate];
							dateStringSize = [nextDateString sizeWithAttributes:dateStringAttr];
							NSRect nextDateRect = NSMakeRect(NSMinX(midnightRect)+1.0+dateStringXOffset, NSMinY(midnightRect)+(NSHeight(midnightRect)*0.3)-(dateStringSize.height * 0.5),
															 dateStringSize.width, dateStringSize.height);
							[nextDateString drawInRect:nextDateRect withAttributes:dateStringAttr];
						}
					}
					if ([hourDateComp hour] == 0) lastHour = 24;
					else lastHour = [hourDateComp hour];
				}		
			}
			else if (graphController.graphPeriod == 3)
			{
				/* 7 Day Week-of */
				int verticalSections = 7;
				float sectionWidth = NSWidth(self.graphImageRect) / ((float) verticalSections);
				for (i=0; i < verticalSections; i++)
				{
					/* Section background */
					NSRect section = NSMakeRect(NSMinX(self.graphImageRect) + (i * sectionWidth), NSMinY(self.graphImageRect), sectionWidth, NSHeight(self.graphImageRect));
					NSBezierPath *sectionPath = [NSBezierPath bezierPathWithRect:section];
					if (i % 2)
					{ [[NSColor colorWithCalibratedWhite:0.0 alpha:0.1] setFill]; }
					else
					{ [[NSColor colorWithCalibratedWhite:0.0 alpha:0.02] setFill]; }
					[sectionPath fill];
					
					/* Label */
					NSString *labelString;
					switch (i)
					{
						case 0:
							labelString = @"Mon";
							break;
						case 1:
							labelString = @"Tue";
							break;
						case 2:
							labelString = @"Wed";
							break;
						case 3:
							labelString = @"Thu";
							break;
						case 4:
							labelString = @"Fri";
							break;
						case 5:
							labelString = @"Sat";
							break;
						case 6:
							labelString = @"Sun";
							break;
					}
					NSSize labelSize = [labelString sizeWithAttributes:textAttr];
					NSRect labelRect = NSMakeRect (roundf(NSMidX(section) - (labelSize.width * 0.5f)),  roundf(NSMinY(dateLineRect)), 
												   labelSize.width, labelSize.height);
					NSRect shadowRect = NSMakeRect(NSMinX(labelRect) + 0.5, NSMinY(labelRect) - 0.5, NSWidth(labelRect), NSHeight(labelRect));
					[labelString drawInRect:shadowRect withAttributes:shadowAttr];
					[labelString drawInRect:labelRect withAttributes:textAttr];			
				}		
			}
			else if (graphController.graphPeriod == 4)
			{
				/* Month of */
				NSCalendar *calendar = [[[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar]autorelease];
				NSRange dayRange = [calendar rangeOfUnit:NSDayCalendarUnit inUnit:NSMonthCalendarUnit forDate:graphController.referenceDate];
				int verticalSections = dayRange.length;
				float sectionWidth = NSWidth(self.graphImageRect) / ((float) verticalSections);
				for (i=0; i < verticalSections; i++)
				{
					/* Section background */
					NSRect section = NSMakeRect(NSMinX(self.graphImageRect) + (i * sectionWidth), NSMinY(self.graphImageRect), sectionWidth, NSHeight(self.graphImageRect));
					NSBezierPath *sectionPath = [NSBezierPath bezierPathWithRect:section];
					if (i % 2)
					{ [[NSColor colorWithCalibratedWhite:0.0 alpha:0.1] setFill]; }
					else
					{ [[NSColor colorWithCalibratedWhite:0.0 alpha:0.02] setFill]; }
					[sectionPath fill];
					
					/* Label */
					NSString *labelString = [NSString stringWithFormat:@"%i", i+1];
					NSSize labelSize = [labelString sizeWithAttributes:textAttr];
					NSRect labelRect = NSMakeRect (roundf(NSMidX(section) - (labelSize.width * 0.5f)),  roundf(NSMinY(dateLineRect)), 
												   labelSize.width, labelSize.height);
					NSRect shadowRect = NSMakeRect(NSMinX(labelRect) + 0.5, NSMinY(labelRect) - 0.5, NSWidth(labelRect), NSHeight(labelRect));
					[labelString drawInRect:shadowRect withAttributes:shadowAttr];
					[labelString drawInRect:labelRect withAttributes:textAttr];			
				}		
			}			
			else if (graphController.graphPeriod == 5)
			{
				/* Year of */
				int verticalSections = 12;
				float sectionWidth = NSWidth(self.graphImageRect) / ((float) verticalSections);
				for (i=0; i < verticalSections; i++)
				{
					/* Section background */
					NSRect section = NSMakeRect(NSMinX(self.graphImageRect) + (i * sectionWidth), NSMinY(self.graphImageRect), sectionWidth, NSHeight(self.graphImageRect));
					NSBezierPath *sectionPath = [NSBezierPath bezierPathWithRect:section];
					if (i % 2)
					{ [[NSColor colorWithCalibratedWhite:0.0 alpha:0.1] setFill]; }
					else
					{ [[NSColor colorWithCalibratedWhite:0.0 alpha:0.02] setFill]; }
					[sectionPath fill];
					
					/* Label */
					NSString *labelString;
					switch (i)
					{
						case 0:
							labelString = @"Jan";
							break;
						case 1:
							labelString = @"Feb";
							break;
						case 2:
							labelString = @"Mar";
							break;
						case 3:
							labelString = @"Apr";
							break;
						case 4:
							labelString = @"May";
							break;
						case 5:
							labelString = @"Jun";
							break;
						case 6:
							labelString = @"Jul";
							break;
						case 7:
							labelString = @"Aug";
							break;
						case 8:
							labelString = @"Sep";
							break;
						case 9:
							labelString = @"Oct";
							break;
						case 10:
							labelString = @"Nov";
							break;
						case 11:
							labelString = @"Dec";
							break;						
					}
					NSSize labelSize = [labelString sizeWithAttributes:textAttr];
					NSRect labelRect = NSMakeRect (roundf(NSMidX(section) - (labelSize.width * 0.5f)),  roundf(NSMinY(dateLineRect)), 
												   labelSize.width, labelSize.height);
					NSRect shadowRect = NSMakeRect(NSMinX(labelRect) + 0.5, NSMinY(labelRect) - 0.5, NSWidth(labelRect), NSHeight(labelRect));
					[labelString drawInRect:shadowRect withAttributes:shadowAttr];
					[labelString drawInRect:labelRect withAttributes:textAttr];			
				}		
			}
		}
		else if (self.style == GVSTYLE_STADIUM)
		{
			NSBezierPath *backPath = [NSBezierPath bezierPathWithRoundedRect:[self bounds] xRadius:4.0 yRadius:4.0];
			NSGradient *backGradient = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedWhite:0.02 alpha:0.8] endingColor:[NSColor colorWithCalibratedWhite:0.05 alpha:0.8]];
			[backGradient drawInBezierPath:backPath angle:90.0];
			[backGradient autorelease];
		}
		
		/*
		 * Min/Avg/Max Values
		 */
		
		if (self.style == GVSTYLE_DEFAULT)
		{
			CGFloat yAdjustment = 0.0;
			if (self.fontSize == 11.0) yAdjustment = 0.5;
			else if (self.fontSize == 9.0) yAdjustment = 1.0;
			
			/* Draw Max */
			[[NSColor colorWithCalibratedWhite:1.0 alpha:0.6] setStroke];
			[[NSColor colorWithCalibratedWhite:1.0 alpha:0.6] setFill];
			NSString *valString = [self scaledValueString:graphController.maxValue];
			NSSize stringSize = [valString sizeWithAttributes:stringAttr];
			NSRect stringRect = NSMakeRect(NSMinX(self.graphRect) + 5.0, NSMaxY(self.graphRect) - stringSize.height + yAdjustment, stringSize.width, stringSize.height); 
			NSRect backingRect = NSMakeRect(NSMinX(self.graphRect)+ 1.0, NSMaxY(self.graphRect) - stringSize.height, stringSize.width + 8.0, stringSize.height);  
			NSBezierPath *backPath = [NSBezierPath bezierPathWithRoundedRect:backingRect xRadius:8 yRadius:8];
			[[NSColor colorWithCalibratedWhite:0.0 alpha:0.4] setFill];
			[backPath fill];
			[valString drawInRect:stringRect withAttributes:stringAttr];
			
			/* Draw Min */
			valString = [self scaledValueString:graphController.minValue];
			stringSize = [valString sizeWithAttributes:stringAttr];
			stringRect = NSMakeRect(NSMinX(self.graphRect) + 5.0, NSMinY(self.graphRect)+yAdjustment, stringSize.width, stringSize.height); 
			backingRect = NSMakeRect(NSMinX(self.graphRect)+ 1.0, NSMinY(self.graphRect), stringSize.width + 8.0, stringSize.height);  
			backPath = [NSBezierPath bezierPathWithRoundedRect:backingRect xRadius:8 yRadius:8];
			[[NSColor colorWithCalibratedWhite:0.0 alpha:0.4] setFill];
			[backPath fill];
			[valString drawInRect:stringRect withAttributes:stringAttr];
			
			/* Draw Mid */
			valString = [self scaledValueString:graphController.minValue + ((graphController.maxValue - graphController.minValue) / 2.0f)];
			stringSize = [valString sizeWithAttributes:stringAttr];
			stringRect = NSMakeRect(NSMinX(self.graphRect) + 5.0, NSMidY(self.graphRect) - (stringSize.height * 0.5f) + yAdjustment, stringSize.width, stringSize.height); 
			backingRect = NSMakeRect(NSMinX(self.graphRect)+ 1.0, NSMidY(self.graphRect) - (stringSize.height * 0.5f), stringSize.width + 8.0, stringSize.height);  
			backPath = [NSBezierPath bezierPathWithRoundedRect:backingRect xRadius:8 yRadius:8];
			[[NSColor colorWithCalibratedWhite:0.0 alpha:0.4] setFill];
			[backPath fill];
			[valString drawInRect:stringRect withAttributes:stringAttr];
		}
		
		/* Check for display of warning */
		if (showTooManySourcesWarning && self.style == GVSTYLE_DEFAULT)
		{
			[[NSColor colorWithDeviceWhite:0.2 alpha:0.9] setFill];
			NSBezierPath *backPath = [NSBezierPath bezierPathWithRect:[self bounds]];
			[backPath fill];
			
			/* Default text attributes */
			NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
								  [NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:1.0], NSForegroundColorAttributeName,
								  [NSFont fontWithName:@"Lucida Grande" size:18.0], NSFontAttributeName,
								  nil];
			
			/* Desc */
			NSString *string = @"Error: Too many data sources specified";
			NSSize stringSize = [string sizeWithAttributes:attr];
			[string drawAtPoint:NSMakePoint(NSMidX([self bounds]) - (0.5 * stringSize.width),
											NSMidY([self bounds]) - (0.5 * stringSize.height)) withAttributes:attr];
			string = @"Each graph can only display up to 9 metrics.";
			attr = [NSDictionary dictionaryWithObjectsAndKeys:
					[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:1.0], NSForegroundColorAttributeName,
					[NSFont fontWithName:@"Lucida Grande" size:14.0], NSFontAttributeName,
					nil];
			stringSize = [string sizeWithAttributes:attr];
			[string drawAtPoint:NSMakePoint(NSMidX([self bounds]) - (0.5 * stringSize.width),
											NSMidY([self bounds]) - (0.5 * stringSize.height) - 22) withAttributes:attr];			
			string = @"Metrics can be removed from the graph using the table view below.";
			attr = [NSDictionary dictionaryWithObjectsAndKeys:
					[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:1.0], NSForegroundColorAttributeName,
					[NSFont fontWithName:@"Lucida Grande" size:12.0], NSFontAttributeName,
					nil];
			stringSize = [string sizeWithAttributes:attr];
			[string drawAtPoint:NSMakePoint(NSMidX([self bounds]) - (0.5 * stringSize.width),
											NSMinY([self bounds]) - (0.5 * stringSize.height) + 14) withAttributes:attr];			
			
		}
		
		/* Draw Overlay label */
		if ([self overlayLabel])
		{
			NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
								  [NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.4], NSForegroundColorAttributeName,
								  [NSFont fontWithName:@"Lucida Grande" size:14.0], NSFontAttributeName,
								  nil];
			NSSize textSize = [overlayLabel sizeWithAttributes:attr];
			NSRect textRect = NSMakeRect(NSMidX([self bounds]) - (0.5 * textSize.width),
										 NSMidY([self bounds]) - (0.5 * textSize.height),
										 textSize.width,textSize.height);
			[[self overlayLabel] drawInRect:textRect withAttributes:attr];
		}
		

		/* Restore context */
		[NSGraphicsContext restoreGraphicsState];
	}

}

#pragma mark "Dragging Methods"

- (void)mouseDown:(NSEvent *)theEvent
{
	if (dragAndDropEnabled && graphController.graphPDFRep && entityArrayController && [theEvent type] == NSLeftMouseDown && [theEvent clickCount] == 1)
	{
		/* Promised File Drag */
		NSPoint dragPosition;
		NSRect imageLocation;
		dragPosition = [self convertPoint:[theEvent locationInWindow]
								 fromView:nil];
		dragPosition.x -= 16;
		dragPosition.y -= 16;
		imageLocation.origin = dragPosition;
		imageLocation.size = NSMakeSize(32,32);
		NSArray *array = [NSArray arrayWithObject:@"pdf"];
		[self dragPromisedFilesOfTypes:array
							  fromRect:imageLocation
								source:self
							 slideBack:YES
								 event:theEvent];
	}
	else if ([theEvent type] == NSLeftMouseDown && [theEvent clickCount] == 2)
	{
		LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
		NSMutableArray *entityArray = [NSMutableArray array];
		for (LCMetricGraphMetricItem *item in graphController.metricItems)
		{ [entityArray addObject:item.metric]; }
		document.initialEntities = entityArray;
		[[NSDocumentController sharedDocumentController] addDocument:document];
		[document makeWindowControllers];
		[document showWindows];			
	}
	else
	{ [super mouseDown:theEvent]; }
}

- (NSArray *)namesOfPromisedFilesDroppedAtDestination:(NSURL *)dropDestination
{
	int duplicateIndex = 0;
	NSString *fileName = @"LithiumGraph.pdf";

	/* Check whether or not the basic file exists */
	while ([[NSFileManager defaultManager] fileExistsAtPath:[[NSURL URLWithString:fileName relativeToURL:dropDestination] path]])
	{
		/* File exists, create a new filename with a integer */
		duplicateIndex++;
		fileName = [NSString stringWithFormat:@"LithiumGraph(%i).pdf", duplicateIndex];		
	}
	
	/* Write the file */
//	[[graphController.graphPDFRep PDFRepresentation] writeToURL:[NSURL URLWithString:fileName relativeToURL:dropDestination] atomically:NO];

	[[[self layer] dataForPDFRepresentationOfLayer] writeToURL:[NSURL URLWithString:fileName relativeToURL:dropDestination] atomically:NO];
	
	/* Return file written */
	return [NSArray arrayWithObject:fileName];
}

- (void)dragImage:(NSImage *)anImage at:(NSPoint)imageLoc offset:(NSSize)mouseOffset event:(NSEvent *)theEvent pasteboard:(NSPasteboard *)pboard source:(id)sourceObject slideBack:(BOOL)slideBack
{
	dragSource = sourceObject;
//	[pboard addTypes:[NSArray arrayWithObjects:NSPDFPboardType, NSFileContentsPboardType, @"LCEntityDescriptor", nil] owner:self];
	[pboard addTypes:[NSArray arrayWithObjects:NSPDFPboardType, NSFileContentsPboardType, nil] owner:self];
	[super dragImage:anImage at:imageLoc offset:mouseOffset event:theEvent pasteboard:pboard source:sourceObject slideBack:slideBack];
}

- (void)pasteboard:(NSPasteboard *)pboard provideDataForType:(NSString *)type
{
    if ([type compare: NSPDFPboardType]==NSOrderedSame)
	{ 
		[pboard setData:[graphController.graphPDFRep PDFRepresentation] forType:NSPDFPboardType];
	}
	else if  ([type compare: NSFileContentsPboardType]==NSOrderedSame)
	{
		[pboard setData:[graphController.graphPDFRep PDFRepresentation] forType:NSFileContentsPboardType];
	}
//	else if ([type isEqualToString:@"LCEntityDescriptor"])
//	{
//		/* Create paste-data property list */
//		NSMutableArray *propertyList = [NSMutableArray array];
//		for (LCMetricGraphMetricItem *metricItem in [entityArrayController content])
//		{		
//			/* Create entity descriptor */
//			LCEntityDescriptor *entityDesc = [LCEntityDescriptor descriptorForEntity:(LCEntity *) metricItem.metric];
//			
//			/* Create dup properties dictionary */
//			NSDictionary *properties = [NSDictionary dictionaryWithDictionary:[entityDesc properties]];
//			
//			/* Add properties to list */
//			[propertyList addObject:properties];
//		}
//	
//	[pboard declareTypes:[NSArray arrayWithObject:@"LCEntityDescriptor"] owner:self];
//	[pboard setPropertyList:propertyList forType:@"LCEntityDescriptor"];
//	}		
}

- (BOOL)mouseDownCanMoveWindow
{
	return NO;
}

#pragma mark "Drag and Drop"

- (NSDragOperation)draggingSourceOperationMaskForLocal:(BOOL)isLocal
{
	return NSDragOperationCopy;
}

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)info 
{
	if (dragAndDropEnabled && entityArrayController && [info draggingSource] != dragSource)
	{
		/* Get Entities */
		NSArray *entityDescriptorArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
		if (([[entityArrayController arrangedObjects] count] + [entityDescriptorArray count])>= 9)
		{ 
			showTooManySourcesWarning = YES;
			[self setNeedsDisplay:YES];
			return NSDragOperationNone; 
		}
		return NSDragOperationCopy;
	}
	else
	{ return NSDragOperationNone; }
}

- (void)draggingExited:(id <NSDraggingInfo>)sender
{
	/* Hide warning */
	showTooManySourcesWarning = NO;
	[self setNeedsDisplay:YES];
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)info
{
	/* Check for local */
	if ([info draggingSource] == dragSource)
	{ return NO; }
	
	
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
			/* A trigger was dropped, use the metric instead */
			dropEntity = [dropEntity metric];
		}
		
		/* Add metrics */
		NSArray *metricArray = [LCMetricGraphController graphableMetricsForEntities:[NSArray arrayWithObject:dropEntity]];
		NSEnumerator *metricEnum = [metricArray objectEnumerator];
		LCMetric *metric;
		while (metric = [metricEnum nextObject])
		{
			LCMetricGraphMetricItem *metricItem = [LCMetricGraphMetricItem itemForMetric:metric];
			[entityArrayController insertObject:metricItem atArrangedObjectIndex:[[entityArrayController arrangedObjects] count]];
		}
	}
	
	[self setNeedsDisplay:YES];
	
    return YES;
}

#pragma mark "Menu Event Handling"

- (NSMenu *) menuForEvent:(NSEvent *)event
{
	return [self menu];
}

#pragma mark "Baseline Highlighting"

- (BOOL) highlightBaselineView
{ return highlightBaselineView; }
- (void) setHighlightBaselineView:(BOOL)flag
{
	if (!graphController.baselinePDFRep) return;
	
	highlightBaselineView = flag;
	if (highlightBaselineView)
	{
		[[self.baselineImageView animator] setAlphaValue:1.0];
		[[self.primaryImageView animator] setAlphaValue:0.1];
	}
	else
	{
		[[self.baselineImageView animator] setAlphaValue:0.3];
		[[self.primaryImageView animator] setAlphaValue:1.0];
	}
}

#pragma mark "Accessor Methods"

@synthesize overlayLabel;
- (void) setOverlayLabel:(NSString *)string
{
	[overlayLabel release];
	overlayLabel = [string retain];
	[self setNeedsDisplay:YES];
}

@synthesize showTooManySourcesWarning;
@synthesize graphController;
- (void) setGraphController:(LCMetricGraphController *)value
{
	[graphController removeObserver:self forKeyPath:@"graphPDFRep"];
	[graphController removeObserver:self forKeyPath:@"baselinePDFRep"];

	[graphController release];
	graphController = [value retain];

	[graphController addObserver:self
					  forKeyPath:@"graphPDFRep"
						 options:NSKeyValueObservingOptionNew
						 context:nil];
	[graphController addObserver:self
					  forKeyPath:@"baselinePDFRep"
						 options:NSKeyValueObservingOptionNew
						 context:nil];
}

@synthesize baselineImageView;
@synthesize primaryImageView;
@synthesize style;

@synthesize fontSize;

@synthesize dragAndDropEnabled;
@synthesize showDates;

@end
