//
//  LCWatcherWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 12/01/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCWatcherWindowController.h"


@implementation LCWatcherWindowController

#pragma mark "Initialisation"

- (LCWatcherWindowController *) init
{
	/* Load NIB */
	[super initWithWindowNibName:@"WatcherWindow"];
	[[self window] makeKeyAndOrderFront:self];
	
	return self;
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Remove bindings/observers */
	[controllerAlias setContent:nil];
	
	/* Autorelease */
	[self autorelease];
}

@synthesize controllerAlias;
@end
