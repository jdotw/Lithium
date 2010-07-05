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
		tiledLayer.levelsOfDetail = 1;
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
	
	/* Time orientation */
	UIScrollView *graphScrollView = (UIScrollView *) self.superview;
	NSDate *now = [NSDate date];
	CGFloat offset = graphScrollView.contentSize.width - CGRectGetMaxX(clipRect);
	
	/* Check for cached request */
	if (!graphRequestCache) graphRequestCache = [[NSMutableDictionary dictionary] retain];
	LTMetricGraphRequest *graphReq = [graphRequestCache objectForKey:[NSNumber numberWithFloat:offset]];
	if (graphReq && !graphReq.refreshInProgress)
	{
		/* Draw image */
		CGDataProviderRef provider = CGDataProviderCreateWithCFData((CFDataRef)[graphReq imageData]);
		if (provider)
		{
			CGFloat yScale = 1.0;
			CGFloat yOffset = 0.0;
			if (minMaxSet)
			{
				NSLog (@"layer height is %f for scale calc", layer.frame.size.height);
				yOffset = layer.frame.size.height * (graphReq.minValue / (maxValue - minValue));
				
				yScale = 1.0 / (maxValue / (graphReq.maxValue - graphReq.minValue));
			}
			
			CGPDFDocumentRef documentRef = CGPDFDocumentCreateWithProvider(provider);
			CFRelease(provider);
			CGPDFPageRef pageRef = CGPDFDocumentGetPage(documentRef, 1);
			CGRect imageRect = CGRectMake(CGRectGetMinX(clipRect), 0.0, 
										  clipRect.size.width, graphScrollView.contentSize.height);
			CGContextSetRGBFillColor(ctx, 0.0, 0.0, 0.0, 0.0);
			CGContextFillRect(ctx, CGContextGetClipBoundingBox(ctx));
			CGContextTranslateCTM(ctx, 0.0, layer.bounds.size.height - yOffset);
			CGContextScaleCTM(ctx, 1.0, -1.0 * yScale);
			CGContextConcatCTM(ctx, CGPDFPageGetDrawingTransform(pageRef, kCGPDFCropBox, imageRect, 0, false));
			NSLog (@"Drawing imageRect %@ in clip %@ with scale %f and yOffset %f", NSStringFromCGRect(imageRect), NSStringFromCGRect(clipRect), yScale, yOffset);
			
			CGContextDrawPDFPage(ctx, pageRef);
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
		CGFloat secondsPerPixel = (1.0 * 86400) / CGRectGetWidth(graphScrollView.frame);
		graphReq.size = CGSizeMake(clipRect.size.width, graphScrollView.frame.size.height);
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
