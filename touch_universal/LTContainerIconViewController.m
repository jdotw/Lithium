//
//  LTContainerIconViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 7/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTContainerIconViewController.h"
#import "LTEntity.h"
#import "LTEntityIconView.h"
#import "LTMetricGraphRequest.h"
#import "LTContainerIconViewGraphView.h"

@implementation LTContainerIconViewController

- (id) initWithContainer:(LTEntity *)container
{
	self = [super initWithNibName:@"LTContainerIconViewController" entity:container];
	if (!self) return nil;
	
	((LTEntityIconView *)[self view]).selectedBackgroundImage = [UIImage imageNamed:@"LTContainerIconViewBack-Selected"];
	graphView.container = container;
	
	return self;
}

- (void) iconTapped:(UIGestureRecognizer *)recog
{
	if ([delegate respondsToSelector:@selector(setSelectedContainer:)])
	{
		[delegate performSelector:@selector(setSelectedContainer:) withObject:self.entity];
	}
}
							   
- (void) dealloc
{
	[graphReq release];
	[super dealloc];
}

@end
