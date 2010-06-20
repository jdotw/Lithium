//
//  LCGraphStadiumController.m
//  Lithium Console
//
//  Created by James Wilson on 27/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCGraphStadiumController.h"

#import "LCMetric.h"
#import "LCObject.h"
#import "LCDevice.h"

#import "LCMetricGraphController.h"
#import "LCMetricGraphView.h"
#import "LCGraphStadiumLayout.h"
#import "LCGraphStadiumSlice.h"
#import "LCGraphStadiumSliceLayer.h"
#import "LCTextLayerNoHit.h"
#import "LCLayerNoHit.h"

@implementation LCGraphStadiumController

#pragma mark "Constructors"

- (void) awakeFromNib
{
	CGColorSpaceRef colorSpace;
	CGColorRef color;
	
	/* Create variables */
	graphableObjects = [[NSMutableDictionary dictionary] retain];
	graphControllers = [[NSMutableArray array] retain];
	slices = [[NSMutableArray array] retain];
	
	/* Shadow Gradient */
	CGRect r = CGRectMake(0.0, 0.0, 100.0, 100.0);
	size_t bytesPerRow = (size_t) 4 * (size_t) r.size.width;
	void* bitmapData = calloc (bytesPerRow, r.size.height);
	colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	CGContextRef context = CGBitmapContextCreate(bitmapData, r.size.width,
												 r.size.height, 8,  bytesPerRow, 
												 colorSpace, kCGImageAlphaPremultipliedFirst);
	CGColorSpaceRelease(colorSpace);
	NSGradient *gradient = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithDeviceWhite:0.15 alpha:1.0] endingColor:[NSColor colorWithDeviceWhite:0.03 alpha:1.0]];
	NSGraphicsContext *nsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:YES];
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:nsContext];
	[gradient drawInRect:NSMakeRect(0, 0, r.size.width, r.size.height - (r.size.height * 0.3)) angle:90];
	[NSGraphicsContext restoreGraphicsState];
	shadowImage = CGBitmapContextCreateImage(context);
	CGContextRelease(context);
	free(bitmapData);
	[gradient release];	
	
	/* Ceate Flow Controller */
	flowController = [LCGraphFlowController new];
	flowController.scroller = flowScroller;
	
	/* Create root layer */
	rootLayer = [CALayer layer];
	rootLayer.layoutManager = [CAConstraintLayoutManager layoutManager];
	color = CGColorCreateGenericGray(0.03, 1.0);
	rootLayer.backgroundColor = color;
	CGColorRelease(color);
	rootLayer.name = @"root";
	rootLayer.contents = (id) shadowImage;
	rootLayer.frame = NSRectToCGRect([stadiumView bounds]);
	
	textStyle = [NSDictionary dictionaryWithObjectsAndKeys:
				 [NSNumber numberWithInteger:12], @"cornerRadius",
				 [NSValue valueWithSize:NSMakeSize(5, 0)], @"margin",
				 @"Lucida Grande Bold", @"font",
				 [NSNumber numberWithInteger:12], @"fontSize",
				 kCAAlignmentCenter, @"alignmentMode",
				 nil];
		
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
	LCGraphStadiumLayout *stadiumLayout = [LCGraphStadiumLayout new];
	stadiumLayout.controller = self;
	bodyLayer.layoutManager = stadiumLayout;
	[stadiumLayout autorelease];
	[containerLayer addSublayer:bodyLayer];		
	
	/* Add the flow layer */
	flowController.rootLayer.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
	[flowController.rootLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMinX relativeTo:@"superlayer" attribute:kCAConstraintMinX]];
	[flowController.rootLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMaxX relativeTo:@"superlayer" attribute:kCAConstraintMaxX]];
	[flowController.rootLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMaxY relativeTo:@"superlayer" attribute:kCAConstraintMinY offset:100.0]];
	[flowController.rootLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMinY relativeTo:@"superlayer" attribute:kCAConstraintMinY]];
	[rootLayer addSublayer:flowController.rootLayer];
	
	/* create a pleasant gradient mask around our central layer.
	 We don't have to worry about re-creating these when the window
	 size changes because the images will be automatically interpolated
	 to their new sizes; and as gradients, they are very well suited to
	 interpolation. */
	CALayer *maskLayer = [LCLayerNoHit layer];
	maskLayer.name = @"mask";
	CALayer *leftGradientLayer = [LCLayerNoHit layer];
	leftGradientLayer.name = @"left_gradient";
	CALayer *rightGradientLayer = [LCLayerNoHit layer];
	rightGradientLayer.name = @"right_gradient";
	CALayer *bottomGradientLayer = [LCLayerNoHit layer];
	bottomGradientLayer.name = @"bottom_gradient";

	float gradientCurtain = 0.5;
	
	// left
	r.origin = CGPointZero;
	r.size.width = [stadiumView frame].size.width;
	r.size.height = [stadiumView frame].size.height;
	bytesPerRow = 4*r.size.width;
	bitmapData = calloc(bytesPerRow, r.size.height);
	colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	context = CGBitmapContextCreate(bitmapData, r.size.width,
									r.size.height, 8,  bytesPerRow, 
									colorSpace, kCGImageAlphaPremultipliedFirst);
	CGColorSpaceRelease(colorSpace);
	gradient = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithDeviceWhite:0. alpha:0.3] endingColor:[NSColor colorWithDeviceWhite:0. alpha:0]];
	nsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:YES];
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:nsContext];
	[gradient drawInRect:NSMakeRect(0, 0, r.size.width * gradientCurtain, r.size.height) angle:0];
	[NSGraphicsContext restoreGraphicsState];
	CGImageRef gradientImage = CGBitmapContextCreateImage(context);
	leftGradientLayer.contents = (id)gradientImage;
	CGContextRelease(context);
	CGImageRelease(gradientImage);
	free(bitmapData);
	
	// right
	bitmapData = calloc(bytesPerRow, r.size.height);
	colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	context = CGBitmapContextCreate(bitmapData, r.size.width,
									r.size.height, 8,  bytesPerRow, 
									colorSpace, kCGImageAlphaPremultipliedFirst);
	CGColorSpaceRelease(colorSpace);	
	nsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:YES];
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:nsContext];
	[gradient drawInRect:NSMakeRect(r.size.width * (1 - gradientCurtain), 0, r.size.width * gradientCurtain, r.size.height) angle:180];
	[NSGraphicsContext restoreGraphicsState];
	gradientImage = CGBitmapContextCreateImage(context);
	rightGradientLayer.contents = (id)gradientImage;
	CGContextRelease(context);
	CGImageRelease(gradientImage);
	free(bitmapData);
	
	gradientCurtain = 0.4;
	
	// bottom
	r.size.width = [stadiumView frame].size.width;
	r.size.height = [stadiumView frame].size.height * gradientCurtain;
	bytesPerRow = 4*r.size.width;
	bitmapData = calloc(bytesPerRow, r.size.height);
	colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	context = CGBitmapContextCreate(bitmapData, r.size.width,
									r.size.height, 8,  bytesPerRow, 
									colorSpace, kCGImageAlphaPremultipliedFirst);
	CGColorSpaceRelease(colorSpace);
	nsContext = [NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:YES];
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:nsContext];
	[gradient drawInRect:NSMakeRect(0, 0, r.size.width, r.size.height) angle:90];
	[NSGraphicsContext restoreGraphicsState];
	gradientImage = CGBitmapContextCreateImage(context);
	bottomGradientLayer.contents = (id)gradientImage;
	CGContextRelease(context);
	CGImageRelease(gradientImage);
	free(bitmapData);
	[gradient release];
	
	// the autoresizing mask allows it to change shape with the parent layer
	maskLayer.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
	maskLayer.layoutManager = [CAConstraintLayoutManager layoutManager];
	maskLayer.frame = NSRectToCGRect([stadiumView bounds]);
	[leftGradientLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMinX relativeTo:@"superlayer" attribute:kCAConstraintMinX]];
	[leftGradientLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMinY relativeTo:@"superlayer" attribute:kCAConstraintMinY]];
	[leftGradientLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMaxY relativeTo:@"superlayer" attribute:kCAConstraintMaxY]];
	[leftGradientLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMaxX relativeTo:@"superlayer" attribute:kCAConstraintMaxX scale:.5 offset:5.0]];
	[rightGradientLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMaxX relativeTo:@"superlayer" attribute:kCAConstraintMaxX]];
	[rightGradientLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMinY relativeTo:@"superlayer" attribute:kCAConstraintMinY]];
	[rightGradientLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMaxY relativeTo:@"superlayer" attribute:kCAConstraintMaxY]];
	[rightGradientLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMinX relativeTo:@"superlayer" attribute:kCAConstraintMaxX scale:.5 offset:5.0]];
	[bottomGradientLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMaxX relativeTo:@"superlayer" attribute:kCAConstraintMaxX]];
	[bottomGradientLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMinY relativeTo:@"superlayer" attribute:kCAConstraintMinY]];
	[bottomGradientLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMinX relativeTo:@"superlayer" attribute:kCAConstraintMinX]];
	[bottomGradientLayer addConstraint:[CAConstraint constraintWithAttribute:kCAConstraintMaxY relativeTo:@"superlayer" attribute:kCAConstraintMinY offset:50]];
	
	bottomGradientLayer.masksToBounds = YES;
	
	[maskLayer addSublayer:rightGradientLayer];
	[maskLayer addSublayer:leftGradientLayer];
	[maskLayer addSublayer:bottomGradientLayer];
	// we make it a sublayer rather than a mask so that the overlapping alpha will work correctly
	// without the use of a compositing filter
	[containerLayer addSublayer:maskLayer];
	
	/* Add Layer */
	[stadiumView setLayer:rootLayer];
	[stadiumView setWantsLayer:YES];
}

- (void) dealloc
{
	[selectedEntity release];
	[target release];
	[graphableObjects release];
	[graphControllers release];	
	[slices release];
	flowController.scroller = nil;
	[flowController autorelease];
	[super dealloc];
}

#pragma mark "Slice Management"

- (void) updateControllersAndLayers
{
	CGColorRef color;
	CGColorSpaceRef colorSpace;

	/*
	 * Clean out the old 
	 */
	
	while (slices.count > 0)
	{
		LCGraphStadiumSlice *slice = [slices objectAtIndex:0];
		[slice.sliceLayer removeFromSuperlayer];
		[slices removeObjectAtIndex:0];
	}
	
	/*
	 * Graph Controller Setup
	 */
	
	/* Filter and group metrics */
	NSMutableArray *objectArray = [NSMutableArray array];
	NSMutableArray *graphableMetrics = nil;
	if (target)
	{ graphableMetrics = [target graphableMetrics];	}
	else if (targetArray)
	{
		graphableMetrics = [NSMutableArray array];
		for (LCEntity *entity in targetArray)
		{ [graphableMetrics addObjectsFromArray:[entity graphableMetrics]]; }
	}
	NSPredicate *percentPredicate = [NSPredicate predicateWithFormat:@"units == %@", @"%"];
	if ([[graphableMetrics filteredArrayUsingPredicate:percentPredicate] count] > 0)
	{ [graphableMetrics filterUsingPredicate:percentPredicate]; }
	
	for (LCMetric *metric in graphableMetrics)
	{
		if (![metric.parent uniqueIdentifier])
		{ continue; }
		
		NSMutableArray *objectMetrics = [graphableObjects objectForKey:[metric.parent uniqueIdentifier]];
		if (!objectMetrics && graphableObjects.count < 50)
		{ 
			objectMetrics = [NSMutableArray array];
			[graphableObjects setObject:objectMetrics forKey:[metric.parent uniqueIdentifier]];
			[objectArray addObject:metric.parent];
		}
		[objectMetrics addObject:metric];
	}
	
	/* Create controllers and views */
	NSSortDescriptor *containerSortDesc = [[NSSortDescriptor alloc] initWithKey:@"parent.displayString" ascending:YES selector:@selector(localizedCompare:)];
	NSSortDescriptor *objectSortDesc = [[NSSortDescriptor alloc] initWithKey:@"displayString" ascending:YES  selector:@selector(localizedCompare:)];
	[objectArray sortUsingDescriptors:[NSArray arrayWithObjects:containerSortDesc, objectSortDesc, nil]];
	for (LCObject *object in objectArray)
	{
		/* Create graph controller */
		NSMutableArray *objectMetrics = [graphableObjects objectForKey:[object uniqueIdentifier]];
		LCMetricGraphController *graphController = [[LCMetricGraphController alloc] init];
		[graphController refreshGraph:XMLREQ_PRIO_HIGH];
		for (LCMetric *metric in objectMetrics)
		{
			[graphController addMetric:metric];
		}
		[graphControllers addObject:graphController];		
		[graphController autorelease];
	}
	[containerSortDesc release];
	[objectSortDesc release];	
	
	/* Check count */
	if (graphableObjects.count < 1) return;
	
	/* 
	 * Group into slices 
	 */
	
	int cols = 10;
	int rows = self.graphControllers.count / cols;
	if ((rows * cols) < self.graphControllers.count) rows++;
	
	/* Create Slices */
	int i;
	for (i=0; i < cols; i++)
	{ 
		LCGraphStadiumSlice *slice = [LCGraphStadiumSlice new];
		[slices addObject:slice]; 
		slice.index = i;
		slice.total = cols;
		slice.rows = rows;
		slice.stadiumController = self;
		[slice autorelease];
	}
	i=0;
	for (LCMetricGraphController *controller in graphControllers)
	{
		LCGraphStadiumSlice *slice = [slices objectAtIndex:(i % cols)];
		[slice insertObject:controller inGraphControllersAtIndex:slice.graphControllers.count];
		i++;
	}

	/* 
	 * Layer Setup
	 */

	/* Determine Geometry */
	CGSize sliceSize = CGSizeMake (NSWidth([stadiumView bounds]) / cols, NSHeight([stadiumView bounds]));
	
	/* Shadow Gradient */
	CGRect r;
	r.origin = CGPointZero;
	r.size = CGSizeMake(sliceSize.width, sliceSize.height);
	size_t bytesPerRow = (size_t) 4 * (size_t) r.size.width;
	void* bitmapData = calloc(bytesPerRow, r.size.height);
	colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
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
	shadowImage = CGBitmapContextCreateImage(context);
	CGContextRelease(context);
	free(bitmapData);
	[gradient release];
	
	NSMutableArray *sliceLayers = [NSMutableArray array];
	for (LCGraphStadiumSlice *slice in slices)
	{
		/* Check for existing layer */
		if (!slice.sliceLayer)
		{
			CGRect r;
			
			/* Create Layers */
			slice.sliceLayer = [CALayer layer];
			slice.sliceLayer.name = @"slice";
			slice.imageLayer = [LCGraphStadiumSliceLayer layer];
			[slice.imageLayer setNeedsDisplay];
			slice.imageLayer.delegate = slice;
			slice.imageLayer.controller = self;
			
			/* Create sublayer rotation perspective transform */
			float position = (float) (slice.index) / (float) (slice.total - 1);
			CGFloat positionFactor;
			if (position < 0.5)
			{ positionFactor = 0.5 - position; }
			else if (position > 0.5)
			{ positionFactor = position - 0.5; }
			else
			{ positionFactor = 0.0; }
			
			float zDistance = (sliceSize.width * ((slice.total / 4) + (positionFactor * (slice.total / 2))));
			CATransform3D sublayerTransform = CATransform3DIdentity; 
			sublayerTransform.m34 = 1. / -zDistance;
			
			/* Set default appearance for slice layer */
			r.origin = CGPointZero;
			r.size = sliceSize;
			slice.imageLayer.bounds = r;
			color = CGColorCreateGenericGray(0.05, 1.0);
			slice.imageLayer.backgroundColor = color;
			CGColorRelease(color);
			slice.imageLayer.name = @"image";
			slice.sliceLayer.bounds = r;
			color = CGColorCreateGenericRGB(0.0, 0.0, 0.0, 0.0);
			slice.sliceLayer.backgroundColor = color;
			CGColorRelease(color);
			[slice.sliceLayer setSublayers:[NSArray arrayWithObject:slice.imageLayer]];
			slice.sliceLayer.sublayerTransform = sublayerTransform;	
	
			// informative header text
			int i;
			for (i=0; i < slice.rows; i++)
			{
				if (i < [slice.graphControllers count])
				{
					LCMetricGraphController *graphController = [slice.graphControllers objectAtIndex:i];
					LCMetric *metric = [[graphController.metricItems objectAtIndex:0] metric];
					CATextLayer *textLayer = [LCTextLayerNoHit layer];
					textLayer.name = [NSString stringWithFormat:@"label%i", [slice.graphControllers indexOfObject:graphController]];
					NSString *labelString = @"";
					if ([[metric.object name] isEqualToString:@"master"] && [[metric.object container] displayString])
					{ labelString = [[metric.object container] displayString]; }
					else if ([metric.object displayString])
					{ labelString = [metric.object displayString]; }
					
					textLayer.string = labelString;
					textLayer.font = [NSFont systemFontOfSize:8.5];
					textLayer.alignmentMode = kCAAlignmentCenter;
					textLayer.fontSize = 8.5;
					textLayer.cornerRadius = 4;
					textLayer.wrapped = NO;
					textLayer.truncationMode = kCATruncationStart;
					color = CGColorCreateGenericGray(1.0, 0.7);
					textLayer.foregroundColor = color;
					CGColorRelease(color);
					[slice.imageLayer addSublayer:textLayer];		
					[slice.labelLayers addObject:textLayer];
				}
			}
					
			/* Create reflection layer */
			slice.reflectionLayer = [CALayer layer];
			r.origin = CGPointMake(0.0, -1.0 * r.size.height);
			slice.reflectionLayer.frame = r;
			slice.reflectionLayer.name = @"reflection";
			CATransform3D transform = CATransform3DMakeScale(1,-1,1);
			slice.reflectionLayer.transform = transform;
			color = CGColorCreateGenericGray(0.0, 0.0);
			slice.reflectionLayer.backgroundColor = color;
			CGColorRelease(color);
			slice.reflectionLayer.delegate = slice;
			[slice.imageLayer addSublayer:slice.reflectionLayer];
			[slice.reflectionLayer setNeedsDisplay];

			
			/* Create reflection gradient layer */
			slice.gradientLayer = [CALayer layer];
			slice.gradientLayer.name = @"reflection_gradient";
			r.origin.y += r.size.height;
			r.origin.x -= 0.5;
			r.size.height += 1.0;
			r.size.width += 1.0;
			slice.gradientLayer.frame = r;
			slice.gradientLayer.contents = (id) shadowImage;
			slice.gradientLayer.opaque = NO;
			[slice.reflectionLayer setMask:slice.gradientLayer];
		
			[slice.sliceLayer setValue:[NSNumber numberWithInt:slice.index] forKey:@"slice_index"];
			[slice.imageLayer setValue:[NSNumber numberWithInt:slice.index] forKey:@"slice_index"];
		}
		
		[sliceLayers addObject:slice.sliceLayer];
	}
	
	/* Add layers */
	[bodyLayer setSublayers:sliceLayers];
	[bodyLayer layoutIfNeeded];		
	
	/* Clean up */
	CGImageRelease(shadowImage);
}

#pragma mark "Properties"

@synthesize target;
- (void) setTarget:(LCEntity *)value
{
	[target release];
	target = [value retain];
	
	[self updateControllersAndLayers];
}
@synthesize targetArray;
- (void) setTargetArray:(NSArray *)value
{
	[targetArray release];
	targetArray = [value copy];
	
	[self updateControllersAndLayers];
}
@synthesize title;
@synthesize selectedEntity;
@synthesize graphableObjects;
@synthesize graphControllers;
@synthesize slices;
@synthesize stadiumView;
@synthesize flowController;
@synthesize bodyLayer;

@end
