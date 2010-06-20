//
//  LTWindow.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 9/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTWindow.h"


@implementation LTWindow

- (void) drawRect:(CGRect)rect
{
	if (
#ifdef UI_USER_INTERFACE_IDIOM
		UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad
#else
		0
#endif
		)
	{
		/* iPad */
		UIImage *image = [UIImage imageNamed:@"circuitback.png"];
		[image drawInRect:self.bounds blendMode:kCGBlendModeSourceAtop alpha:0.2];	
	}
	else
	{
		UIImage *image = [UIImage imageNamed:@"Mesh.png"];
		CGRect imageRect = CGRectMake(CGRectGetMinX(self.bounds),
									  CGRectGetMinY(self.bounds),
									  image.size.width, image.size.height);
		while (CGRectGetMinY(imageRect) < CGRectGetMaxY(self.bounds))
		{
			while (CGRectGetMinX(imageRect) < CGRectGetMaxX(self.bounds))
			{
				[image drawInRect:imageRect blendMode:kCGBlendModeSourceAtop alpha:0.1];
				imageRect.origin.x = imageRect.origin.x + image.size.width;			
			}
			imageRect.origin.y = imageRect.origin.y + image.size.height;
			imageRect.origin.x = CGRectGetMinX(self.bounds);
		}		
	}	
}

@end
