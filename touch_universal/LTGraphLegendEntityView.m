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
        // Initialization code
		swatchView = [[UIView alloc] initWithFrame:CGRectZero];
		swatchView.userInteractionEnabled = NO;
		[self addSubview:swatchView];
		[swatchView release];

		label = [[UILabel alloc] initWithFrame:CGRectZero];
		label.backgroundColor = [UIColor clearColor];
		label.opaque = NO;
		label.textColor = [UIColor whiteColor];
		label.highlightedTextColor = [UIColor grayColor];
		label.font = [UIFont boldSystemFontOfSize:12.0];
		label.textAlignment = UITextAlignmentLeft;
		label.userInteractionEnabled = NO;
		[self addSubview:label];
		[label release];
		
		UITapGestureRecognizer *touchRecog = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(legendTapped:)];
		touchRecog.numberOfTapsRequired = 1;
		[self addGestureRecognizer:touchRecog];
    }
    return self;
}

- (void) presentPopoverForEntityFromRect:(CGRect)rect
{
	/* Draw pop-over for the entity */
	LTEntityTableViewController *vc = [[LTEntityTableViewController alloc] initWithStyle:UITableViewStylePlain];
	UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:vc];
	vc.entity = self.entity.parent;
	UIPopoverController *popover = [[UIPopoverController alloc] initWithContentViewController:nav];
	[popover presentPopoverFromRect:rect inView:self permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];	
}

- (void) legendTapped:(UITapGestureRecognizer *)gestureRecog
{
	CGPoint touchPoint = [gestureRecog locationOfTouch:0 inView:self];
	[self presentPopoverForEntityFromRect:CGRectMake(touchPoint.x, touchPoint.y, 1.0, 1.0)];
}

- (void) layoutSubviews
{
	CGFloat padding = 4.0;
	
	/* Swatch */
	CGFloat swatchWidth=12.0;
	swatchView.frame = CGRectMake(padding, CGRectGetMidY(self.bounds)-(swatchWidth*0.5), swatchWidth, swatchWidth);
	
	/* Label */
	label.frame = CGRectMake(CGRectGetMaxX(swatchView.frame)+padding, CGRectGetMidY(self.bounds)-(label.font.pointSize*0.5), 
							 CGRectGetWidth(self.bounds) - ((CGRectGetMaxX(swatchView.frame)+padding) + padding), label.font.pointSize);	
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
	
	if (entity) label.text = [NSString stringWithFormat:@"%@ %@", entity.parent.desc, entity.desc];
	else label.text = @"";
	
	[self layoutSubviews];
	
}

@end
