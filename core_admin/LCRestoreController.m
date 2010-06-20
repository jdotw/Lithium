//
//  LCRestoreController.m
//  LCAdminTools
//
//  Created by James Wilson on 31/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCRestoreController.h"

#include <sys/select.h>

int  execlp(const char *, const char *, ...);
int  setuid(uid_t);
int  setgid(gid_t);
unsigned int sleep(unsigned int seconds);
ssize_t read(int fildes, void *buf, size_t nbyte);
ssize_t write(int fildes, const void *buf, size_t nbyte);

@implementation LCRestoreController

#pragma mark "Constructor"

- (void) awakeFromNib
{
	
}

#pragma mark "UI Actions"

- (IBAction) chooseFileClicked:(id)sender
{
	openPanel = [[NSOpenPanel openPanel] retain];
	[openPanel setTitle:@"Choose backup file to restore"];
	[openPanel setMessage:@"Choose backup file to restore"];
	[openPanel setPrompt:@"Restore"];
	[openPanel beginSheetForDirectory:[@"~/Desktop" stringByExpandingTildeInPath]
								 file:nil
					   modalForWindow:window
						modalDelegate:self
					   didEndSelector:@selector(restoreClicked:returnCode:contextInfo:)
						  contextInfo:nil];
}

- (IBAction) restoreClicked:(NSSavePanel *)sheet returnCode:(int)returnCode  contextInfo:(void  *)contextInfo
{
	if (returnCode != NSOKButton)
	{ return; }
	
	/* Set flag */
	[self setRestoreInProgress:YES];
	[self setStatus:@"Restore in progress..."];
	[self setRestoreFile:[openPanel filename]];
	
	/* Close Sheet */
	[NSApp endSheet:openPanel];
	[openPanel close];

	/* Get UUID */
	CFUUIDRef     myUUID;
	CFStringRef   myUUIDString;
	char          strBuffer[100];
	myUUID = CFUUIDCreate(kCFAllocatorDefault);
	myUUIDString = CFUUIDCreateString(kCFAllocatorDefault, myUUID);
	CFStringGetCString(myUUIDString, strBuffer, 100, kCFStringEncodingASCII);
	NSString *uuidString = [NSString stringWithFormat:@"%s", strBuffer];	
	
	/* Perform restore */
	OSStatus err;
	err = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
	if (err == 0)
	{
		char *args[] = { "restore", (char *) [uuidString cStringUsingEncoding:NSUTF8StringEncoding], (char *) [[openPanel filename] cStringUsingEncoding:NSUTF8StringEncoding], NULL};
		char *myPath = (char *) [[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"HelperTool"] cStringUsingEncoding:NSUTF8StringEncoding];
		
		err = AuthorizationExecuteWithPrivileges(authRef,myPath,kAuthorizationFlagDefaults,args,&pipe);	
		if (err == errAuthorizationSuccess)
		{
			pollingTimer = [[NSTimer scheduledTimerWithTimeInterval:0.5
															 target:self
														   selector:@selector(pollPipe)
														   userInfo:nil
															repeats:YES] retain];
		}
		else
		{ NSLog (@"Auth failed"); }
		
		err = AuthorizationFree(authRef,kAuthorizationFlagDefaults);
	}
	
}

- (IBAction) cancelClicked:(id)sender
{
	
}

#pragma mark "PIPE Polling"

- (void) pollPipe
{
	struct fd_set fdset;
	struct fd_set exfdset;
	
	/* Check EOF */
	FD_ZERO (&fdset);
	FD_SET (fileno(pipe), &fdset);
	FD_ZERO (&exfdset);
	FD_SET (fileno(pipe), &exfdset);
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	int num = select (fileno(pipe)+1, &fdset, NULL, NULL, &timeout);
	if (num > 0)
	{
		char readBuffer[128];
		int bytesRead = read (fileno (pipe), readBuffer, sizeof (readBuffer) - 1);
		if (bytesRead < 0)
		{
			/* ERROR : Pipe Closed */
			pipe = nil;
			[pollingTimer invalidate];
			pollingTimer = nil;
			if ([self restoreInProgress])
			{
				[self setStatus:@"ERROR: Failed to communicate with restore process"];
				[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
				[self setRestoreInProgress:NO];
			}
			return;
		}
		readBuffer[bytesRead] = '\0';
		NSString *output = [NSString stringWithCString:readBuffer encoding:NSUTF8StringEncoding];
		
		/* Get lines */
		NSArray *lines = [output componentsSeparatedByString:@"\n"];
		NSEnumerator *lineEnum = [lines objectEnumerator];
		NSString *line;
		while (line=[lineEnum nextObject])
		{		
			NSLog (@"line is %@", line);
			char *line_str = (char *) [line cStringUsingEncoding:NSUTF8StringEncoding];
			if (strstr(line_str, "STOP"))
			{
				/* DB Feedback */
				if (strstr(line_str, "OK"))
				{ 
					[self setStopStatus:@"Done."];
					[self setArchiveStatus:@"In Progress.."];
				}
				else
				{ 
					[self setStopStatus:@"FAILED!"];
					[self setStatus:@"ERROR: Restore failed!"];
					[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
				}
			}			
			if (strstr(line_str, "ARCHIVE"))
			{
				/* DB Feedback */
				if (strstr(line_str, "OK"))
				{ 
					[self setArchiveStatus:@"Done."];
					[self setDbStatus:@"In Progress..."];
				}
				else
				{ 
					[self setStatus:@"ERROR: Restore failed!"];					
					[self setArchiveStatus:@"FAILED!"];
					[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
				}
			}
			if (strstr(line_str, "DB"))
			{
				/* DB Feedback */
				if (strstr(line_str, "OK"))
				{ 
					[self setDbStatus:@"Done."];
					[self setDataStatus:@"In Progress..."];
				}
				else
				{ 
					[self setDbStatus:@"FAILED!"];
					[self setStatus:@"ERROR: Restore failed!"];					
					[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
				}
			}
			if (strstr(line_str, "DATA"))
			{
				/* DB Feedback */
				if (strstr(line_str, "OK"))
				{ 
					[self setDataStatus:@"Done."];
					[self setWebStatus:@"In Progress..."];
				}
				else
				{ 
					[self setDataStatus:@"FAILED!"];
					[self setStatus:@"ERROR: Restore failed!"];					
					[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
				}
			}
			if (strstr(line_str, "WEB"))
			{
				/* DB Feedback */
				if (strstr(line_str, "OK"))
				{
					[self setWebStatus:@"Done."];
				}
				else
				{ 
					[self setStatus:@"ERROR: Restore failed!"];					
					[self setWebStatus:@"FAILED!"];
					[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
				}
			}				
			if (strstr(line_str, "RESTART"))
			{
				/* DB Feedback */
				if (strstr(line_str, "OK"))
				{ 
					[self setStatus:@"Restore completed successfully."];
					[self setStatusIcon:[NSImage imageNamed:@"ok_32.tif"]];
					[self setRestartStatus:@"Done."];
					[self setRestoreInProgress:NO];
					[pollingTimer invalidate];
					[pollingTimer release];
					pollingTimer = nil;
				}
				else
				{ 
					[self setStopStatus:@"FAILED!"];
				}
			}			
			
		}
	}
}


#pragma mark "Accessors"

- (NSString *) stopStatus
{ return stopStatus; }

- (void) setStopStatus:(NSString *)string
{ 
	[stopStatus release];
	stopStatus = [string retain];
}

- (NSString *) restartStatus
{ return restartStatus; }

- (void) setRestartStatus:(NSString *)string
{ 
	[restartStatus release];
	restartStatus = [string retain];
}

- (NSString *) dbStatus
{ return dbStatus; }

- (void) setDbStatus:(NSString *)string
{
	[dbStatus release];
	dbStatus = [string retain];
}

- (NSString *) webStatus
{ return webStatus; }

- (void) setWebStatus:(NSString *)string
{
	[webStatus release];
	webStatus = [string retain];
}

- (NSString *) dataStatus
{ return dataStatus; }

- (void) setDataStatus:(NSString *)string
{
	[dataStatus release];
	dataStatus = [string retain];
}

- (NSString *) archiveStatus
{ return archiveStatus; }

- (void) setArchiveStatus:(NSString *)string
{
	[archiveStatus release];
	archiveStatus = [string retain];
}

- (BOOL) restoreInProgress
{ return restoreInProgress; }

- (void) setRestoreInProgress:(BOOL)flag
{ restoreInProgress = flag; }

- (NSString *) status
{ return status; }

- (void) setStatus:(NSString *)string
{
	[status release];
	status = [string retain];
}

- (NSImage *) statusIcon
{ return statusIcon; }

- (void) setStatusIcon:(NSImage *)icon
{
	[statusIcon release];
	statusIcon = [icon retain];
}

- (NSString *) restoreFile
{ return restoreFile; }

- (void) setRestoreFile:(NSString *)string
{
	[restoreFile release];
	restoreFile = [string retain];
}


@end
