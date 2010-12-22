//
//  LTGraphLegendEntityView.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 8/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTGraphLegendEntityView.h"
#import "LTEntity.h"
#import "LTEntityTableViewController.h"


@implementation LTGraphLegendEntityView

@synthesize swatchColor,entity;

- (id)initWithFrame:(CGRect)frame 
{
    if ((self = [super initWithFrame:frame])) 
	{
		self.opaque = NO;
		
        // Initialization code
		swatchView = [[UIView alloc] initWithFrame:CGRectZero];
		swatchView.userInteractionEnabled = NO;
		[self addSubview:swatchView];
		[swatchView release];

		descLabel = [[UILabel alloc] initWithFrame:CGRectZero];
		descLabel.backgroundColor = [UIColor clearColor];
		descLabel.opaque = NO;
		descLabel.textColor = [UIColor whiteColor];
		descLabel.highlightedTextColor = [UIColor grayColor];
		descLabel.font = [UIFont boldSystemFontOfSize:12.0];
		descLabel.textAlignment = UITextAlignmentLeft;
		descLabel.userInteractionEnabled = NO;
		descLabel.shadowColor = [UIColor colorWithWhite:0.0 alpha:0.3];
		descLabel.shadowOffset = CGSizeMake(0.0, -1.0);
		[self addSubview:descLabel];
		[descLabel release];
		
		valueLabel = [[UILabel alloc] initWithFrame:CGRectZero];
		valueLabel.backgroundColor = [UIColor clearColor];
		valueLabel.textColor = [UIColor whiteColor];
		valueLabel.highlightedTextColor = [UIColor grayColor];
		valueLabel.font = [UIFont systemFontOfSize:12.0];
		valueLabel.textAlignment = UITextAlignmentRight;
		valueLabel.userInteractionEnabled = NO;
		valueLabel.shadowColor = [UIColor colorWithWhite:0.0 alpha:0.3];
		valueLabel.shadowOffset = CGSizeMake(0.0, -1.0);
		[self addSubview:valueLabel];
		[valueLabel release];
		
		UITapGestureRecognizer *touchRecog = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(legendTapped:)];
		touchRecog.numberOfTapsRequired = 1;
		[self addGestureRecognizer:touchRecog];
    }
    return self;
}

- (UIPopoverController *) presentPopoverForEntityFromRect:(CGRect)rect
{
	/* Draw pop-over for the entity */
	LTEntityTableViewController *vc = [[LTEntityTableViewController alloc] initWithEntity:self.entity.parent];
	UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:vc];
	UIPopoverController *popover = [[UIPopoverController alloc] initWithContentViewController:nav];
	if (self.frame.size.height > 0. && self.frame.size.width > 0.)
	{
		[popover presentPopoverFromRect:rect inView:self permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];	
		popoverWaitingToBeDisplayed = nil;
	}
	else 
	{
		popoverWaitingToBeDisplayed = popover;
		popoverWaitingToBeDisplayedRect = rect;
	}

	return popover;
}

- (void) legendTapped:(UITapGestureRecognizer *)gestureRecog
{
	CGPoint touchPoint = [gestureRecog locationOfTouch:0 inView:self];
	[self presentPopoverForEntityFromRect:CGRectMake(touchPoint.x, touchPoint.y, 1.0, 1.0)];
}

- (void) layoutSubviews
{
	CGFloat padding = 8.0;
	
	/* Swatch */
	CGFloat swatchWidth=12.0;
	swatchView.frame = CGRectMake(padding, CGRectGetMidY(self.bounds)-(swatchWidth*0.5), swatchWidth, swatchWidth);
	
	/* Labels */
	CGFloat descValueSplit = 0.7;
	CGRect labelRect = CGRectMake(CGRectGetMaxX(swatchView.frame)+padding, CGRectGetMidY(self.bounds)-(descLabel.font.pointSize*0.5), 
								   CGRectGetWidth(self.bounds) - ((CGRectGetMaxX(swatchView.frame)+padding) + padding), descLabel.font.pointSize);
	
	CGRect descLabelRect = labelRect;
	descLabelRect.size.width *= descValueSplit;
	descLabel.frame = CGRectIntegral(descLabelRect);;

	CGRect valueLabelRect = labelRect;
	valueLabelRect.size.width = (labelRect.size.width * (1.0-descValueSplit)) - (2.0 * padding);
	valueLabelRect.origin.x = CGRectGetMaxX(descLabelRect) + padding;
	valueLabel.frame = CGRectIntegral(valueLabelRect);
	
	/* Check to see if a popover is waitingto be displayed */
	if (popoverWaitingToBeDisplayed && self.frame.size.height > 0. && self.frame.size.width > 0.)
	{
		/* There's a popover to be displayed which could not be 
		 * presented earlier because our frame had not been set.
		 * We can now display this popover
		 */
		if (CGRectEqualToRect(popoverWaitingToBeDisplayedRect, CGRectZero)) 
		{ popoverWaitingToBeDisplayedRect = self.bounds; }
		[popoverWaitingToBeDisplayed presentPopoverFromRect:popoverWaitingToBeDisplayedRect 
													 inView:self 
								   permittedArrowDirections:UIPopoverArrowDirectionAny 
												   animated:YES];	
		popoverWaitingToBeDisplayed = nil;
	}
		
}

- (void) drawRect:(CGRect)rect
{
	[super drawRect:rect];
	
	UIBezierPath *outineOuter = [UIBezierPath bezierPathWithRoundedRect:self.bounds cornerRadius:6.0];
	[[UIColor colorWithWhite:0.0 alpha:0.1] setFill];
	[outineOuter addClip];
	[outineOuter fill];
	
	UIBezierPath *outlineDarkInnder = [UIBezierPath bezierPathWithRoundedRect:CGRectOffset(self.bounds, 1.0, 1.0) cornerRadius:6.0];
	[[UIColor colorWithWhite:0.0 alpha:0.1] setStroke];
	[outlineDarkInnder stroke];

	UIBezierPath *outlineLightInnder = [UIBezierPath bezierPathWithRoundedRect:CGRectOffset(self.bounds, -1.0, -1.0) cornerRadius:6.0];
	[[UIColor colorWithWhite:1.0 alpha:0.1] setStroke];
	[outlineLightInnder stroke];

	/* Draw clipped status color */
	UIBezierPath *statusClipPath = [UIBezierPath bezierPathWithRoundedRect:CGRectMake(CGRectGetMinX(self.bounds)+1.0, CGRectGetMinY(self.bounds)+1,
																					  CGRectGetWidth(self.bounds)-2.0, CGRectGetHeight(self.bounds)-2.0)
															  cornerRadius:6.0];
	[statusClipPath addClip];
	UIImage *statusImage = nil;
	switch (entity.opState)
	{
		case 1:
			statusImage = [UIImage imageNamed:@"LTTableViewCellBack-Yellow.png"];
			break;
		case 2:
			statusImage = [UIImage imageNamed:@"LTTableViewCellBack-Orange.png"];
			break;
		case 3:
			statusImage = [UIImage imageNamed:@"LTTableViewCellBack-Red.png"];
			break;
	}	
	[statusImage drawInRect:self.bounds blendMode:kCGBlendModeNormal alpha:0.7];
}

- (void)dealloc {
	[swatchColor release];
	[entity release];
    [super dealloc];
}

- (void) setSwatchColor:(UIColor *)value
{
	[swatchColor release];
	swatchColor = [value retain];
	
	if (swatchColor) swatchView.backgroundColor = swatchColor;
	else swatchView.backgroundColor = [UIColor clearColor];
}

- (void) setEntity:(LTEntity *)value
{
	[entity release];
	entity = [value retain];
	
	descLabel.text = entity ? [NSString stringWithFormat:@"%@ %@", entity.parent.desc, entity.desc] : @" ";
	valueLabel.text = entity.currentValue ? : @" ";
	
	[self layoutSubviews];
	
}

@end
