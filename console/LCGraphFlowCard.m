//
//  LCGraphFlowCard.m
//  Lithium Console
//
//  Created by James Wilson on 4/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCGraphFlowCard.h"
#import "LCObject.h"

#import "LCTextLayerNoHit.h"

@implementation LCGraphFlowCard

- (id) init
{
	self = [super init];
	if (!self) return nil;
	
	/* Create Graph Controller */
	graphController = [[[LCMetricGraphController new] retain] autorelease];
	graphController.undoEnabled = NO;
	[graphController addObserver:self 
					  forKeyPath:@"graphPDFRep" 
						 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
						 context:nil];
	
	/*
	 * Create layers 
	 */
	
	CGSize cardSize = CGSizeMake (160.0, 62.0);
	
	/* Shadow Gradient */
	CGRect r;
	r.origin = CGPointZero;
	r.size = CGSizeMake(cardSize.width, cardSize.height);
	size_t bytesPerRow = (size_t) 4 * (size_t) r.size.width;
	void* bitmapData = calloc(bytesPerRow, r.size.height);
	CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	CGContextRef context = CGBitmapContextCreate(bitmapData, r.size.width,
												 r.size.height, 8,  bytesPerRow, 
												 colorSpace, kCGImageAlphaPremultipliedFirst);
	CGColorSpaceRelease(colorSpace);
	NSGradient *gradient = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithDeviceWhite:0 alpha:0.8] endingColor:[NSColor colorWithDeviceWhite:0 alpha:0.0]];
	NSGraphicsContext *nsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:YES];
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:nsContext];
	[gradient drawInRect:NSMakeRect(0, 0, r.size.width, (r.size.height * 0.4)) angle:90];
	[NSGraphicsContext restoreGraphicsState];
	CGImageRef shadowImage = CGBitmapContextCreateImage(context);
	CGContextRelease(context);
	free(bitmapData);
	[gradient release];

	/* Create Root/Image Layers */
	cardLayer = [[CALayer layer] retain];
	cardLayer.name = @"card";
	imageLayer = [CALayer layer];
	imageLayer.name = @"image";
	imageLayer.delegate = self;
	[imageLayer setNeedsDisplay];
	
	/* Set default appearance for card layer */
	r.origin = CGPointZero;
	r.size = cardSize;
	self.imageLayer.frame = r;
	CGColorRef color = CGColorCreateGenericGray(0.15, 1.0);
	self.imageLayer.backgroundColor = color;
	CGColorRelease(color);
	self.imageLayer.opacity = 0.7;
	self.cardLayer.bounds = r;
	color = CGColorCreateGenericRGB(0.0, 0.0, 0.0, 0.0);
	self.cardLayer.backgroundColor = color;
	CGColorRelease(color);
	[self.cardLayer addSublayer:imageLayer];
	
	// object name text
	objLabelLayer = [LCTextLayerNoHit layer];
	objLabelLayer.name = @"objlabel";
	objLabelLayer.font = [NSFont systemFontOfSize:8.5];
	objLabelLayer.alignmentMode = kCAAlignmentCenter;
	objLabelLayer.fontSize = 8.5;
	objLabelLayer.cornerRadius = 4;
	objLabelLayer.wrapped = NO;
	objLabelLayer.truncationMode = kCATruncationStart;
	color = CGColorCreateGenericGray(1.0, 0.7);
	objLabelLayer.foregroundColor = color;
	CGColorRelease(color);
	objLabelLayer.frame = CGRectMake(2.0, imageLayer.bounds.size.height * 0.5 + (objLabelLayer.fontSize * 0.20), 
									 CGRectGetWidth(imageLayer.bounds) - 4.0, 10.0);
	
	[self.imageLayer addSublayer:objLabelLayer];		
	
	// metric name text
	metLabelLayer = [LCTextLayerNoHit layer];
	metLabelLayer.name = @"metlabel";
	metLabelLayer.font = [NSFont systemFontOfSize:8.5];
	metLabelLayer.alignmentMode = kCAAlignmentCenter;
	metLabelLayer.fontSize = 8.5;
	metLabelLayer.cornerRadius = 4;
	metLabelLayer.wrapped = NO;
	metLabelLayer.truncationMode = kCATruncationStart;
	color = CGColorCreateGenericGray(1.0, 0.7);
	metLabelLayer.foregroundColor = color;
	CGColorRelease(color);
	metLabelLayer.frame = CGRectMake(2.0, imageLayer.bounds.size.height * 0.5 - (metLabelLayer.fontSize * 1.2), 
									 CGRectGetWidth(imageLayer.bounds) - 4.0, 10.0);
	[self.imageLayer addSublayer:metLabelLayer];		
	
	/* Create reflection layer */
	reflectionLayer = [CALayer layer];
	r.origin = CGPointMake(0.0, -1.0 * r.size.height);
	reflectionLayer.frame = r;
	reflectionLayer.name = @"reflection";
	CATransform3D transform = CATransform3DMakeScale(1,-1,1);
	reflectionLayer.transform = transform;
	color = CGColorCreateGenericGray(0.0, 0.0);
	reflectionLayer.backgroundColor = color;
	CGColorRelease(color);
	reflectionLayer.delegate = self;

	/* Create reflection gradient layer */
	gradientLayer = [CALayer layer];
	gradientLayer.name = @"reflection_gradient";
	r.origin.y = 0.0;
	r.origin.x = 0.0;
	gradientLayer.frame = r;
	gradientLayer.contents = (id) shadowImage;
	gradientLayer.opaque = NO;
	[reflectionLayer setMask:self.gradientLayer];

	[imageLayer addSublayer:self.reflectionLayer];
	[reflectionLayer setNeedsDisplay];
	
	
	/* Set frame for object label layer */
	
	/* Set frame for metric label layer */
		
		
	return self;
}

- (void) dealloc
{
	[graphController removeObserver:self forKeyPath:@"graphPDFRep"];
	[graphController release];
	[cardLayer release];
	[super dealloc];
}

static void addRoundedRectToPath(CGContextRef context, CGRect rect,
								 float ovalWidth,float ovalHeight)

{
    float fw, fh;
	
    if (ovalWidth == 0 || ovalHeight == 0) {// 1
        CGContextAddRect(context, rect);
        return;
    }
	
    CGContextSaveGState(context);// 2
	
    CGContextTranslateCTM (context, CGRectGetMinX(rect),// 3
						   CGRectGetMinY(rect));
    CGContextScaleCTM (context, ovalWidth, ovalHeight);// 4
    fw = CGRectGetWidth (rect) / ovalWidth;// 5
    fh = CGRectGetHeight (rect) / ovalHeight;// 6
	
    CGContextMoveToPoint(context, fw, fh/2); // 7
    CGContextAddArcToPoint(context, fw, fh, fw/2, fh, 1);// 8
    CGContextAddArcToPoint(context, 0, fh, 0, fh/2, 1);// 9
    CGContextAddArcToPoint(context, 0, 0, fw/2, 0, 1);// 10
    CGContextAddArcToPoint(context, fw, 0, fw, fh/2, 1); // 11
    CGContextClosePath(context);// 12
	
    CGContextRestoreGState(context);// 13
}

- (void)drawLayer:(CALayer *)theLayer
        inContext:(CGContextRef)theContext
{
	/* Geometry */
	CGFloat graphHeight = [self.imageLayer bounds].size.height;
	CGFloat graphWidth = [self.imageLayer bounds].size.width;
	CGRect pdfRect = CGRectMake(0.0, 0.0, graphWidth, graphHeight);
	
	/* Draw gradient wash */
	if ([[theLayer name] isEqualToString:@"image"])
	{
		CGRect r;
		r.origin = CGPointZero;
		r.size = CGSizeMake([self.imageLayer bounds].size.width, [self.imageLayer bounds].size.height);
		size_t bytesPerRow = (size_t) 4 * (size_t) r.size.width;
		void* bitmapData = calloc(bytesPerRow, r.size.height);
		CGContextRef context = CGBitmapContextCreate(bitmapData, r.size.width,
													 r.size.height, 8,  bytesPerRow, 
													 CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB), kCGImageAlphaPremultipliedFirst);
		float gradientLight = 0.07;
		float gradientDark = 0.1;
		
		NSColor *startColor = [NSColor colorWithCalibratedWhite:gradientLight alpha:1.0];
		NSColor *endColor = [NSColor colorWithCalibratedWhite:gradientDark alpha:1.0];
		NSGradient *gradient = [[NSGradient alloc] initWithStartingColor:endColor endingColor:startColor]; 
		NSGraphicsContext *nsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:theContext flipped:YES];
		[NSGraphicsContext saveGraphicsState];
		[NSGraphicsContext setCurrentContext:nsContext];
		if (self.orientation == STACKED_LEFT)
		{ [gradient drawInRect:NSMakeRect(0, 0, r.size.width, r.size.height) angle:0]; }
		else if (self.orientation == STACKED_RIGHT)
		{ [gradient drawInRect:NSMakeRect(0, 0, r.size.width, r.size.height) angle:180]; }
		else if (self.orientation == FLAT)
		{ [gradient drawInRect:NSMakeRect(0, 0, r.size.width, r.size.height) angle:90]; }
		[NSGraphicsContext restoreGraphicsState];
		CGContextRelease(context);
		free(bitmapData);
		[gradient release];		
	}	
	
		
	/* Draw */
	LCObject *object = (LCObject *) [[[[graphController metricItems] objectAtIndex:0] metric] parent];
	if (object.opState > 0)
	{
		/* Create image context */
		CGRect r = pdfRect;
		size_t bytesPerRow = (size_t) 4 * (size_t) r.size.width;
		void* bitmapData = calloc(bytesPerRow, r.size.height);
		CGContextRef context = CGBitmapContextCreate(bitmapData, r.size.width,
													 r.size.height, 8,  bytesPerRow, 
													 CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB), kCGImageAlphaPremultipliedFirst);
		
		/* Draw Status Wash */
		NSColor *startColor;
		NSColor *endColor;
		if (object.opState == 1 || object.opState == 2) 
		{
			startColor = [NSColor colorWithCalibratedRed:1.0 green:0.5 blue:0.0 alpha:0.2];
			endColor = [NSColor colorWithCalibratedRed:1.0 green:0.5 blue:0.0 alpha:0.1];
		}
		if (object.opState == 3) 
		{
			startColor = [NSColor colorWithCalibratedRed:1.0 green:0.0 blue:0.0 alpha:0.2];
			endColor = [NSColor colorWithCalibratedRed:1.0 green:0.0 blue:0.0 alpha:0.1];
		}
		NSGradient *gradient = [[NSGradient alloc] initWithStartingColor:endColor endingColor:startColor];
		NSGraphicsContext *nsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:YES];
		[NSGraphicsContext saveGraphicsState];
		[NSGraphicsContext setCurrentContext:nsContext];
		[gradient drawInRect:NSMakeRect(0, 0, r.size.width, r.size.height) angle:90];
		[NSGraphicsContext restoreGraphicsState];
		CGContextRelease(context);
		free(bitmapData);
		[gradient release];					
	}
		
	/* Draw PDF */
	NSData *pdfData = [graphController.graphPDFRep PDFRepresentation];
	if (pdfData)
	{
		CGDataProviderRef provider = CGDataProviderCreateWithCFData ((CFDataRef) pdfData);
		CGPDFDocumentRef document = CGPDFDocumentCreateWithProvider (provider);
		CGPDFPageRef page = CGPDFDocumentGetPage (document, 1);// 2
		CGAffineTransform m;	
		m = CGPDFPageGetDrawingTransform (page, kCGPDFMediaBox, pdfRect, 0, 0);
		CGContextSaveGState (theContext);// 2
		CGContextConcatCTM (theContext, m);// 3
		CGContextClipToRect (theContext,CGPDFPageGetBoxRect (page, kCGPDFMediaBox));// 4
		CGContextDrawPDFPage (theContext, page);// 5
		CGContextRestoreGState (theContext);
		CGPDFDocumentRelease (document);
		CGDataProviderRelease(provider);
	}
	
	/* Draw Outline */
	if ([[theLayer name] isEqualToString:@"image"] || [[theLayer name] isEqualToString:@"reflection"])
	{
		CGMutablePathRef thePath = CGPathCreateMutable();
		
		addRoundedRectToPath (theContext, CGRectMake(pdfRect.origin.x+0.5, pdfRect.origin.y+0.5, pdfRect.size.width-1.0, pdfRect.size.height-1.0), 4.0, 4.0);
		
		if ([[theLayer name] isEqualToString:@"image"])
		{ 
			CGColorRef color = CGColorCreateGenericGray(69.0/256.0, 1.0);
			CGContextSetStrokeColorWithColor(theContext, color); 
			CGColorRelease(color);
		}
		else if ([[theLayer name] isEqualToString:@"reflection"])
		{ 
			CGColorRef color = CGColorCreateGenericGray(69.0/256.0, 1.0);
			CGContextSetStrokeColorWithColor(theContext, color); 
			CGColorRelease(color);
		}
		CGContextSetLineWidth(theContext, 1.0);
		CGContextStrokePath(theContext);
//		CGContextClip(theContext);
		
		CFRelease(thePath);
	}	
}

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	[self.imageLayer setNeedsDisplay];
	[self.reflectionLayer setNeedsDisplay];
}

@synthesize cardLayer;
@synthesize imageLayer;
@synthesize reflectionLayer;
@synthesize gradientLayer;
@synthesize objLabelLayer;
@synthesize metLabelLayer;

@synthesize metric;
- (void) setMetric:(LCMetric *)value
{
	[metric release];
	metric = [value retain];
	[cardLayer setValue:self.metric forKey:@"metric"];
	[imageLayer setValue:self.metric forKey:@"metric"];
	if ([[[metric object] name] isEqualToString:@"master"])
	{ objLabelLayer.string = [[metric container] desc]; }
	else
	{ objLabelLayer.string = [[metric object] desc]; }
	metLabelLayer.string = metric.desc;
	[graphController removeAllMetricItems];
	[graphController addMetric:metric];
}

@synthesize flowController;
@synthesize graphController;
@synthesize orientation;

@end
