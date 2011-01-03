//
//  LCConfigController.m
//  LCAdminTools
//
//  Created by James Wilson on 31/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCConfigController.h"
#include <SecurityFoundation/SFAuthorization.h>

int  execlp(const char *, const char *, ...);
int  setuid(uid_t);
int  setgid(gid_t);
unsigned int sleep(unsigned int seconds);
ssize_t read(int fildes, void *buf, size_t nbyte);
ssize_t write(int fildes, const void *buf, size_t nbyte);
struct hostent *gethostbyname(const char *name);
int gethostname(char *name, size_t namelen);

static LCConfigController *masterController;

@implementation LCConfigController

#pragma mark "Constructor"

+ (LCConfigController *) masterController
{ return masterController; }

- (void) awakeFromNib
{
	masterController = self;
	
	if ([[NSString stringWithContentsOfFile:@"/Library/Preferences/Lithium/lithium/node.conf"
								   encoding:NSUTF8StringEncoding
									  error:nil] length] > 10)
	{
		[self readConfig];
	}
	else
	{
		[self setAdminUsername:@"admin"];
		[self setDbUsername:@"lithium"];
		[self setDbPassword:@"lithium"];
		[self setDbPasswordConfirm:@"lithium"];
		[self setDbHostname:@"localhost"];
		[self setDbPort:@"51132"];
		[self setHttpRoot:@"/Library/Application Support/Lithium/ClientService/Resources/htdocs"];
		[self setAuthExternal:NO];
	}
}

- (void) dealloc
{
	[adminUsername release];
	[adminPassword release];
	[dbUsername release];
	[dbPassword release];
	[dbHostname release];
	[dbPort release];
	[httpRoot release];
	[super dealloc];
}		

#pragma mark "UI Actions"

- (IBAction) saveConfigClicked:(id)sender
{
	/* Check Admin Password */
	if (![adminPassword isEqualToString:adminPasswordConfirm])
	{
		alert = [[[NSAlert alloc] init] autorelease];
		[alert addButtonWithTitle:@"Try Again"];
		[alert setMessageText:@"Passwords to not match."];
		[alert setInformativeText:@"The Administrator passwords do not match. Please re-enter and try again."];
		[alert setAlertStyle:NSWarningAlertStyle];
		[alert beginSheetModalForWindow:window
						  modalDelegate:self
						 didEndSelector:@selector(sheetEnded)
							contextInfo:nil];
		return;
	}

	/* Check Database Password */
	if (![dbPassword isEqualToString:dbPasswordConfirm])
	{
		alert = [[[NSAlert alloc] init] autorelease];
		[alert addButtonWithTitle:@"Try Again"];
		[alert setMessageText:@"Passwords to not match."];
		[alert setInformativeText:@"The Database user passwords do not match. Please re-enter and try again."];
		[alert setAlertStyle:NSWarningAlertStyle];
		[alert beginSheetModalForWindow:window
						  modalDelegate:self
						 didEndSelector:@selector(sheetEnded)
							contextInfo:nil];
		return;
	}	
	
	/* Write Config */
	NSString *config = [self writeConfig];
	OSStatus err;
	AuthorizationRef authRef;
	err = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
	if (err == 0)
	{
		FILE *pipe;
		char *args[] = { "writeconfig", NULL};
		char *myPath = (char *) [[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"HelperTool"] cStringUsingEncoding:NSUTF8StringEncoding];
		
		err = AuthorizationExecuteWithPrivileges(authRef,myPath,kAuthorizationFlagDefaults,args,&pipe);	
		if (err == errAuthorizationSuccess)
		{
			int num = write (fileno(pipe), [config cStringUsingEncoding:NSUTF8StringEncoding], [config lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
			if (num < [config lengthOfBytesUsingEncoding:NSUTF8StringEncoding])
			{
				/* Failed to write */
				return;
			}
		}
		else
		{ return; }
		
		err = AuthorizationFree(authRef,kAuthorizationFlagDefaults);
	}	
	else { return; }
}

- (void) sheetEnded
{
	[NSApp endSheet:[alert window]];
	[[alert window] close];
}

#pragma mark "Config Read/Write"

- (void) readConfig
{
	/* Interpret config */
	int i=1;
	NSString *config = [NSString stringWithContentsOfFile:@"/Library/Preferences/Lithium/lithium/node.conf"
												 encoding:NSUTF8StringEncoding
													error:nil];
	NSArray *lines = [config componentsSeparatedByString:@"\n"];
	NSEnumerator *lineEnum = [lines objectEnumerator];
	NSString *line;
	while (line=[lineEnum nextObject])
	{
		/* Get components -- value is in index 1 */
		NSArray *components = [line componentsSeparatedByString:@"\""];
		if ([components count] < 2) { i++;  continue; }
		NSString *value = [components objectAtIndex:1];
		
		/* Interpret line */
		switch (i)
		{
			case 11:
				[self setAdminUsername:value];
				break;
			case 12:
				[self setAdminPassword:value];
				[self setAdminPasswordConfirm:value];
				break;
			case 16:
				[self setDbUsername:value];
				break;
			case 17:
				[self setDbPassword:value];
				[self setDbPasswordConfirm:value];
				break;
			case 18:
				[self setDbHostname:value];
				break;
			case 19:
				[self setDbPort:value];
				break;
			case 27:
				[self setHttpRoot:value];
				break;
			case 31:
				if ([value intValue] == 0) [self setAuthExternal:NO];
				else [self setAuthExternal:YES];
				break;
			case 35:
				if ([value intValue] == 1) [self setSqlMetricRecording:YES];
				else [self setSqlMetricRecording:NO];
				break;
			case 39:
				if ([value intValue] == 1) [self setDeepSearch:YES];
				else [self setDeepSearch:NO];
				break;
		}
		
		/* Increment */
		i++;
	}
}

- (NSString *) writeConfig
{
	/* Get Hostname */
	char hostname[256];
	gethostname(hostname, 256);
	NSArray *hostArray = [[NSString stringWithCString:hostname encoding:NSUTF8StringEncoding] componentsSeparatedByString:@"."];
	
	/* ID Section */
	NSMutableString *config;	
	config = [NSMutableString string];
	[config appendFormat:@"<section id>\n  plexus \"%@\"\n  node \"%@\"\n</id>\n\n", [hostArray objectAtIndex:0], [hostArray objectAtIndex:0]];
	
	/* Debug */
	[config appendFormat:@"<section debug>\n  level \"1\"\n</debug>\n\n"];
	
	/* Master User */
	[config appendFormat:@"<section master_user>\n"];
	[config appendFormat:@"  username \"%@\"\n", adminUsername];
	[config appendFormat:@"  password \"%@\"\n", adminPassword];
	[config appendFormat:@"</master_user>\n\n"];
	
	/* Postgres */
	[config appendFormat:@"<section postgresql>\n"];
	[config appendFormat:@"  username \"%@\"\n", dbUsername];
	[config appendFormat:@"  password \"%@\"\n", dbPassword];
	[config appendFormat:@"  host \"%@\"\n", dbHostname];
	[config appendFormat:@"  port \"%@\"\n", dbPort];
	[config appendFormat:@"</postgresql>\n\n"];
	
	/* Fonts */
	[config appendFormat:@"<section fonts>\n"];
	[config appendFormat:@"  rrdfont \"none.ttf\"\n"];
	[config appendFormat:@"</fonts>\n\n"];
	
	/* HTTPD */
	[config appendFormat:@"<section httpd>\n"];
	[config appendFormat:@"  imageroot \"%@\"\n", httpRoot];
	[config appendFormat:@"</httpd>\n\n"];
	
	/* Auth */
	[config appendFormat:@"<section authentication>\n"];
	if (authExternal) [config appendFormat:@"  external \"1\"\n"];
	else [config appendFormat:@"  external \"0\"\n"];
	[config appendFormat:@"</authentication>\n\n"];

	/* SQL Recording */
	[config appendFormat:@"<section recording>\n"];
	if (sqlMetricRecording) [config appendFormat:@"  sql \"1\"\n"];
	else [config appendFormat:@"  sql \"0\"\n"];
	[config appendFormat:@"</recording>\n\n"];	

	/* Deep Search */
	[config appendFormat:@"<section search>\n"];
	if (deepSearch) [config appendFormat:@"  deep \"1\"\n"];
	else [config appendFormat:@"  deep \"0\"\n"];
	[config appendFormat:@"</search>\n\n"];	
	
	
	/* Write */
	return config;
}

#pragma mark "Accessors"

- (NSString *) adminUsername
{ return adminUsername; }

- (void) setAdminUsername:(NSString *)string
{
	[adminUsername release];
	adminUsername = [string retain];
}

- (NSString *) adminPassword
{ return adminPassword; }

- (void) setAdminPassword:(NSString *)string
{
	[adminPassword release];
	adminPassword = [string retain];
}

- (NSString *) adminPasswordConfirm
{ return adminPasswordConfirm; }

- (void) setAdminPasswordConfirm:(NSString *)string
{
	[adminPasswordConfirm release];
	adminPasswordConfirm = [string retain];
}

- (NSString *) dbUsername
{ return dbUsername; }

- (void) setDbUsername:(NSString *)string
{
	[dbUsername release];
	dbUsername = [string retain];
}

- (NSString *) dbPassword
{ return dbPassword; }

- (void) setDbPassword:(NSString *)string
{
	[dbPassword release];
	dbPassword = [string retain];
}

- (NSString *) dbPasswordConfirm
{ return dbPasswordConfirm; }

- (void) setDbPasswordConfirm:(NSString *)string
{
	[dbPasswordConfirm release];
	dbPasswordConfirm = [string retain];
}

- (NSString *) dbHostname
{ return dbHostname; }

- (void) setDbHostname:(NSString *)string
{
	[dbHostname release];
	dbHostname = [string retain];
}

- (NSString *) dbPort
{ return dbPort; }

- (void) setDbPort:(NSString *)string
{
	[dbPort release];
	dbPort = [string retain];
}

- (NSString *) httpRoot
{ return httpRoot; }

- (void) setHttpRoot:(NSString *)string
{
	[httpRoot release];
	httpRoot = [string retain];
}

- (BOOL) authExternal
{ return authExternal; }

- (void) setAuthExternal:(BOOL)flag
{ authExternal = flag; }

- (BOOL) sqlMetricRecording
{
	return sqlMetricRecording;
}
- (void) setSqlMetricRecording:(BOOL)flag
{
	sqlMetricRecording = flag;
}

- (BOOL) deepSearch
{
	return deepSearch;
}
- (void) setDeepSearch:(BOOL)flag
{
	deepSearch = flag;
}


@end
