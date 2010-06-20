//
//  CALayer-PDF.m
//  Lithium Console
//
//  Created by James Wilson on 4/02/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "CALayer-PDF.h"


@implementation CALayer (PDF)

static NSGraphicsContext *pushedContext = nil;

void CPPushCGContext(CGContextRef newContext)
{
    pushedContext = [NSGraphicsContext currentContext];
    [NSGraphicsContext setCurrentContext:[NSGraphicsContext graphicsContextWithGraphicsPort:newContext flipped:NO]];
}

void CPPopCGContext(void)
{
    [NSGraphicsContext setCurrentContext:pushedContext];
    pushedContext = nil;
}

-(void)recursivelyRenderInContext:(CGContextRef)context
{
	[self.delegate drawLayer:self inContext:context];
	for (CALayer *currentSublayer in self.sublayers) {
		CGContextSaveGState(context);
		
		// Shift origin of context to match starting coordinate of sublayer
		CGPoint currentSublayerFrameOrigin = currentSublayer.frame.origin;
		CGPoint currentSublayerBoundsOrigin = currentSublayer.bounds.origin;
		CGContextTranslateCTM(context, currentSublayerFrameOrigin.x - currentSublayerBoundsOrigin.x, currentSublayerFrameOrigin.y - currentSublayerBoundsOrigin.y);
		if (self.masksToBounds) {
			CGContextClipToRect(context, currentSublayer.bounds);
		}
		if ([currentSublayer isKindOfClass:[CALayer class]]) {
			[currentSublayer recursivelyRenderInContext:context];
		} else {
			[currentSublayer drawInContext:context];
		}
		CGContextRestoreGState(context);
	}
}

-(NSData *)dataForPDFRepresentationOfLayer
{
	NSMutableData *pdfData = [[NSMutableData alloc] init];
	CGDataConsumerRef dataConsumer = CGDataConsumerCreateWithCFData((CFMutableDataRef)pdfData);
	
	const CGRect mediaBox = CGRectMake(0.0f, 0.0f, self.bounds.size.width, self.bounds.size.height);
	CGContextRef pdfContext = CGPDFContextCreate(dataConsumer, &mediaBox, NULL);

	CPPushCGContext(pdfContext);
	
	CGContextBeginPage(pdfContext, &mediaBox);
	[self recursivelyRenderInContext:pdfContext];
	CGContextEndPage(pdfContext);
	CGPDFContextClose(pdfContext);

	CPPopCGContext();
	
	CGContextRelease(pdfContext);
	CGDataConsumerRelease(dataConsumer);
	
	return [pdfData autorelease];
}

@end
