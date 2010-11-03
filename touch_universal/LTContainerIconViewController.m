//
//  LTContainerIconViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 7/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTContainerIconViewController.h"
#import "LTEntity.h"

@implementation LTContainerIconViewController

- (id) initWithContainer:(LTEntity *)container
{
	self = [super initWithNibName:@"LTContainerIconViewController" entity:container];
	if (!self) return nil;
	
	return self;
}

- (void) iconTapped:(UIGestureRecognizer *)recog
{
	if ([delegate respondsToSelector:@selector(setSelectedContainer:)])
	{
		[delegate performSelector:@selector(setSelectedContainer:) withObject:self.entity];
	}
}

@end
