//
//  LTContainerIconViewGraphView.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 29/11/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTContainerIconViewGraphView.h"
#import "LTMetricGraphRequest.h"
#import "LTEntity.h"

@implementation LTContainerIconViewGraphView

@synthesize container=container_;

- (void)awakeFromNib
{
	self.opaque = NO;
	self.backgroundColor = [UIColor clearColor];
	
}

- (void)drawRect:(CGRect)rect 
{
	if (graphReq)
	{
		/* Draw image */
		UIGraphicsBeginImageContext(self.bounds.size);
		CGContextRef ctx = UIGraphicsGetCurrentContext();
		CGDataProviderRef provider = CGDataProviderCreateWithCFData((CFDataRef)[graphReq imageData]);
		if (provider)
		{
			CGPDFDocumentRef documentRef = CGPDFDocumentCreateWithProvider(provider);
			CFRelease(provider);
			CGPDFPageRef pageRef = CGPDFDocumentGetPage(documentRef, 1);
			CGRect imageRect = self.bounds;
			CGContextSetRGBFillColor(ctx, 0.0, 0.0, 0.0, 0.0);
			CGContextFillRect(ctx, CGContextGetClipBoundingBox(ctx));
			CGContextTranslateCTM(ctx, 0.0, imageRect.size.height);
			CGContextScaleCTM(ctx, 1.0, -1.0);
			CGContextConcatCTM(ctx, CGPDFPageGetDrawingTransform(pageRef, kCGPDFCropBox, imageRect, 0, false));
			CGContextSetBlendMode(ctx, kCGBlendModeDifference);
			
			CGContextDrawPDFPage(ctx, pageRef);
			UIImage *graphImage = UIGraphicsGetImageFromCurrentImageContext();
			UIGraphicsEndImageContext();
			[graphImage drawInRect:self.bounds blendMode:kCGBlendModeNormal alpha:1.0];
		}
	}
	else if (self.container)
	{
		/* Create graph request */
		NSDate *now = [NSDate date];
		graphReq = [[LTMetricGraphRequest alloc] init];
		graphReq.delegate = self;
		graphReq.size = CGSizeMake(400, 400);
		graphReq.endSec = (int) [now timeIntervalSince1970];
		graphReq.startSec = (int) [now timeIntervalSince1970] - 86400;
		graphReq.allWhiteLines = YES;
		[graphReq.metrics addObjectsFromArray:[self.container graphableMetrics]];
		if (!graphReq.customer && graphReq.metrics.count > 0)
		{
			graphReq.customer = [[graphReq.metrics objectAtIndex:0] customer];
		}
		
		/* Perform refresh */
		[graphReq refresh];		
	}
}

- (void)dealloc {
	[container_ release];
	[graphReq release];
    [super dealloc];
}

- (void) apiCallDidFinish:(LTMetricGraphRequest *)req
{
	[self setNeedsDisplay];
}

@end
