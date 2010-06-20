//
//  LCHelperTool.m
//  LCAdminTools
//
//  Created by James Wilson on 12/05/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LCHelperTool.h"

#import "LCHelperToolDelegate.h"

@implementation LCHelperTool

#pragma mark "Generic Methods

- (void) startHelperToolCommand:(NSString *)command arguments:(NSArray *)arguments
{
	OSStatus err = 0;
	if (!authRef)
	{ err = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef); }
	if (err == 0)
	{
		/* Setup arguments array */
		char *args[[arguments count]+2];
		args[0] = (char *) [command UTF8String];
		for (NSString *argument in arguments)
		{
			args[[arguments indexOfObject:argument]+1] = (char *) [argument cStringUsingEncoding:NSUTF8StringEncoding];
		}
		args[[arguments count]+1] = NULL;
		
		char *helperPath = (char *) [[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"HelperTool"] cStringUsingEncoding:NSUTF8StringEncoding];
				
		err = AuthorizationExecuteWithPrivileges(authRef,helperPath,kAuthorizationFlagDefaults,args,&pipe);	
		if (err == errAuthorizationSuccess)
		{
			/* Authorized and executed */
			readHandle = [[NSFileHandle alloc] initWithFileDescriptor:fileno(pipe)];
			[[NSNotificationCenter defaultCenter] addObserver:self
													 selector:@selector(dataToBeReadFromHelper:)
														 name:NSFileHandleReadCompletionNotification
													   object:readHandle];	
			[readHandle readInBackgroundAndNotify];
		}
		else
		{ 
			/* Failed to authorize or execute */
			[self informDelegteHelperFailed];
		}
		
	}
	
}

- (void) dealloc
{
	if (authRef)
	{ AuthorizationFree(authRef,kAuthorizationFlagDefaults); }
	[status release];
	[readHandle release];
	[super dealloc];
}

- (BOOL) processDataFromHelper:(NSString *)data
{
	/* Return YES to keep reading */
	/* Return NO to suppress further read */
	return YES;
}

- (void) dataToBeReadFromHelper:(NSNotification *)notification
{
	NSDictionary *resultDictionary = [notification userInfo];
	NSData *readData = [resultDictionary objectForKey:NSFileHandleNotificationDataItem];
	
	NSString *string = [[[NSString alloc] initWithData:readData encoding:NSUTF8StringEncoding] autorelease];
	if (string && [string length] > 0)
	{
		/* Interpret */
		BOOL keepReading = [self processDataFromHelper:string];
		if (keepReading)
		{ [readHandle readInBackgroundAndNotify]; }
	}
	else
	{
		/* Failed to read! */
		[self informDelegteHelperFailed];
	}
}

#pragma mark "Delegate Methods"

@synthesize delegate;

@synthesize progress;
- (void) setProgress:(float)value
{
	progress = value;
	if ([delegate respondsToSelector:@selector(helperTool:progressUpdate:)])
	{ [(LCHelperToolDelegate *)delegate helperTool:self progressUpdate:value]; }
}

@synthesize status;
- (void) setStatus:(NSString *)value
{
	[status release];
	status = [value copy];
	if ([delegate respondsToSelector:@selector(helperTool:progressUpdate:)])
	{ [(LCHelperToolDelegate *)delegate helperTool:self statusUpdate:value]; }
}

- (void) informDelegteHelperFinished
{
	if ([delegate respondsToSelector:@selector(helperToolDidFinish:)])
	{ [(LCHelperToolDelegate *)delegate helperToolDidFinish:self]; }
}

- (void) informDelegteHelperFailed
{
	if ([delegate respondsToSelector:@selector(helperToolDidFail:)])
	{ [(LCHelperToolDelegate *)delegate helperToolDidFail:self]; }	
}

@synthesize successful;

@end
