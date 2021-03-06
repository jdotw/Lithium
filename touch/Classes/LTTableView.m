//
//  LTTableView.m
//  Lithium
//
//  Created by James Wilson on 2/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTTableView.h"

#define SHADOW_HEIGHT 30
#define SHADOW_INVERSE_OFFSET 15;

@implementation LTTableView

- (void) drawRect:(CGRect)theRect
{
	UIImage *image = [UIImage imageNamed:@"Mesh.png"];
	CGRect imageRect = CGRectMake(CGRectGetMinX(self.bounds),
								  CGRectGetMinY(self.bounds),
								  image.size.width, image.size.height);
	while (CGRectGetMinY(imageRect) < CGRectGetMaxY(self.bounds))
	{
		while (CGRectGetMinX(imageRect) < CGRectGetMaxX(self.bounds))
		{
			[image drawInRect:imageRect blendMode:kCGBlendModeSourceAtop alpha:0.2];
			imageRect.origin.x = imageRect.origin.x + image.size.width;			
		}
		imageRect.origin.y = imageRect.origin.y + image.size.height;
		imageRect.origin.x = CGRectGetMinX(self.bounds);
	}
}

@end
