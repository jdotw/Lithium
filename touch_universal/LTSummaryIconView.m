//
//  LTSummaryIconView.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 14/10/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTSummaryIconView.h"
#import "LTEntity.h"
#import "LTEntityTableViewController.h"
#import "AppDelegate_Pad.h"

@implementation LTSummaryIconView

@synthesize entity;

- (id)initWithEntity:(LTEntity *)initEntity
{    
    self = [super initWithFrame:CGRectZero];
    if (self) 
	{
		/* View setup */
		self.opaque = NO;
		
		/* Create UI Elements */
		label = [[UILabel alloc] initWithFrame:CGRectZero];
		label.textAlignment = UITextAlignmentCenter;
		label.userInteractionEnabled = NO;
		[self addSubview:label];
		[label release];

		UIGestureRecognizer *touchRecog = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(buttonTouched:)];
		[self addGestureRecognizer:touchRecog];
		
		UIGestureRecognizer *longHoldRecog = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(buttonHeld:)];
		longHoldRecog.cancelsTouchesInView = YES;
		[self addGestureRecognizer:longHoldRecog];
		
		
//		button = [UIButton buttonWithType:UIButtonTypeCustom];
//		[button addTarget:self action:@selector(buttonTouched:) forControlEvents:UIControlEventTouchUpInside];
//		[button addG
//		[self addSubview:button];

		/* Set entity */
		self.entity = initEntity;
    }
    return self;
}

- (void) layoutSubviews
{
	/* Label Layout */
	CGFloat labelPadding = 4.0;
	CGFloat labelHeight = roundf (CGRectGetHeight(self.bounds)*0.08);
	label.frame = CGRectMake(CGRectGetMinX(self.bounds)+labelPadding, CGRectGetMaxY(self.bounds)-labelPadding-labelHeight, CGRectGetWidth(self.bounds)-(2*labelPadding), labelHeight);
	label.font = [UIFont systemFontOfSize:labelHeight];
	
	/* Button */
	button.frame = self.bounds;
}

- (void)drawRect:(CGRect)rect 
{
    // Drawing code.
}

- (void) buttonTouched:(id)sender
{
	AppDelegate_Pad *appDelegate = (AppDelegate_Pad *) [[UIApplication sharedApplication] delegate];
	[appDelegate displayEntityInDetailView:self.entity];
}

- (void) buttonHeld:(UILongPressGestureRecognizer *)recog
{
	if (recog.state == UIGestureRecognizerStateBegan)
	{
		/* Show Pop-up of Device */
		LTEntityTableViewController *tvc = [[LTEntityTableViewController alloc] initWithStyle:UITableViewStylePlain];
		tvc.entity = self.entity;
		UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:tvc];
		UIPopoverController *popOver = [[UIPopoverController alloc] initWithContentViewController:nav];
		[popOver presentPopoverFromRect:self.bounds inView:self permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
		[tvc release];
	}
}

- (void)dealloc 
{
	[entity release];
    [super dealloc];
}

- (void) setEntity:(LTEntity *)value
{
	[entity release];
	entity = [value retain];
	
	label.text = entity.desc;
}


@end
