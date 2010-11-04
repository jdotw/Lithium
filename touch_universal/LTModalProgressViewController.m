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

- (void) entityRefreshFinished:(NSNotification *)note
{
	progressLabel.text = @"Finished.";
	if (hasAppeared)
	{
		NSLog (@"%@ dismissing immediately", self);
		[self dismissModalViewControllerAnimated:YES];
	}
	else
	{
		NSLog (@"%@ deferring dismissal until it appears", self);
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
		NSLog (@"%@ performing deferred dismissal on appearance", self);
	}
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
