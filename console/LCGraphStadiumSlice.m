//
//  LCGraphStadiumSlice.m
//  Lithium Console
//
//  Created by James Wilson on 29/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCGraphStadiumSlice.h"
#import "LCObject.h"


@implementation LCGraphStadiumSlice

- (id) init
{
	self = [super init];
	
	if (self)
	{
		graphControllers = [[NSMutableArray array] retain];
		labelLayers = [[NSMutableArray array] retain];
	}
	
	return self;
}

- (void) dealloc
{
	for (LCMetricGraphController *controller in graphControllers)
	{ [controller removeObserver:self forKeyPath:@"graphPDFRep"]; }
	[graphControllers release];
	[labelLayers release];
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
	int i;
	
	/* Check to make sure we have a real size */
	if ([theLayer bounds].size.height == 0.0 || [theLayer bounds].size.width == 0.0)
	{ 
		/* Size is 0.0x0.0, do not go further */
		return; 
	}
	
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
		float gradientDelta = gradientLight - gradientDark;
		float gradientSteps = gradientDelta / ((float) total * (float) 0.5);

		NSGradient *gradient;
		if (self.index < 5) 
		{
			float gradientIndex = (float) self.index;
			float gradientLocalStart = gradientLight - (gradientIndex * gradientSteps);			
			NSColor *startColor = [NSColor colorWithCalibratedWhite:gradientLocalStart alpha:1.0];
			NSColor *endColor = [NSColor colorWithCalibratedWhite:gradientLocalStart - gradientSteps alpha:1.0];
			if (self.index < 5) gradient = [[NSGradient alloc] initWithStartingColor:endColor endingColor:startColor];
		}
		
		if (self.index >= 5) 
		{
			float gradientIndex = (float) self.index - 5; 
			float gradientLocalStart = gradientDark + (gradientIndex * gradientSteps);			
			NSColor *startColor = [NSColor colorWithCalibratedWhite:gradientLocalStart alpha:1.0];
			NSColor *endColor = [NSColor colorWithCalibratedWhite:gradientLocalStart + gradientSteps alpha:1.0];			
			gradient = [[NSGradient alloc] initWithStartingColor:endColor endingColor:startColor]; 
		}
		NSGraphicsContext *nsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:YES];
		[NSGraphicsContext saveGraphicsState];
		[NSGraphicsContext setCurrentContext:nsContext];
		[gradient drawInRect:NSMakeRect(0, 0, r.size.width, r.size.height) angle:180];
		[NSGraphicsContext restoreGraphicsState];
		CGImageRef gradientBitmap = CGBitmapContextCreateImage(context);
		CGContextRelease(context);
		free(bitmapData);
		[gradient release];
		
		CGContextDrawImage(theContext, r, gradientBitmap);
		
		CGImageRelease (gradientBitmap);
	}	
	
	/* Geometry */
	CGFloat graphHeight = [self.imageLayer bounds].size.height / (float) self.rows;
	CGFloat graphWidth = [self.imageLayer bounds].size.width;	
	
	/* Draw Individual Screens */
	for (i=0; i < self.rows; i++)
	{
		CGRect pdfRect = CGRectMake(0.0, graphHeight * (self.rows - i - 1), graphWidth, graphHeight);
		
		/* Check to make sure we have a graph for this screen */
		if (i < [graphControllers count])
		{
			LCMetricGraphController *controller = [graphControllers objectAtIndex:i];
			
			/* Draw Status Wash */
			LCObject *object = (LCObject *) [[[[controller metricItems] objectAtIndex:0] metric] parent];
			if (object.opState > 0)
			{
				CGRect r = pdfRect;
				if (r.size.width == 0.0 || r.size.height == 0.0) 
				{ 
					continue;
				}
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
				NSGraphicsContext *nsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:theContext flipped:YES];
				[NSGraphicsContext saveGraphicsState];
				[NSGraphicsContext setCurrentContext:nsContext];
				[gradient drawInRect:NSMakeRect(r.origin.x, r.origin.y, r.size.width, r.size.height) angle:90];
				[NSGraphicsContext restoreGraphicsState];
				[gradient release];
			}	
			
			/* Draw PDF */
			if (controller.graphPDFRep)
			{			
				NSGraphicsContext *nsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:theContext flipped:NO];
				[NSGraphicsContext saveGraphicsState];
				[NSGraphicsContext setCurrentContext:nsContext];
				[controller.graphPDFRep drawInRect:NSRectFromCGRect(pdfRect)];
				[NSGraphicsContext restoreGraphicsState];
			}
		}
		
		/* Draw Outline */
		if ([[theLayer name] isEqualToString:@"image"] || [[theLayer name] isEqualToString:@"reflection"])
		{
			CGMutablePathRef thePath = CGPathCreateMutable();

			addRoundedRectToPath (theContext, pdfRect, 4.0, 4.0);

			CGColorRef color = CGColorCreateGenericGray(0.2, 1.0);
			if ([[theLayer name] isEqualToString:@"image"])
			{ CGContextSetStrokeColorWithColor(theContext, color); }
			else if ([[theLayer name] isEqualToString:@"reflection"])
			{ CGContextSetStrokeColorWithColor(theContext, color); }
			CGColorRelease(color);
			CGContextSetLineWidth(theContext, 1.0);
			CGContextStrokePath(theContext);
			CFRelease(thePath);
		}
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

@synthesize graphControllers;
- (void) insertObject:(LCMetricGraphController *)controller inGraphControllersAtIndex:(unsigned int)i
{
	[graphControllers insertObject:controller atIndex:i];
	[controller addObserver:self forKeyPath:@"graphPDFRep" options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) context:nil];
}
- (void) removeObjectFromGraphControllersAtIndex:(unsigned int)i
{
	LCMetricGraphController *controller = [graphControllers objectAtIndex:i];
	[controller removeObserver:self forKeyPath:@"graphPDFRep"];
	[graphControllers removeObjectAtIndex:i];
}

@synthesize stadiumController;

@synthesize sliceLayer;
@synthesize imageLayer;
@synthesize reflectionLayer;
@synthesize gradientLayer;
@synthesize labelLayers;

@synthesize index;
@synthesize total;
@synthesize rows;

@end
