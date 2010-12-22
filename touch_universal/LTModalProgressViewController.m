    //
//  LTModalProgressViewController.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 8/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTModalProgressViewController.h"
#import "LTEntity.h"

@implementation LTModalProgressViewController

@synthesize entity;

- (id) initWithEntity:(LTEntity *)initEntity
{
	self = [super initWithNibName:@"LTModalProgressViewController" bundle:nil];
	if (!self) return nil;
	
	self.entity = initEntity;
	
	return self;
}

- (void) entityRefreshFinished:(NSNotification *)note
{
	progressLabel.text = @"Finished.";
	if (hasAppeared)
	{
		[self dismissModalViewControllerAnimated:YES];
	}
	else
	{
		shouldDismissWhenAppears = YES;
	}
}

- (void) entityRefreshStatusUpdated:(NSNotification *)note
{
	progressLabel.text = entity.xmlStatus;
}

- (void) setEntity:(LTEntity *)value
{
	[entity release];
	entity = [value retain];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(entityRefreshFinished:)
												 name:@"RefreshFinished" object:entity];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(entityRefreshStatusUpdated:)
												 name:@"LTEntityXmlStatusChanged" object:entity];	
}

- (void) viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];
	[self entityRefreshStatusUpdated:nil];	
	hasAppeared = YES;
	if (shouldDismissWhenAppears) 
	{
		[self dismissModalViewControllerAnimated:YES];
	}
}

- (BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
	return YES;
}

- (void)dealloc 
{
	if (entity)
	{
		[[NSNotificationCenter defaultCenter] removeObserver:self name:@"RefreshFinished" object:entity];
		[[NSNotificationCenter defaultCenter] removeObserver:self name:@"LTEntityXmlStatusChanged" object:entity];
		[entity release];
	}
    [super dealloc];
}


@end
