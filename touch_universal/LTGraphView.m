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
		tiledLayer.tileSize = CGSizeMake(512.0, 512.0);
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

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

//- (void) setFrame:(CGRect)rect
//{
//	NSLog (@"Askd to size to %@", NSStringFromCGRect(rect));
//	UIScrollView *scrollView = (UIScrollView *) [self superview];
//	rect.origin.y = 0.0;
//	rect.size.height = scrollView.contentSize.height;
//	NSLog (@"Resizing to %@", NSStringFromCGRect(rect));
//	[super setFrame:rect];
//}

- (void)dealloc 
{
	for (LTMetricGraphRequest *graphReq in [graphRequestCache allValues])
	{
		[[NSNotificationCenter defaultCenter] removeObserver:self name:@"GraphRefreshFinished" object:graphReq];
	}
	[graphRequestCache release];
	[metrics release];
	[minLabels release];
	[avgLabels release];
	[maxLabels release];
    [super dealloc];
}

- (void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx
{
	/* Determine draw size */
	CGRect clipRect = CGContextGetClipBoundingBox(ctx);
	NSLog (@"layer %@ asked to draw in %@", layer, NSStringFromCGRect(clipRect));
	
	/* Dimensions */
	CGFloat zoomScale = 1.0;
	CGSize contentSize;
	if ([[self.superview class] isSubclassOfClass:[UIScrollView class]])
	{
		UIScrollView *graphScrollView = (UIScrollView *) self.superview;
		contentSize = graphScrollView.contentSize;
		zoomScale = graphScrollView.zoomScale;
	}
	else
	{
		contentSize = self.superview.bounds.size;
	}

	
	/* Time orientation */
	NSDate *now = [NSDate date];
	CGFloat offset = contentSize.width - CGRectGetMaxX(clipRect);
	CGFloat secondsPerPixel = (zoomScale * 86400) / CGRectGetWidth(self.superview.frame);
	
	/* Check for cached request */
	if (!graphRequestCache) graphRequestCache = [[NSMutableDictionary dictionary] retain];
	LTMetricGraphRequest *graphReq = [graphRequestCache objectForKey:[NSNumber numberWithFloat:offset]];
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
				NSLog (@"layer height is %f for scale calc", layer.frame.size.height);
				yOffset = layer.frame.size.height * (graphReq.minValue / (maxValue - minValue));
				
				yScale = 1.0 / (maxValue / (graphReq.maxValue - graphReq.minValue));
			}
			
			CGFloat graphImageMargin = 10.0;	/* Top and bottom padding/margin for graph */
			
			CGPDFDocumentRef documentRef = CGPDFDocumentCreateWithProvider(provider);
			CFRelease(provider);
			CGPDFPageRef pageRef = CGPDFDocumentGetPage(documentRef, 1);
			CGRect imageRect = CGRectMake(CGRectGetMinX(clipRect), graphImageMargin, 
										  clipRect.size.width, contentSize.height - (2 * graphImageMargin));
			CGContextSetRGBFillColor(ctx, 0.0, 0.0, 0.0, 0.0);
			CGContextFillRect(ctx, CGContextGetClipBoundingBox(ctx));
			CGContextTranslateCTM(ctx, 0.0, (imageRect.size.height  - yOffset));
			CGContextScaleCTM(ctx, 1.0, -1.0 * yScale);
			CGContextConcatCTM(ctx, CGPDFPageGetDrawingTransform(pageRef, kCGPDFCropBox, imageRect, 0, false));
			NSLog (@"Drawing imageRect %@ in clip %@ with scale %f and yOffset %f", NSStringFromCGRect(imageRect), NSStringFromCGRect(clipRect), yScale, yOffset);
			
			CGContextDrawPDFPage(ctx, pageRef);
			
			CGContextRestoreGState(ctx);
		}
		
		
		if (invalidated) NSLog (@"\n\n========================================\n\n");
		else NSLog (@"----------------------------------------");		
	}
	else if (!graphReq)
	{
		/* Configure graph request */
		graphReq = [[LTMetricGraphRequest alloc] init];
		graphReq.delegate = self;
		[graphRequestCache setObject:graphReq forKey:[NSNumber numberWithFloat:offset]];
		graphReq.size = CGSizeMake(clipRect.size.width, self.superview.frame.size.height);
		graphReq.endSec = (int) [now timeIntervalSince1970] - (offset * secondsPerPixel);
		graphReq.startSec = graphReq.endSec - (clipRect.size.width * secondsPerPixel);
		graphReq.rectToInvalidate = clipRect;
		[graphReq.metrics addObjectsFromArray:self.metrics];
		if (!graphReq.customer && graphReq.metrics.count > 0)
		{
			graphReq.customer = [[graphReq.metrics objectAtIndex:0] customer];
		}
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(graphLoadFinished:)
													 name:@"GraphRefreshFinished"
												   object:graphReq];
		
		/* Perform refresh */
		NSLog (@"STarting load");
		[graphReq refresh];
	}
	
	/* Common date/time variables */
	NSDate *sliceEndDate = [NSDate dateWithTimeIntervalSince1970:([now timeIntervalSince1970] - (offset * secondsPerPixel))];
	NSDateComponents *endDateComponents = [[NSCalendar currentCalendar] components:NSMinuteCalendarUnit|NSHourCalendarUnit fromDate:sliceEndDate];
	
	/* Draw time line */
	int hour = [endDateComponents hour];
	NSString *hourString = [NSString stringWithFormat:@"%.2i:00", hour];
	CGSize hourStringSize = [hourString sizeWithFont:[UIFont fontWithName:@"Helvetica-Bold" size:10.0] constrainedToSize:CGSizeMake(100.0, 12.0) lineBreakMode:UILineBreakModeClip];
	CGRect hourRect = CGRectMake(CGRectGetMaxX(clipRect) - (([endDateComponents minute] * 60.0) / secondsPerPixel), 
								 CGRectGetHeight(self.superview.frame) - 10.0, 
								 hourStringSize.width, hourStringSize.height);
	CGContextSelectFont (ctx, "Helvetica-Bold", 10.0, kCGEncodingMacRoman);
	CGContextSetTextDrawingMode (ctx, kCGTextFill);
	CGContextSetTextMatrix(ctx, CGAffineTransformMake(1.0,0.0, 0.0, -1.0, 0.0, 0.0));
	while (CGRectGetMaxX(hourRect) > CGRectGetMinX(clipRect))
	{
		/* Draw current hour */
		NSLog (@"Drawing %@ at %@ for clip %@", hourString, NSStringFromCGRect(hourRect), NSStringFromCGRect(clipRect));
		CGContextSetRGBFillColor (ctx, 0, 0, 0, .5);
		CGContextShowTextAtPoint (ctx, hourRect.origin.x, hourRect.origin.y, [hourString cStringUsingEncoding:NSUTF8StringEncoding], [hourString length]);
		CGContextSetRGBFillColor (ctx, 1, 1, 1, .5);
		CGContextShowTextAtPoint (ctx, hourRect.origin.x, hourRect.origin.y+1, [hourString cStringUsingEncoding:NSUTF8StringEncoding], [hourString length]);
		
		/* Move (back) to prev hour */
		if (hour == 0) hour = 23;
		else hour -= 1;
		hourString = [NSString stringWithFormat:@"%.2i:00", hour];
		hourStringSize = [hourString sizeWithFont:[UIFont fontWithName:@"Helvetica-Bold" size:10.0] constrainedToSize:CGSizeMake(100.0, 12.0) lineBreakMode:UILineBreakModeClip];
		hourRect = CGRectMake(hourRect.origin.x - ((60 * 60) / secondsPerPixel), hourRect.origin.y, 
							  hourStringSize.width, hourStringSize.height);
	}

	/* Draw date line */
	NSDate *dateToDraw = sliceEndDate;
	NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
	[formatter setFormatterBehavior:NSDateFormatterBehavior10_4];
	[formatter setDateStyle:NSDateFormatterMediumStyle];
	[formatter setTimeStyle:NSDateFormatterNoStyle];
	NSString *dateString = [formatter stringForObjectValue:dateToDraw];
	CGSize dateStringSize = [dateString sizeWithFont:[UIFont fontWithName:@"Helvetica-Bold" size:14.0] constrainedToSize:CGSizeMake(200.0, 12.0) lineBreakMode:UILineBreakModeClip];
	CGRect dateRect = CGRectMake(CGRectGetMaxX(clipRect) - ((([endDateComponents minute] * 60.0) + ([endDateComponents hour] * 60.0 * 60.0)) / secondsPerPixel), 
								 CGRectGetHeight(self.superview.frame) - 40.0, 
								 dateStringSize.width, hourStringSize.height);
	CGContextSelectFont (ctx, "Helvetica-Bold", 14.0, kCGEncodingMacRoman);
	CGContextSetTextDrawingMode (ctx, kCGTextFill);
	CGContextSetTextMatrix(ctx, CGAffineTransformMake(1.0,0.0, 0.0, -1.0, 0.0, 0.0));
	while (CGRectGetMaxX(dateRect) > CGRectGetMinX(clipRect))
	{
		/* Draw current date */
		NSLog (@"Drawing %@ at %@ for clip %@", dateString, NSStringFromCGRect(dateRect), NSStringFromCGRect(clipRect));
		CGContextSetRGBFillColor (ctx, 0, 0, 0, .5);
		CGContextShowTextAtPoint (ctx, dateRect.origin.x, dateRect.origin.y, [dateString cStringUsingEncoding:NSUTF8StringEncoding], [dateString length]);
		CGContextSetRGBFillColor (ctx, 1, 1, 1, .5);
		CGContextShowTextAtPoint (ctx, dateRect.origin.x, dateRect.origin.y+1, [dateString cStringUsingEncoding:NSUTF8StringEncoding], [dateString length]);
		
		/* Move (back) to prev date */
		dateToDraw = [dateToDraw dateByAddingTimeInterval:-86400.0];
		dateString = [formatter stringForObjectValue:dateToDraw];
		dateStringSize = [dateString sizeWithFont:[UIFont fontWithName:@"Helvetica-Bold" size:14.0] constrainedToSize:CGSizeMake(120.0, 12.0) lineBreakMode:UILineBreakModeClip];
		dateRect = CGRectMake(dateRect.origin.x - (86400.0 / secondsPerPixel), dateRect.origin.y, 
							  dateStringSize.width, dateStringSize.height);
	}
	
}

- (void) updateMinMaxLabels
{
	for (UILabel *label in minLabels)
	{
		label.text = [NSString stringWithFormat:@"%.2f", minValue];
	}
	for (UILabel *label in maxLabels)
	{
		label.text = [NSString stringWithFormat:@"%.2f", maxValue];
	}
	CGFloat avgValue = minValue + ((maxValue - minValue)*0.5);
	for (UILabel *label in avgLabels)
	{
		label.text = [NSString stringWithFormat:@"%.2f", avgValue];
	}
}

- (void) apiCallDidFinish:(LTMetricGraphRequest *)graphReq
{
	NSLog (@"Got graph (%i bytes) -- %@", [graphReq.imageData length], [[NSString alloc] initWithData:graphReq.imageData encoding:NSUTF8StringEncoding]);
	[self.layer setNeedsDisplayInRect:graphReq.rectToInvalidate];
	
	/* Determine scale based on min/max */
	invalidated = NO;
	if (minMaxSet)
	{
		if (graphReq.minValue < minValue)
		{
			/* New min value... invalidate layer */
			NSLog (@"Got a new MIN");
			[self.layer setNeedsDisplayInRect:self.layer.bounds];
			invalidated = YES;
			minValue = graphReq.minValue;
		}
		if (graphReq.maxValue > maxValue)
		{
			/* New max value.. invalidate layer */
			NSLog (@"Got a new MAX");
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

- (void) setMetrics:(NSArray *)value
{
	[metrics release];
	metrics = [value retain];
	
	NSLog (@"Metrics is now %@", metrics);
	
	minMaxSet = NO;
	maxValue = 0.0;
	minValue = 0.0;
	
	[graphRequestCache removeAllObjects];
	NSLog (@"%@ Invalidating %@ in %@", self, self.layer, NSStringFromCGRect(self.layer.bounds));
	[self.layer setNeedsDisplayInRect:self.layer.bounds];
}

@end
