//
//  LCGraphStadiumLayout.m
//  Lithium Console
//
//  Created by James Wilson on 29/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCGraphStadiumLayout.h"

#import "LCGraphStadiumSlice.h"

CGFloat DegreesToRadians(CGFloat degrees) {return degrees * M_PI / 180;};
CGFloat RadiansToDegrees(CGFloat radians) {return radians * 180 / M_PI;};

@implementation LCGraphStadiumLayout

- (void)layoutSublayersOfLayer:(CALayer *)rootLayer
{
	CALayer *layer = rootLayer;
	while (layer)
	{
		layer = layer.superlayer;
	}
	for (CALayer *sliceLayer in [rootLayer sublayers])
	{
		NSNumber *sliceIndex = [sliceLayer valueForKey:@"slice_index"];
		if (!sliceIndex) continue;		
		LCGraphStadiumSlice *slice = [controller.slices objectAtIndex:[sliceIndex intValue]];
		
		if (slice.total < 1) continue;
		
		CGFloat containerWidth = CGRectGetWidth(rootLayer.frame);
		CGFloat sliceWidth = containerWidth / (float) slice.total;
		CGFloat sliceHeight = CGRectGetHeight(rootLayer.frame) * 0.595;

		CGRect sliceRect = CGRectMake(sliceWidth * (float) slice.index, CGRectGetHeight(rootLayer.bounds) * 0.375, sliceWidth, sliceHeight);
		CGRect imageRect = CGRectMake(0.0, 0.0, sliceWidth, sliceHeight);
		
		/* Width boost for outlier */
		if (slice.index == 0 || slice.index == 9)
		{
			sliceRect.size.width = sliceRect.size.width * 1.08;
			imageRect.size.width = imageRect.size.width * 1.08;
		}
		if (slice.index == 1 || slice.index == 8)
		{
			sliceRect.size.width = sliceRect.size.width * 1.065;
			imageRect.size.width = imageRect.size.width * 1.065;
		}
		if (slice.index == 2)
		{
			sliceRect.origin.x = sliceRect.origin.x * 1.01;
			imageRect.origin.x = imageRect.origin.x * 1.01;
		}
		if (slice.index == 7)
		{
			sliceRect.origin.x = sliceRect.origin.x * 0.998;
			imageRect.origin.x = imageRect.origin.x * 0.998;
		}
		if (slice.index == 8)
		{
			sliceRect.origin.x = sliceRect.origin.x * 0.992;
			imageRect.origin.x = imageRect.origin.x * 0.992;
		}
		if (slice.index == 9)
		{
			sliceRect.origin.x = sliceRect.origin.x * 0.992;
			imageRect.origin.x = imageRect.origin.x * 0.992;
		}
		
		[CATransaction begin];
		[CATransaction setValue:(id)kCFBooleanTrue
						 forKey:kCATransactionDisableActions];		
		
		slice.imageLayer.frame = imageRect;
		
		CGRect reflectionRect = slice.reflectionLayer.frame;
		reflectionRect.size.width = imageRect.size.width;		

		imageRect.origin = CGPointMake(0.0, -1.0 * imageRect.size.height);
		slice.reflectionLayer.frame = imageRect; 

		imageRect.origin.y += imageRect.size.height;
		imageRect.origin.x -= 0.5;
		imageRect.size.height += 1.0;
		imageRect.size.width += 1.0;
		slice.gradientLayer.frame = imageRect;
		
		imageRect.origin = CGPointMake(0.0, 0.0);
		slice.reflectionLayer.bounds = imageRect;
		slice.gradientLayer.bounds = imageRect;

		sliceLayer.frame = sliceRect;
				
		float position = (float) (slice.index) / (float) (slice.total - 1);
		float angle = position;
		
		float angles[slice.total];
		angles[0] = -0.4500;
		angles[1] = -0.3900;
		angles[2] = -0.2778;
		angles[3] = -0.1667;
		angles[4] = -0.0556;
		angles[5] = 0.0556;
		angles[6] = 0.1667;
		angles[7] = 0.2778;
		angles[8] = 0.3900;
		angles[9] = 0.4500;
		
		CGFloat positionFactor = sin(DegreesToRadians(180.0 * position));
		
		CGFloat scaleFactor = 1.0 - (positionFactor * 0.3);

		CATransform3D transform = CATransform3DMakeRotation(angles[slice.index], 0, -1, 0);

		if (position < 0.5) 
		{
			/* Left */
			transform = CATransform3DScale(transform, 1.0, scaleFactor, 1.0);			
			slice.imageLayer.transform = transform;
		}
		else if (position > 0.5) 
		{
			/* Right */
			transform = CATransform3DScale(transform, 1.0, scaleFactor, 1.0);			
			slice.imageLayer.transform = transform;
			
		}
		else if (position == 0.5)
		{
			CATransform3D centreTransform = CATransform3DMakeRotation(angle - 0.4999, 0, -1, 0);
			centreTransform = CATransform3DScale(centreTransform, 1.0, scaleFactor, 1.0);			
			slice.imageLayer.transform = centreTransform;
		}
		
		int i;
		for (i=0; i < slice.rows; i++)
		{
			if (i < [slice.graphControllers count])
			{
				CATextLayer *labelLayer = [slice.labelLayers objectAtIndex:i];
				CGFloat labelHeight = sliceLayer.bounds.size.height / slice.rows;
				CGFloat textHeight = labelLayer.fontSize;
				CGRect labelRect = CGRectMake(2.0, (labelHeight * (slice.rows - i - 1)) + (labelHeight * 0.5) - (textHeight * 0.5), CGRectGetWidth(sliceLayer.bounds) - 4.0, 10.0);
				labelLayer.frame = labelRect;
			}
		}
		
		[CATransaction commit];
		
		[slice.imageLayer setNeedsDisplay];
	
	}
}

@synthesize controller;

@end
