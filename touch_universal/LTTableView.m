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

+ (UITableViewCellStyle) defaultCellStyle
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
		return UITableViewStylePlain;
	}
	else
	{
		return UITableViewStylePlain;
	}
}
		
- (void) drawRect:(CGRect)theRect
{
	NSLog (@"backgroundView is %@", self.backgroundView);
	if (
#ifdef UI_USER_INTERFACE_IDIOM
		UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad
#else
		0
#endif
		)
	{
		/* iPad Background */
		[super drawRect:theRect];
	}
	else
	{
		if (self.backgroundView) self.backgroundView = nil;
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
