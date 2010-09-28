//
//  LCSetupController.m
//  LCAdminTools
//
//  Created by James Wilson on 31/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCSetupController.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <libpq-fe.h>
#include <SecurityFoundation/SFAuthorization.h>

int  execlp(const char *, const char *, ...);
int  setuid(uid_t);
int  setgid(gid_t);
unsigned int sleep(unsigned int seconds);
ssize_t read(int fildes, void *buf, size_t nbyte);
ssize_t write(int fildes, const void *buf, size_t nbyte);
struct hostent *gethostbyname(const char *name);
int select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict errorfds, struct timeval *restrict timeout);
int gethostname(char *name, size_t namelen);

@implementation LCSetupController

#pragma mark "UI Actions"

- (IBAction) runSetupWizard:(id)sender
{
	[self setDbUseDefault:YES];
	[nextButton setTitle:@"Next"];
	[backButton setHidden:NO];
	[backButton setEnabled:YES];
	[cancelButton setHidden:NO];
	[cancelButton setEnabled:YES];
	[tabView selectTabViewItemAtIndex:0];
	
	self.pushEnabled = YES;
	
	if (![self customerDesc] || [[self customerDesc] length] < 1)
	{ 
		
		char s[256];
		gethostname(s, 255);
		[self setCustomerDesc:[NSString stringWithCString:s encoding:NSUTF8StringEncoding]];
	}
	
	[NSApp beginSheet:sheet
	   modalForWindow:window
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (IBAction) launchConsole:(id)sender
{
	[[NSWorkspace sharedWorkspace] launchApplication:@"Lithium Console"];
}

- (IBAction) nextClicked:(id)sender
{
	int oldIndex = [tabView indexOfTabViewItem:[tabView selectedTabViewItem]];
	
	/* Validate */
	if (![self validateInputForStage:oldIndex]) return;
	
	/* Button Logic */
	if ([tabView indexOfTabViewItem:[tabView selectedTabViewItem]] < 6)
	{ 
		[tabView selectTabViewItemAtIndex:[tabView indexOfTabViewItem:[tabView selectedTabViewItem]]+1]; 
		if ([tabView indexOfTabViewItem:[tabView selectedTabViewItem]] == 6)
		{ 
			[nextButton setTitle:@"Close"]; 
			[backButton setHidden:YES];
			[cancelButton setHidden:YES];
		}
		else if ([tabView indexOfTabViewItem:[tabView selectedTabViewItem]] == 5)
		{
			[nextButton setTitle:@"Finish"];
		}
		if ([tabView indexOfTabViewItem:[tabView selectedTabViewItem]] > 0)
		{ 
			[backButton setEnabled:YES]; 
		}
	}
	
	/* Set FR */
	if (oldIndex == 0)
	{ [sheet makeFirstResponder:step1FR]; }
	else if (oldIndex == 1)
	{ [sheet makeFirstResponder:step2FR]; }
	else if (oldIndex == 2)
	{ [sheet makeFirstResponder:step3FR]; }
	else if (oldIndex == 3)
	{ [sheet makeFirstResponder:step4FR]; }
	
	/* 
	 * Actions 
	 */
	
	/* Finished Clicked */
	if (oldIndex == 5 && [tabView indexOfTabViewItem:[tabView selectedTabViewItem]] == 6)
	{
		/* Do Setup */
		[self performSetup];
	}
	
	/* Close Clicked */
	if (oldIndex == 6 && [tabView indexOfTabViewItem:[tabView selectedTabViewItem]] == 6)
	{
		/* Close */
		[NSApp endSheet:sheet];
		[sheet close];
	}
}

- (IBAction) backClicked:(id)sender
{
	/* Button Logic */
	if ([tabView indexOfTabViewItem:[tabView selectedTabViewItem]] > 0)
	{ 
		[tabView selectTabViewItemAtIndex:[tabView indexOfTabViewItem:[tabView selectedTabViewItem]]-1];
		if ([tabView indexOfTabViewItem:[tabView selectedTabViewItem]] == 0)
		{ [backButton setEnabled:NO]; }
		
		if ([tabView indexOfTabViewItem:[tabView selectedTabViewItem]] < 6)
		{ [nextButton setEnabled:YES]; }

		if ([tabView indexOfTabViewItem:[tabView selectedTabViewItem]] == 5)
		{
			[nextButton setTitle:@"Finish"];
			[backButton setHidden:NO];
			[cancelButton setHidden:NO];
		}	
		else if ([tabView indexOfTabViewItem:[tabView selectedTabViewItem]] < 5)
		{
			[nextButton setTitle:@"Next"];
		}
	}
}

- (IBAction) cancelClicked:(id)sender
{
	[NSApp endSheet:sheet];
	[sheet close];	
}

#pragma mark "Validation"

- (BOOL) validateInputForStage:(int)index
{
	if (index == 1)
	{
		/* Customer Name */
		if (![self customerName] || [[self customerName] length] < 1 || ![self customerDesc] || [[self customerDesc] length] < 1)
		{
			/* Password dont match */
			alert = [[[NSAlert alloc] init] autorelease];
			[alert addButtonWithTitle:@"Try Again"];
			[alert setMessageText:@"Customer Name and Description Required"];
			[alert setInformativeText:@"A Customer Description and Name must be specified to identify your Lithium install."];
			[alert setAlertStyle:NSWarningAlertStyle];
			[alert beginSheetModalForWindow:sheet
							  modalDelegate:self
							 didEndSelector:@selector(alertSheetEnded)
								contextInfo:nil];
			return NO;
		}
	}
	if (index == 2)
	{
		/* Admin Pass Validation */
		if (![configController adminPassword] || [[configController adminPassword] length] < 1 || 
			![configController adminUsername] || [[configController adminUsername] length] < 1)
		{
			/* No User/Password */
			alert = [[[NSAlert alloc] init] autorelease];
			[alert addButtonWithTitle:@"Try Again"];
			[alert setMessageText:@"Administrator username and password required."];
			[alert setInformativeText:@"A username and password must be specified to create the Administrator user account in Lithium"];
			[alert setAlertStyle:NSWarningAlertStyle];
			[alert beginSheetModalForWindow:sheet
							  modalDelegate:self
							 didEndSelector:@selector(alertSheetEnded)
								contextInfo:nil];
			return NO;
		}
		if (![[configController adminPassword] isEqualToString:[configController adminPasswordConfirm]])
		{
			/* Password dont match */
			alert = [[[NSAlert alloc] init] autorelease];
			[alert addButtonWithTitle:@"Try Again"];
			[alert setMessageText:@"Passwords to not match."];
			[alert setInformativeText:@"The Administrator passwords do not match. Please re-enter and try again."];
			[alert setAlertStyle:NSWarningAlertStyle];
			[alert beginSheetModalForWindow:sheet
							  modalDelegate:self
							 didEndSelector:@selector(alertSheetEnded)
								contextInfo:nil];
			return NO;
		}
	}
	if (index == 3)
	{
		/* DB Pass Validation */
		if (![configController dbPassword] || [[configController dbPassword] length] < 1 || 
			![configController dbUsername] || [[configController dbUsername] length] < 1)
		{
			/* No User/Password */
			alert = [[[NSAlert alloc] init] autorelease];
			[alert addButtonWithTitle:@"Try Again"];
			[alert setMessageText:@"Database username and password required."];
			[alert setInformativeText:@"A username and password must be specified for Lithium to connect to the PostgreSQL database"];
			[alert setAlertStyle:NSWarningAlertStyle];
			[alert beginSheetModalForWindow:sheet
							  modalDelegate:self
							 didEndSelector:@selector(alertSheetEnded)
								contextInfo:nil];
			return NO;
		}
		if (![[configController dbPassword] isEqualToString:[configController dbPasswordConfirm]])
		{
			/* Password dont match */
			alert = [[[NSAlert alloc] init] autorelease];
			[alert addButtonWithTitle:@"Try Again"];
			[alert setMessageText:@"Passwords to not match."];
			[alert setInformativeText:@"The database passwords do not match. Please re-enter and try again."];
			[alert setAlertStyle:NSWarningAlertStyle];
			[alert beginSheetModalForWindow:sheet
							  modalDelegate:self
							 didEndSelector:@selector(alertSheetEnded)
								contextInfo:nil];
			return NO;
		}		
	}
	if (index == 5)
	{
		/* Setup Readiness */
		if (![[statusController procList] postgresRunning])
		{
			/* PostgreSQL not running */
			alert = [[[NSAlert alloc] init] autorelease];
			[alert addButtonWithTitle:@"OK"];
			[alert setMessageText:@"Database is not running."];
			[alert setInformativeText:@"The Database must be running before completing the setup. Please click on the 'Start Database' button before proceeding."];
			[alert setAlertStyle:NSWarningAlertStyle];
			[alert beginSheetModalForWindow:sheet
							  modalDelegate:self
							 didEndSelector:@selector(alertSheetEnded)
								contextInfo:nil];
			return NO;
		}
	}
			
	
	return YES;
}

- (void) alertSheetEnded
{
	[NSApp endSheet:[alert window]];
	[[alert window] close];
}


#pragma mark "Perform Setup"

- (void) performSetup
{
	/*
	 * Use helper tool to:
	 *
	 * 1) Write global config to file 
	 *
	 * 2) Create webroot profile.php files
	 *
	 */
	
	/* Write Config */
	NSString *config = [configController writeConfig];
	OSStatus err;
	AuthorizationRef authRef;
	err = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
	if (err == 0)
	{
		FILE *pipe;
		char *args[] = { "writeconfig", (char *) [customerName cStringUsingEncoding:NSUTF8StringEncoding], (char *) [customerDesc cStringUsingEncoding:NSUTF8StringEncoding], NULL};
		char *myPath = (char *) [[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"HelperTool"] cStringUsingEncoding:NSUTF8StringEncoding];
		
		err = AuthorizationExecuteWithPrivileges(authRef,myPath,kAuthorizationFlagDefaults,args,&pipe);	
		if (err == errAuthorizationSuccess)
		{
			[self setStatus:@"Setup in progress..."];
			[self setStatusIcon:nil];
			[self setSetupInProgress:YES];
			[nextButton setEnabled:NO];
			
			struct fd_set fdset;
			FD_ZERO (&fdset);
			FD_SET (fileno(pipe), &fdset);
			struct timeval timeout;
			timeout.tv_sec = 10;
			timeout.tv_usec = 0;
			int num = select (fileno(pipe)+1, &fdset, NULL, NULL, &timeout);
			if (num > 0)
			{
				char readBuffer[128];
				int bytesRead = read (fileno (pipe), readBuffer, sizeof (readBuffer) - 1);
				if (bytesRead < 1)
				{
					/* Error reading response */
					[self setConfigStatus:@"Failed."];
					[self setStatus:@"Error occurred while writing configuration files."];
					[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
					[self setSetupInProgress:NO];
					[backButton setHidden:NO];
					[cancelButton setHidden:NO];		
					return;
				}
				else
				{ 
					readBuffer[bytesRead] = '\0';
					if (strcmp(readBuffer, "WRITECONFIG: OK") != 0)
					{
						/* Error reading response */
						[self setConfigStatus:@"Failed."];
						[self setStatus:@"Failed to create database."];
						[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
						[self setSetupInProgress:NO];
						[backButton setHidden:NO];
						[cancelButton setHidden:NO];		
						return;
					}
				}
			}
			else
			{ 
				/* Timed out waiting for a response */
				[self setConfigStatus:@"Failed."];
				[self setStatus:@"Timeout error occurred while writing configuration files."];
				[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
				[self setSetupInProgress:NO];
				[backButton setHidden:NO];
				[cancelButton setHidden:NO];				
				return;
			} 
			
			num = write (fileno(pipe), [config cStringUsingEncoding:NSUTF8StringEncoding], [config lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
			if (num < [config lengthOfBytesUsingEncoding:NSUTF8StringEncoding])
			{
				/* Failed to write */
				[self setConfigStatus:@"Failed."];
				[self setStatus:@"Failed to write configuration files."];
				[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
				[self setSetupInProgress:NO];
				[backButton setHidden:NO];
				[cancelButton setHidden:NO];
			}

		}
		else
		{
			/* Did not Auth */
			[self backClicked:self];
			return;
		}
		
		err = AuthorizationFree(authRef,kAuthorizationFlagDefaults);
	}	
	else 
	{ 
		[self setConfigStatus:@"Failed."];
		[self setStatus:@"Failed to get authorisation."];
		[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
		[self setSetupInProgress:NO];
		[backButton setHidden:NO];
		[cancelButton setHidden:NO];
		return; 
	}
	
	[self setConfigStatus:@"Done."];
	
	/*
	 * Connect to PostgreSQL to write customer record to db
	 */	

	/* Connect */
	PGconn *conn;
	conn = PQsetdbLogin([[configController dbHostname] cStringUsingEncoding:NSUTF8StringEncoding], [[configController dbPort] cStringUsingEncoding:NSUTF8StringEncoding], NULL, NULL, "lithium", [[configController dbUsername] cStringUsingEncoding:NSUTF8StringEncoding], [[configController dbPassword] cStringUsingEncoding:NSUTF8StringEncoding]);
	if (PQstatus(conn) == CONNECTION_BAD)
	{
		[self setDbStatus:@"Failed."];
		[self setStatus:@"Failed to connect to database."];
		[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
		[self setSetupInProgress:NO];
		[backButton setHidden:NO];
		[cancelButton setHidden:NO];
		return;
	}
	
	/* Clear existing */
	PGresult *res = PQexec (conn, "CREATE TABLE customers (name varchar, descr varchar, baseurl varchar)");
	res = PQexec (conn, "DELETE FROM customers");
	
	/* Add new */
	NSString *query = [NSString stringWithFormat:@"INSERT INTO customers (name, descr) VALUES ('%@', '%@')", customerName, customerDesc];
	res = PQexec (conn, [query cStringUsingEncoding:NSUTF8StringEncoding]);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		[self setDbStatus:@"Failed."];
		[self setStatus:@"Failed to add customer record to database."];
		[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
		[self setSetupInProgress:NO];
		[backButton setHidden:NO];
		[cancelButton setHidden:NO];
		return;
	}
	
	/* Create database */
	query = [NSString stringWithFormat:@"CREATE DATABASE customer_%@", customerName];
	res = PQexec (conn, [query UTF8String]);
	
	/* Switch to customer DB */
	PQfinish (conn);
	conn = PQsetdbLogin([[configController dbHostname] UTF8String],
						[[configController dbPort] UTF8String], 
						NULL, NULL, [[NSString stringWithFormat:@"customer_%@", customerName] UTF8String], 
						[[configController dbUsername] UTF8String], 
						[[configController dbPassword] UTF8String]);
	if (PQstatus(conn) == CONNECTION_BAD)
	{
		[self setDbStatus:@"Failed."];
		[self setStatus:@"Failed to connect to customer database."];
		[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
		[self setSetupInProgress:NO];
		[backButton setHidden:NO];
		[cancelButton setHidden:NO];
		return;
	}
	
	/* Ensure action tables are present */
	res = PQexec (conn, "CREATE TABLE actions (id serial, descr varchar, enabled integer, activation integer, delay integer, rerun integer, rerundelay integer, timefilter integer, daymask integer, starthour integer, endhour integer, script varchar)");
	res = PQexec (conn, "CREATE TABLE action_configvars (id serial, action integer, name varchar, value varchar)");
	
	/* Create actions */
	if ([emailTo length] > 0 && [emailServer length] > 0)
	{
		/* Check sender */
		if (!emailFrom || [emailFrom length] < 1)
		{ self.emailFrom = @"lithium"; }
		
		/* Add Email Action */
		res = PQexec (conn, "DELETE FROM actions WHERE descr='Default Email Notification Action'");
		res = PQexec (conn, "INSERT INTO actions (descr, enabled, activation, delay, rerun, rerundelay, timefilter, daymask, starthour, endhour, script) VALUES ('Default Email Notification Action', '1', '1', '0', '0', '0', '0', '127', '0', '0', 'email_alert.pl')");
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			[self setDbStatus:@"Failed."];
			[self setStatus:@"Failed to add email alert record to database."];
			[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
			[self setSetupInProgress:NO];
			[backButton setHidden:NO];
			[cancelButton setHidden:NO];
			return;
		}	
		
		/* Get action ID */
		res = PQexec (conn, "SELECT lastval()");
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) < 1)
		{
			[self setDbStatus:@"Failed."];
			[self setStatus:@"Failed to add get ID for email alert action."];
			[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
			[self setSetupInProgress:NO];
			[backButton setHidden:NO];
			[cancelButton setHidden:NO];
			return;
		}
		char *curid_str = PQgetvalue (res, 0, 0);
		int action_id = atoi (curid_str);

		/* Add Config variables */
		query = [NSString stringWithFormat:@"INSERT INTO action_configvars (action, name, value) VALUES ('%i', 'sender', '%s')",
				 action_id, [emailFrom UTF8String]];
		res = PQexec (conn, [query UTF8String]);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			[self setDbStatus:@"Failed."];
			[self setStatus:@"Failed to add email alert sender configuration record to database."];
			[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
			[self setSetupInProgress:NO];
			[backButton setHidden:NO];
			[cancelButton setHidden:NO];
			return;
		}	
		query = [NSString stringWithFormat:@"INSERT INTO action_configvars (action, name, value) VALUES ('%i', 'mailhost', '%s')",
				 action_id, [emailServer UTF8String]];
		res = PQexec (conn, [query UTF8String]);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			[self setDbStatus:@"Failed."];
			[self setStatus:@"Failed to add email alert server configuration record to database."];
			[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
			[self setSetupInProgress:NO];
			[backButton setHidden:NO];
			[cancelButton setHidden:NO];
			return;
		}	
		query = [NSString stringWithFormat:@"INSERT INTO action_configvars (action, name, value) VALUES ('%i', 'recipients', '%s')",
				 action_id, [emailTo UTF8String]];
		res = PQexec (conn, [query UTF8String]);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			[self setDbStatus:@"Failed."];
			[self setStatus:@"Failed to add email alert recipients configuration record to database."];
			[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
			[self setSetupInProgress:NO];
			[backButton setHidden:NO];
			[cancelButton setHidden:NO];
			return;
		}	
		if (self.smtpUsername)
		{
			query = [NSString stringWithFormat:@"INSERT INTO action_configvars (action, name, value) VALUES ('%i', 'smtp_username', '%s')",
					 action_id, [smtpUsername UTF8String]];
			res = PQexec (conn, [query UTF8String]);
			if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
			{
				[self setDbStatus:@"Failed."];
				[self setStatus:@"Failed to add email alert smtp username configuration record to database."];
				[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
				[self setSetupInProgress:NO];
				[backButton setHidden:NO];
				[cancelButton setHidden:NO];
				return;
			}	
		}
		if (self.smtpPassword)
		{
			query = [NSString stringWithFormat:@"INSERT INTO action_configvars (action, name, value) VALUES ('%i', 'smtp_password', '%s')",
					 action_id, [smtpPassword UTF8String]];
			res = PQexec (conn, [query UTF8String]);
			if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
			{
				[self setDbStatus:@"Failed."];
				[self setStatus:@"Failed to add email alert smtp password configuration record to database."];
				[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
				[self setSetupInProgress:NO];
				[backButton setHidden:NO];
				[cancelButton setHidden:NO];
				return;
			}	
		}
	}
	if (pushEnabled)
	{
		/* Add push notification */
		res = PQexec (conn, "DELETE FROM actions WHERE descr='Default iPhone Push Notification Action'");
		res = PQexec (conn, "INSERT INTO actions (descr, enabled, activation, delay, rerun, rerundelay, timefilter, daymask, starthour, endhour, script) VALUES ('Default iPhone Push Notification Action', '1', '1', '0', '0', '0', '0', '127', '0', '0', 'push_alert.pl')");
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			[self setDbStatus:@"Failed."];
			[self setStatus:@"Failed to add email alert record to database."];
			[self setStatusIcon:[NSImage imageNamed:@"stop_32.tif"]];
			[self setSetupInProgress:NO];
			[backButton setHidden:NO];
			[cancelButton setHidden:NO];
			return;
		}
		
	}
	PQfinish (conn);
	conn = nil;
	
	/*
	 * Finished! 
	 */

	[NSTimer scheduledTimerWithTimeInterval:3.0 target:self selector:@selector(setupFinishedTimer) userInfo:nil repeats:NO];
}
	

- (void) setupFinishedTimer
{	
	[self setDbStatus:@"Done."];
	[self setStatus:@"Setup completed successfully."];
	[self setStatusIcon:[NSImage imageNamed:@"ok_32.tif"]];
	[self setSetupInProgress:NO];
	[nextButton setEnabled:YES];
}

- (void) closeTimer
{
	[NSApp endSheet:sheet];
	[sheet close];
}

#pragma mark "Properties"

- (BOOL) setupInProgress
{ return setupInProgress; }

- (void) setSetupInProgress:(BOOL)flag
{ setupInProgress = flag; }

- (NSString *) customerName
{ return customerName; }

- (void) setCustomerName:(NSString *)string
{ 
	[customerName release];
	customerName = [string retain];
}

- (NSString *) customerDesc
{ return customerDesc; }

- (void) setCustomerDesc:(NSString *)string
{ 
	/* Set desc */
	[customerDesc release];
	customerDesc = [string retain];
	
	/* Update name */
	if (customerDesc && [customerDesc length] > 0)
	{
		char *name_str = malloc ([customerDesc length]);
		char *desc_str = (char *) [customerDesc cStringUsingEncoding:NSUTF8StringEncoding];
		int i;
		int name_len = 0;
		for (i=0; i < strlen (desc_str); i++)
		{
			if (isalnum(desc_str[i]))
			{ 
				name_str[name_len] = tolower(desc_str[i]);
				name_len++;
				if (name_len == 12) break; 
			}
		}
		name_str[name_len] = '\0';
		[self setCustomerName:[NSString stringWithCString:name_str encoding:NSUTF8StringEncoding]];
	}
	else
	{ [self setCustomerName:@""]; }
}

- (BOOL) editName
{ return editName; }

- (void) setEditName:(BOOL)flag
{ editName = flag; }

- (NSString *) status
{ return status; }

- (void) setStatus:(NSString *)string
{
	[status release];
	status = [string retain];
}

- (NSImage *) statusIcon
{ return statusIcon; }

- (void) setStatusIcon:(NSImage *)image
{
	[statusIcon release];
	statusIcon = [image retain];
}

- (NSString *) configStatus
{ return configStatus; }

- (void) setConfigStatus:(NSString *)string
{
	[configStatus release];
	configStatus = [string retain];
}

- (NSString *) dbStatus
{ return dbStatus; }

- (void) setDbStatus:(NSString *)string
{
	[dbStatus release];
	dbStatus = [string retain];
}

- (BOOL) dbUseDefault
{ return dbUseDefault; }

- (void) setDbUseDefault:(BOOL)flag
{ dbUseDefault = flag; }

@synthesize emailFrom;
@synthesize emailTo;
@synthesize emailServer;
@synthesize pushEnabled;
@synthesize smtpUsername, smtpPassword;

@end
