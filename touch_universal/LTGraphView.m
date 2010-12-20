//
//  LTGraphView.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 11/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTGraphView.h"
#import "LTMetricGraphRequest.h"


@implementation LTGraphView

+ (Class)layerClass
{
	return [CATiledLayer class];
}

@synthesize metrics, minLabels, avgLabels, maxLabels;

- (id)initWithFrame:(CGRect)frame 
{
    if ((self = [super initWithFrame:frame])) 
	{
		CATiledLayer *tiledLayer = (CATiledLayer *) self.layer;
		tiledLayer.tileSize = CGSizeMake(512.0, 9000.0);
		tiledLayer.levelsOfDetail = 4;
		tiledLayer.levelsOfDetailBias = 1;
		tiledLayer.delegate = self;

		minLabels = [[NSMutableArray array] retain];
		avgLabels = [[NSMutableArray array] retain];
		maxLabels = [[NSMutableArray array] retain];
		
		self.opaque = NO;
		
		UILongPressGestureRecognizer *longRecog = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(graphLongTouch:)];
		[self addGestureRecognizer:longRecog];
		
    }
    return self;
}

- (void)dealloc 
{
	[graphRequestCache release];
	[metrics release];
	[minLabels release];
	[avgLabels release];
	[maxLabels release];
    [super dealloc];
}

- (void)drawLayer:(CATiledLayer *)layer inContext:(CGContextRef)ctx
{
	/* Determine draw size */
	CGRect clipRect = CGContextGetClipBoundingBox(ctx);
	NSLog (@"%@ Asked to draw in %@ (tileSize is %@)", self, NSStringFromCGRect(clipRect), NSStringFromCGSize(((CATiledLayer*)self.layer).tileSize));
	
	/* Dimensions */
	CGFloat zoomScale = 1.0;
	CGSize contentSize;
	if ([[self.superview class] isSubclassOfClass:[UIScrollView class]])
	{
		/* Drawing in a scrollview */
		UIScrollView *graphScrollView = (UIScrollView *) self.superview;
		contentSize = graphScrollView.contentSize;
		zoomScale = graphScrollView.zoomScale;
	}
	else
	{
		/* Drawing in a plain view (metric popup) */
		contentSize = self.superview.bounds.size;
		
		/* Check to see if the slice is sane. 
		 * For a non-scrollview drawing there seems to be 
		 * some odd calls to the delegate to draw in half
		 * or less of the viewable area before the 'real'
		 * calls come in for a slice that's the whole view 
		 * Ignore any clipRects that arent out full view 
		 */
		
		if (clipRect.origin.x != 0. || (clipRect.size.width != layer.tileSize.width && clipRect.size.width != contentSize.width)) 
		{
			NSLog (@"%@ Skipping bogus call to draw slice %@", self, NSStringFromCGRect(clipRect));
			return;
		}
	}

	/* Time orientation, by default the visible width of the graph is 24 hours */
	int visibleSeconds = 86400;		/* 24 Hours */
	CGFloat offset = contentSize.width - CGRectGetMaxX(clipRect);
	NSLog (@"offset is %f", offset);
	CGFloat secondsPerPixel = (zoomScale * visibleSeconds) / CGRectGetWidth(self.superview.frame);
	
	/* Check for cached request */
	if (!graphRequestCache) graphRequestCache = [[NSMutableDictionary dictionary] retain];
	LTMetricGraphRequest *graphReq = [graphRequestCache objectForKey:NSStringFromCGRect(clipRect)];
	if (graphReq && !graphReq.refreshInProgress)
	{
		/* Draw image */
		CGDataProviderRef provider = CGDataProviderCreateWithCFData((CFDataRef)[graphReq imageData]);
		if (provider)
		{
			CGContextSaveGState (ctx);
			
			CGFloat yScale = 1.0;
			CGFloat yOffset = 0.0;
			if (minMaxSet)
			{
				yOffset = layer.frame.size.height * (graphReq.minValue / (maxValue - minValue));
				
				yScale = 1.0 / (maxValue / (graphReq.maxValue - graphReq.minValue));
			}
			
			CGFloat graphImageMargin = 0.0;
			
			CGPDFDocumentRef documentRef = CGPDFDocumentCreateWithProvider(provider);
			CFRelease(provider);
			CGPDFPageRef pageRef = CGPDFDocumentGetPage(documentRef, 1);
			CGRect imageRect = CGRectMake(CGRectGetMinX(clipRect), graphImageMargin, 
										  clipRect.size.width, contentSize.height - (2 * graphImageMargin));
			NSLog (@"%@ using imagerect %@", self, NSStringFromCGRect(imageRect));


			CGContextTranslateCTM(ctx, 0.0, (imageRect.size.height  - yOffset));
			CGContextScaleCTM(ctx, 1.0, -1.0 * yScale);
			CGContextConcatCTM(ctx, CGPDFPageGetDrawingTransform(pageRef, kCGPDFCropBox, imageRect, 0, false));
			
			CGContextDrawPDFPage(ctx, pageRef);
			
			CGContextRestoreGState(ctx);
		}
	}
	else if (!graphReq)
	{
		/* Configure graph request */
		graphReq = [[LTMetricGraphRequest alloc] init];
		graphReq.delegate = self;
		[graphRequestCache setObject:graphReq forKey:NSStringFromCGRect(clipRect)];
		graphReq.size = CGSizeMake(clipRect.size.width, self.superview.frame.size.height);
		graphReq.endSec = (int) [graphStartDate timeIntervalSince1970] - (offset * secondsPerPixel);
		graphReq.startSec = graphReq.endSec - (clipRect.size.width * secondsPerPixel);
		NSLog (@"endSet=%li(%@) startSec=%li(%@)", graphReq.endSec, [NSDate dateWithTimeIntervalSince1970:graphReq.endSec], graphReq.startSec, [NSDate dateWithTimeIntervalSince1970:graphReq.startSec]);
		graphReq.rectToInvalidate = clipRect;
		[graphReq.metrics addObjectsFromArray:self.metrics];
		if (!graphReq.customer && graphReq.metrics.count > 0)
		{
			graphReq.customer = [[graphReq.metrics objectAtIndex:0] customer];
		}
		
		/* Perform refresh */
		[graphReq refresh];
	}
	
	/* Common date/time variables */
	NSDate *sliceEndDate = [NSDate dateWithTimeIntervalSince1970:([graphStartDate timeIntervalSince1970] - (offset * secondsPerPixel))];
	NSDateComponents *endDateComponents = [[NSCalendar currentCalendar] components:NSMinuteCalendarUnit|NSHourCalendarUnit fromDate:sliceEndDate];
	
	/* 
	 * Draw time line 
	 */
	
	/* Determine font size and offset */
	CGFloat timeFontSize = 14.0;
	if (self.bounds.size.height < 200.0) timeFontSize = 11.0;
	CGFloat hourLineYOffset = 40.0;
	if (self.bounds.size.height < 200.0) hourLineYOffset = 30.0;
	
	/* Find the hour/minutes at the end of the slice */
	int hour = [endDateComponents hour];
	int minuteOffset = [endDateComponents minute];
	NSString *hourString = [NSString stringWithFormat:@"%.2i:00", hour];
	CGSize hourStringSize = [hourString sizeWithFont:[UIFont fontWithName:@"Helvetica-Bold" size:timeFontSize] constrainedToSize:CGSizeMake(200., timeFontSize) lineBreakMode:UILineBreakModeClip];
	
	/* Calculate a reasonable hour interval (hours between the hour lines) */
	int hourInterval =  (visibleSeconds / (60.0 * 60.0)) / (CGRectGetWidth(self.superview.frame) / (hourStringSize.width * 2.));
	hourInterval += hourInterval % 2;	// Make sure the hour interval is an even number
	
	/* Calculate the first hour to be drawn in this slice. This hour should infact
	 * be the first hour *beyond* the end, to the right, of our slice. This is because
	 * the hour string is centered, meaning there might be a partially drawn hour in the preceeding 
	 * slice which overlaps with ours
	 */
	CGRect hourRect = CGRectMake(CGRectGetMaxX(clipRect) + (((60 - minuteOffset) * 60.0) / secondsPerPixel), 
								 CGRectGetHeight(self.superview.frame) - hourLineYOffset, 
								 hourStringSize.width, hourStringSize.height);
	CGContextSelectFont (ctx, "Helvetica-Bold", timeFontSize, kCGEncodingMacRoman);
	CGContextSetTextDrawingMode (ctx, kCGTextFill);
	CGContextSetTextMatrix(ctx, CGAffineTransformMake(1.0,0.0, 0.0, -1.0, 0.0, 0.0));
	while (CGRectGetMaxX(hourRect) > CGRectGetMinX(clipRect))
	{
		if (hour % hourInterval == 0)	// Only draw on hours that fit into the hourInterval 
		{
			/* Draw hour line */
			CGRect hourLineRect = CGRectMake(CGRectGetMinX(hourRect), CGRectGetMinY(self.bounds), 1.0, CGRectGetHeight(self.bounds));
			UIBezierPath *innerPath = [UIBezierPath bezierPathWithRect:hourLineRect];
			CGContextSetRGBFillColor(ctx, 0.0, 0.0, 0.0, 0.05);
			CGContextAddPath(ctx, innerPath.CGPath);
			CGContextDrawPath(ctx, kCGPathFill);
			UIBezierPath *outerPath = [UIBezierPath bezierPathWithRect:CGRectOffset(hourLineRect, 1.0, 0.0)];
			CGContextSetRGBFillColor(ctx, 1.0, 1.0, 1.0, 0.05);
			CGContextAddPath(ctx, outerPath.CGPath);
			CGContextDrawPath(ctx, kCGPathFill);
			
			/* Draw current hour */
			CGContextSetRGBFillColor (ctx, 1, 1, 1, .2);
			CGContextShowTextAtPoint (ctx, roundf(hourRect.origin.x - (hourRect.size.width * 0.5)), roundf(hourRect.origin.y), [hourString cStringUsingEncoding:NSUTF8StringEncoding], [hourString length]);
			CGContextSetRGBFillColor (ctx, 0, 0, 0, .8);
			CGContextShowTextAtPoint (ctx, roundf(hourRect.origin.x - (hourRect.size.width * 0.5)), roundf(hourRect.origin.y)-1, [hourString cStringUsingEncoding:NSUTF8StringEncoding], [hourString length]);
		}
		
		/* Move (back) to prev hour */
		hour -= 1;
		
		/* Handle 24:00/00:00 */
		if (hour < 1) hour = 24 + hour;
		if (hour == 24) hourString = [NSString stringWithFormat:@"00:00"];
		else hourString = [NSString stringWithFormat:@"%.2i:00", hour];
		
		/* Re-calc size */
		hourStringSize = [hourString sizeWithFont:[UIFont fontWithName:@"Helvetica-Bold" size:timeFontSize] constrainedToSize:CGSizeMake(100.0, timeFontSize) lineBreakMode:UILineBreakModeClip];
		
		/* Move the hour rect back 1 hour */
		hourRect = CGRectMake(hourRect.origin.x - ((60 * 60) / secondsPerPixel), hourRect.origin.y, 
							  hourStringSize.width, hourStringSize.height);
	}

	/* Draw date line */
	CGFloat dateLineYOffset = hourLineYOffset * 2.0;
	NSDate *dateToDraw = sliceEndDate;
	NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
	[formatter setFormatterBehavior:NSDateFormatterBehavior10_4];
	[formatter setDateStyle:NSDateFormatterMediumStyle];
	[formatter setTimeStyle:NSDateFormatterNoStyle];
	NSString *dateString = [formatter stringForObjectValue:dateToDraw];
	CGSize dateStringSize = [dateString sizeWithFont:[UIFont fontWithName:@"Helvetica-Bold" size:timeFontSize] constrainedToSize:CGSizeMake(200.0, timeFontSize) lineBreakMode:UILineBreakModeClip];
	CGRect dateRect = CGRectMake(CGRectGetMaxX(clipRect) - ((([endDateComponents minute] * 60.0) + ([endDateComponents hour] * 60.0 * 60.0)) / secondsPerPixel), 
								 CGRectGetHeight(self.superview.frame) - dateLineYOffset, 
								 dateStringSize.width, hourStringSize.height);
	CGContextSelectFont (ctx, "Helvetica-Bold", timeFontSize, kCGEncodingMacRoman);
	CGContextSetTextDrawingMode (ctx, kCGTextFill);
	CGContextSetTextMatrix(ctx, CGAffineTransformMake(1.0,0.0, 0.0, -1.0, 0.0, 0.0));
	while (CGRectGetMaxX(dateRect) > CGRectGetMinX(clipRect))
	{
		/* Draw current date */
		CGContextSetRGBFillColor (ctx, 1, 1, 1, .2);
		CGContextShowTextAtPoint (ctx, roundf(dateRect.origin.x), roundf(dateRect.origin.y), [dateString cStringUsingEncoding:NSUTF8StringEncoding], [dateString length]);
		CGContextSetRGBFillColor (ctx, 0, 0, 0, .8);
		CGContextShowTextAtPoint (ctx, roundf(dateRect.origin.x), roundf(dateRect.origin.y)-1, [dateString cStringUsingEncoding:NSUTF8StringEncoding], [dateString length]);
				
		/* Move (back) to prev date */
		dateToDraw = [dateToDraw dateByAddingTimeInterval:-86400.0];
		dateString = [formatter stringForObjectValue:dateToDraw];
		dateStringSize = [dateString sizeWithFont:[UIFont fontWithName:@"Helvetica-Bold" size:timeFontSize] constrainedToSize:CGSizeMake(120.0, timeFontSize) lineBreakMode:UILineBreakModeClip];
		dateRect = CGRectMake(dateRect.origin.x - (86400.0 / secondsPerPixel), dateRect.origin.y, 
							  dateStringSize.width, dateStringSize.height);
	}	
}

- (NSString *) scaledValueString:(CGFloat)value
{
	if (value > (1000. * 1000. * 1000))
	{ return [NSString stringWithFormat:@"%.2fG", value / (1000. * 1000. * 1000)]; }
	else if (value > (1000. * 1000.))
	{ return [NSString stringWithFormat:@"%.2fM", value / (1000. * 1000.)]; }
	else if (value > (1000.))
	{ return [NSString stringWithFormat:@"%.2fk", value / 1000.]; }
	else 
	{ return [NSString stringWithFormat:@"%.2f", value]; }
}

- (void) updateMinMaxLabels
{
	for (UILabel *label in minLabels)
	{
		label.text = [self scaledValueString:minValue];
	}
	for (UILabel *label in maxLabels)
	{
		label.text = [self scaledValueString:maxValue];
	}
	CGFloat avgValue = minValue + ((maxValue - minValue)*0.5);
	for (UILabel *label in avgLabels)
	{
		label.text = [self scaledValueString:avgValue];
	}
}

- (void) apiCallDidFinish:(LTMetricGraphRequest *)graphReq
{
	[self.layer setNeedsDisplayInRect:graphReq.rectToInvalidate];
	
	/* Determine scale based on min/max */
	invalidated = NO;
	if (minMaxSet)
	{
		if (graphReq.minValue < minValue)
		{
			/* New min value... invalidate layer */
			[self.layer setNeedsDisplayInRect:self.layer.bounds];
			invalidated = YES;
			minValue = graphReq.minValue;
		}
		if (graphReq.maxValue > maxValue)
		{
			/* New max value.. invalidate layer */
			invalidated = YES;
			[self.layer setNeedsDisplayInRect:self.layer.bounds];
			maxValue = graphReq.maxValue;
		}
	}
	else
	{
		minValue = graphReq.minValue;
		maxValue = graphReq.maxValue;
		minMaxSet = YES;
	}
	[self updateMinMaxLabels];
}

- (void) graphLongTouch:(UILongPressGestureRecognizer *)recog
{
	if (recog.state == UIGestureRecognizerStateBegan)
	{
		NSLog (@"Show loupe");
		//		graphScrollView.userInteractionEnabled = NO;
	}
	else if (recog.state == UIGestureRecognizerStateEnded)
	{
		NSLog (@"Hide loupe");
		//		graphScrollView.userInteractionEnabled = YES;		
	}
	else if (recog.state == UIGestureRecognizerStateChanged)
	{
		NSLog (@"Move loupe");
	}
}

- (void) refreshGraph
{
	[graphRequestCache removeAllObjects];
	[self.layer setNeedsDisplayInRect:self.layer.bounds];
	graphStartDate = [[NSDate date] retain];	
}

- (void) setMetrics:(NSArray *)value
{
	[metrics release];
	metrics = [value retain];
		
	minMaxSet = NO;
	maxValue = 0.0;
	minValue = 0.0;
	
	[self refreshGraph];
}

@end
