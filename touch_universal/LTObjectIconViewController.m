    //
//  LTObjectIconViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 2/11/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTObjectIconViewController.h"
#import "LTEntity.h"

@implementation LTObjectIconViewController

- (id) initWithObject:(LTEntity *)object
{
	self = [super initWithNibName:@"LTObjectIconViewController" entity:object];
	if (!self) return nil;
	
	return self;
}

- (void) iconTapped:(UIGestureRecognizer *)recog
{
	if ([delegate respondsToSelector:@selector(setSelectedObject:)])
	{
		[delegate performSelector:@selector(setSelectedObject:) withObject:self.entity];
	}
}


@end
