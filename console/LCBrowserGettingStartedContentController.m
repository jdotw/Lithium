//
//  LCBrowserGettingStartedContentController.m
//  Lithium Console
//
//  Created by James Wilson on 23/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserGettingStartedContentController.h"


@implementation LCBrowserGettingStartedContentController

- (id) initWithBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [self initWithNibName:@"GettingStartedContent" bundle:nil];
	if (!self) return nil;
	
	/* Set/Create objects */
	self.browser = initBrowser;
	
	/* Load the NIB */
	[self loadView];
	
	return self;	
}

- (void) removedFromBrowserWindow
{
	/* Shutdown and prepare to be autoreleased */
	[super removedFromBrowserWindow];
}

- (void) dealloc
{
	[super dealloc];
}

@synthesize browser;


@end
