//
//  LCCaseLogUpdateController.m
//  Lithium Console
//
//  Created by James Wilson on 9/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCCaseLogUpdateController.h"


@implementation LCCaseLogUpdateController

- (id) initWithCase:(LCCase *)initCase closeCase:(BOOL)initCloseFlag
{
	self = [super initWithWindowNibName:@"CaseLogUpdateWindow"];
	if (!self) return nil;
	
	self.cas = initCase;
	self.closeFlag = initCloseFlag;
	
	[self window];

	if (self.closeFlag)
	{ [recordButton setTitle:@"Close Case"]; }
	else
	{ [recordButton setTitle:@"Log Update"]; }
	[textView setTextColor:[NSColor whiteColor]];
	
	[[self window] makeKeyAndOrderFront:self];
	
	return self;
}

- (void) dealloc
{
	[cas release];
	[super dealloc];
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	[controllerAlias setContent:nil];
}

- (IBAction) recordClicked:(id)sender
{
	if (closeFlag)
	{
		[cas closeCaseWithFinalLogEntry:[textView string] delegate:self];		
	}
	else
	{
		LCCaseLogEntry *log = [[LCCaseLogEntry alloc] initWithString:[textView string] 
																 cas:cas
														   timespent:0.0];
		[log setDelegate:self];
		[log record:cas];
	}
	[[self window] close];
}

- (void) caseClosedFinished:(LCCase *)sender
{
	[self autorelease];
}

- (void) logEntryRecordFinished:(LCCaseLogEntry *)sender
{
	[[cas logEntryList] highPriorityRefresh];
	[self autorelease];
}

- (IBAction) cancelClicked:(id)sender
{
	[[self window] close];
	[self autorelease];
}

@synthesize cas;
@synthesize closeFlag;

@end
