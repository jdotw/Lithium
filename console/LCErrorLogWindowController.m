//
//  LCErrorLogWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 30/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCErrorLogWindowController.h"

static LCErrorLogWindowController *masterController = nil;

@implementation LCErrorLogWindowController

+ (LCErrorLogWindowController *) errorLogController
{
	if (masterController)
	{
		[[masterController window] makeKeyAndOrderFront:self];
		return nil;
	}
	else
	{
		masterController = [[LCErrorLogWindowController alloc] init];
		return masterController;
	}
}

- (LCErrorLogWindowController *) init
{
	[super initWithWindowNibName:@"ErrorLogWindow"];
	
	[self window];
	[self setShouldCascadeWindows:NO];	
	[[self window] makeKeyAndOrderFront:self];
	
	[backView setImage:[NSImage imageNamed:@"slateback.png"]];
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

- (LCErrorLog *) errorLog
{
	return [LCErrorLog masterLog];
}

- (IBAction) clearLogClicked:(id)sender
{
	[[LCErrorLog masterLog] clearLog];
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Autorelease */
	masterController = nil;
	[controllerAlias setContent:nil];
	[self autorelease];
}

@synthesize backView;
@synthesize controllerAlias;
@end
