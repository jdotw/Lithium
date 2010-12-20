//
//  LTEntityIconView.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 4/11/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTEntityIconView.h"
#import "LTEntity.h"

@implementation LTEntityIconView

@synthesize selected, selectedBackgroundImage, entity;

- (id)initWithFrame:(CGRect)frame {
    
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code.
    }
    return self;
}

- (void)drawRect:(CGRect)rect 
{	
	/* Calculate the rest to use for background
	 * and selection washes; use width -2. to not
	 * draw on the bevel
	 */
	CGRect washRect = self.bounds;
	washRect.size.width -= 2.;

	/* Draw entity status wash */
	UIColor *statusColor = nil;
	UIImage *statusImage = nil;
	switch (entity.opState)
	{
		case 1:
			statusImage = [UIImage imageNamed:@"LTTableViewCellBack-Yellow.png"];
			statusColor = [UIColor yellowColor];
			break;
		case 2:
			statusImage = [UIImage imageNamed:@"LTTableViewCellBack-Orange.png"];
			statusColor = [UIColor orangeColor];
			break;
		case 3:
			statusImage = [UIImage imageNamed:@"LTTableViewCellBack-Red.png"];
			statusColor = [UIColor redColor];
			break;
	}	
//	[statusImage drawInRect:washRect blendMode:kCGBlendModeColor alpha:0.2];	
	if (statusColor)
	{
		[statusColor setFill];
		[[UIBezierPath bezierPathWithRect:washRect] fillWithBlendMode:kCGBlendModeHue alpha:0.25];
	}
	
	
	/* Selected overlay wash */
	if (self.selected && self.selectedBackgroundImage)
	{
		[self.selectedBackgroundImage drawInRect:washRect blendMode:kCGBlendModeNormal alpha:0.4];		
	}
	
	/* Draw right-side edge */
	CGRect vertBorderRect = CGRectMake(CGRectGetMaxX(self.bounds)-2.0, CGRectGetMinY(self.bounds), 1.0, CGRectGetHeight(self.bounds));
	[[UIColor colorWithWhite:0.0 alpha:1.0] setFill];
	[[UIBezierPath bezierPathWithRect:vertBorderRect] fillWithBlendMode:kCGBlendModeDarken alpha:0.3];
	[[UIColor colorWithWhite:1.0 alpha:1.0] setFill];
	[[UIBezierPath bezierPathWithRect:CGRectOffset(vertBorderRect, 1.0, 0.0)] fillWithBlendMode:kCGBlendModeDarken alpha:0.25];

}

- (void)dealloc {
    [super dealloc];
}


@end
