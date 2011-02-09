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
#import "LTMetricTableViewController.h"

@implementation LTGraphLegendEntityView

@synthesize swatchColor,entity;

- (id)initWithFrame:(CGRect)frame 
{
    if ((self = [super initWithFrame:frame])) 
	{
		self.opaque = NO;
		
        // Initialization code
        ledView = [[UIImageView alloc] initWithFrame:CGRectZero];
        ledView.userInteractionEnabled = NO;
        [self addSubview:ledView];
        [ledView release];
        
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
	UITableViewController *vc;
    if (self.entity.type == 6) 
    {
        vc = [[LTMetricTableViewController alloc] initWithMetric:self.entity];
    }
    else
    {
        vc = [[LTEntityTableViewController alloc] initWithEntity:self.entity];
    }
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
    /* Cell Layout 
     *
     * [ LED ] [ Swatch ] [ Description ] [ Value ]
     */
    
    /* LED -- Left-hand-side square based on cell height
     *
     * The rect width will be 0 if there are no triggers
     */
    CGRect ledRect = CGRectMake(0.0, 0.0, CGRectGetHeight(self.bounds), CGRectGetHeight(self.bounds));
    ledView.frame = ledRect;
	
	/* Swatch */
    CGFloat swatchXOffset = CGRectGetMaxX(ledRect);
    CGRect swatchRect = CGRectMake(swatchXOffset, CGRectGetMidY(self.bounds)-(12.*0.5), 12., 12.);
    BOOL swatchVisible;
    if (self.swatchColor == [UIColor clearColor])
    {
        /* Swatch is clear, zero width */
        swatchRect.size.width = 0.;
        swatchVisible = NO;
    }
    else
    {
        /* Swatch has color, normal width */
        swatchVisible = YES;
    }
    swatchView.frame = swatchRect;
	
	/* Labels */
    CGFloat labelXOffset = CGRectGetMaxX(swatchRect) + (swatchVisible ? 8. : 0.);   // Offset depends on whether swatch is shown
    CGFloat labelXPadding = 8.;
	CGFloat descValueSplit = 0.7;
    
    /* Calculate rect containing both labels */
	CGRect labelRect = CGRectMake(labelXOffset, CGRectGetMidY(self.bounds)-(descLabel.font.pointSize*0.5), 
								   CGRectGetWidth(self.bounds) - labelXOffset - labelXPadding, descLabel.font.pointSize);
	
    /* Calculate desc label using descValueSplit */
	CGRect descLabelRect = labelRect;
	descLabelRect.size.width *= descValueSplit;
	descLabel.frame = CGRectIntegral(descLabelRect);

    /* Calculate value label using descValueSplit */
	CGRect valueLabelRect = labelRect;
	valueLabelRect.size.width = (labelRect.size.width * (1.0-descValueSplit)) - (2.0 * labelXPadding);
	valueLabelRect.origin.x = CGRectGetMaxX(descLabelRect) + labelXPadding;
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
}

- (void)dealloc {
    self.entity = nil;
	[swatchColor release];
	[entity release];
    [super dealloc];
}

- (void) deviceRefreshFinished:(NSNotification *)note
{
	/* What used to be done here is now done on entity state 
     * and value change notifications
     */
    
    // No longer used
}

- (void) entityStateChanged:(NSNotification *)note
{
    /* Entity state changed, force a redraw */
    [self setNeedsDisplay];
}

- (void) entityValueChanged:(NSNotification *)note
{
    /* Entity value changes, update strings */
	descLabel.text = entity ? [NSString stringWithFormat:@"%@ %@", entity.parent.desc, entity.desc] : @" ";
	valueLabel.text = entity.currentValue ? : @" ";
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"bits" withString:@"b"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"bytes" withString:@"B"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"bit" withString:@"b"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"byte" withString:@"B"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"writes" withString:@"wr"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"reads" withString:@"rd"];
	valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"sec" withString:@"s"];
    switch (entity.opState)
    {
        case 3:
            ledView.image = [UIImage imageNamed:@"LED-Red.png"];
            break;
        case 2:
            ledView.image = [UIImage imageNamed:@"LED-Orange.png"];
            break;
        case 1:
            ledView.image = [UIImage imageNamed:@"LED-Yellow.png"];
            break;
        case 0:
            ledView.image = [UIImage imageNamed:@"LED-Green.png"];
            break;
        default:
            ledView.image = [UIImage imageNamed:@"LED-Red.png"];
            break;
    }
    ledView.hidden = !entity.hasTriggers;
	
	[self setNeedsDisplay];
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
	if (entity)
	{
        [[NSNotificationCenter defaultCenter] removeObserver:self
                                                        name:kLTEntityStateChanged
                                                      object:entity];
        [[NSNotificationCenter defaultCenter] removeObserver:self
                                                        name:kLTEntityValueChanged
                                                      object:entity];
	}
	
	[entity release];
	entity = [value retain];
    
    /* Update View for new Entity */
    [self entityStateChanged:nil];
    [self entityValueChanged:nil];
	
	if (entity)
	{
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(entityStateChanged:)
													 name:kLTEntityStateChanged
												   object:entity];
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(entityValueChanged:)
													 name:kLTEntityValueChanged
												   object:entity];
	}
	
	[self setNeedsLayout];
}

@end
