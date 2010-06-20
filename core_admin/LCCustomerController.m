//
//  LCCustomerController.m
//  LCAdminTools
//
//  Created by James Wilson on 22/09/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCCustomerController.h"
#import "LCCustomer.h"
#import "LCName.h"

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

@implementation LCCustomerController

#pragma mark "Constructors"

- (void) awakeFromNib
{
	customers = [[NSMutableArray array] retain];
	customerDict = [[NSMutableDictionary dictionary] retain];
	
	NSSortDescriptor *sortDesc = [[[NSSortDescriptor alloc] initWithKey:@"name" ascending:YES] autorelease];
	self.customerSortDescriptors = [NSArray arrayWithObject:sortDesc];
}

#pragma mark "Add New Customer"

- (IBAction) addNewCustomerClicked:(id)sender
{
	self.addCustomerDesc = nil;
	self.addCustomerName = nil;
	self.addCustomerSetNameAutomatically = YES;
	self.addCustomerURL = nil;
	self.addCustomerMailServer = nil;
	self.addCustomerMailFrom = nil;
	self.addCustomerMailTo = nil;
	self.addCustomerEnablePush = YES;
	
	[NSApp beginSheet:addCustomerSheet
	   modalForWindow:window
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (void) reportError:(NSString *)informativeText
{
	NSAlert *alert = [NSAlert alertWithMessageText:@"Failed to add New Customer"
									 defaultButton:@"OK"
								   alternateButton:nil
									   otherButton:nil
						 informativeTextWithFormat:informativeText];
	[alert setAlertStyle:NSCriticalAlertStyle];
	[alert beginSheetModalForWindow:window
					  modalDelegate:self
					 didEndSelector:nil
						contextInfo:nil];
}

- (IBAction) addCustomerAddClicked:(id)sender
{
	/* 
	 * Create paths and links
	 */
	
	/* Write Config */
	NSString *config = [configController writeConfig];
	OSStatus err;
	AuthorizationRef authRef;
	err = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
	if (err == 0)
	{
		FILE *pipe;
		char *args[] = { "writeconfig", (char *) [addCustomerName cStringUsingEncoding:NSUTF8StringEncoding], (char *) [addCustomerDesc cStringUsingEncoding:NSUTF8StringEncoding], NULL};
		char *myPath = (char *) [[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"HelperTool"] cStringUsingEncoding:NSUTF8StringEncoding];
		
		err = AuthorizationExecuteWithPrivileges(authRef,myPath,kAuthorizationFlagDefaults,args,&pipe);	
		if (err == errAuthorizationSuccess)
		{
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
					[self reportError:@"Error occurred while writing configuration files."];
					return;
				}
				else
				{ 
					readBuffer[bytesRead] = '\0';
					if (strcmp(readBuffer, "WRITECONFIG: OK") != 0)
					{
						/* Error reading response */
						[self reportError:@"Error occurred while writing configuration files."];
						return;
					}
				}
			}
			else
			{ 
				/* Timed out waiting for a response */
				[self reportError:@"Timeout error occurred while writing configuration files."];
				return;
			} 
			
			num = write (fileno(pipe), [config cStringUsingEncoding:NSUTF8StringEncoding], [config lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
			if (num < [config lengthOfBytesUsingEncoding:NSUTF8StringEncoding])
			{
				/* Failed to write */
				[self reportError:@"Failed to write configuration files."];
				return;
			}
			
		}
		else
		{
			/* Did not Auth */
			return;
		}
		
		err = AuthorizationFree(authRef,kAuthorizationFlagDefaults);
	}	
	else 
	{ 
		[self reportError:@"Failed to get authorisation."];
		return; 
	}
	
	/* 
	 * Add to SQL 
	 */
	
	/* Connect to SQL */
	PGconn *conn;
	conn = PQsetdbLogin([[configController dbHostname] cStringUsingEncoding:NSUTF8StringEncoding], [[configController dbPort] cStringUsingEncoding:NSUTF8StringEncoding], NULL, NULL, "lithium", [[configController dbUsername] cStringUsingEncoding:NSUTF8StringEncoding], [[configController dbPassword] cStringUsingEncoding:NSUTF8StringEncoding]);
	if (PQstatus(conn) == CONNECTION_BAD)
	{
		[self reportError:@"Connection to Lithium Core SQL database failed."];
		return;
	}
	
	/* Ensure customer table exists */
	PGresult *res = PQexec (conn, "CREATE TABLE customers (name varchar, descr varchar, baseurl varchar)");
	
	/* Add new */
	NSString *query = [NSString stringWithFormat:@"INSERT INTO customers (name, descr) VALUES ('%@', '%@')", addCustomerName, addCustomerDesc];
	res = PQexec (conn, [query cStringUsingEncoding:NSUTF8StringEncoding]);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		[self reportError:@"Failed to add customer record to SQL database."];
		return;
	}
	
	/* Create customer database */
	query = [NSString stringWithFormat:@"CREATE DATABASE customer_%@", addCustomerName];
	res = PQexec (conn, [query UTF8String]);
	
	/* Switch to customer DB */
	PQfinish (conn);
	conn = PQsetdbLogin([[configController dbHostname] UTF8String],
						[[configController dbPort] UTF8String], 
						NULL, NULL, [[NSString stringWithFormat:@"customer_%@", addCustomerName] UTF8String], 
						[[configController dbUsername] UTF8String], 
						[[configController dbPassword] UTF8String]);
	if (PQstatus(conn) == CONNECTION_BAD)
	{
		[self reportError:@"Failed to connect to customer SQL database"];
		return;
	}
	
	/* Ensure action tables are present */
	res = PQexec (conn, "CREATE TABLE actions (id serial, descr varchar, enabled integer, activation integer, delay integer, rerun integer, rerundelay integer, timefilter integer, daymask integer, starthour integer, endhour integer, script varchar)");
	res = PQexec (conn, "CREATE TABLE action_configvars (id serial, action integer, name varchar, value varchar)");
	
	/* Create actions */
	if ([addCustomerMailTo length] > 0 && [addCustomerMailServer length] > 0)
	{
		/* Check sender */
		if (!addCustomerMailFrom || [addCustomerMailFrom length] < 1)
		{ self.addCustomerMailFrom = @"lithium"; }
		
		/* Add Email Action */
		res = PQexec (conn, "DELETE FROM actions WHERE descr='Default Email Notification Action'");
		res = PQexec (conn, "INSERT INTO actions (descr, enabled, activation, delay, rerun, rerundelay, timefilter, daymask, starthour, endhour, script) VALUES ('Default Email Notification Action', '1', '1', '0', '0', '0', '0', '127', '0', '0', 'email_alert.pl')");
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			[self reportError:@"Failed to add Email Alert record to database"];
			return;
		}	
		
		/* Get action ID */
		res = PQexec (conn, "SELECT lastval()");
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) < 1)
		{
			[self reportError:@"Failed to get Email Alert record ID"];
			return;
		}
		char *curid_str = PQgetvalue (res, 0, 0);
		int action_id = atoi (curid_str);
		
		/* Add Config variables */
		query = [NSString stringWithFormat:@"INSERT INTO action_configvars (action, name, value) VALUES ('%i', 'sender', '%s')",
				 action_id, [addCustomerMailFrom UTF8String]];
		res = PQexec (conn, [query UTF8String]);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			[self reportError:@"Failed to add email alert sender configuration record to database."];
			return;
		}	
		query = [NSString stringWithFormat:@"INSERT INTO action_configvars (action, name, value) VALUES ('%i', 'mailhost', '%s')",
				 action_id, [addCustomerMailServer UTF8String]];
		res = PQexec (conn, [query UTF8String]);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			[self reportError:@"Failed to add email alert server configuration record to database."];
			return;
		}	
		query = [NSString stringWithFormat:@"INSERT INTO action_configvars (action, name, value) VALUES ('%i', 'recipients', '%s')",
				 action_id, [addCustomerMailTo UTF8String]];
		res = PQexec (conn, [query UTF8String]);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			[self reportError:@"Failed to add email alert recipients configuration record to database."];
			return;
		}	
		
	}
	if (addCustomerEnablePush)
	{
		/* Add push notification */
		res = PQexec (conn, "DELETE FROM actions WHERE descr='Default iPhone Push Notification Action'");
		res = PQexec (conn, "INSERT INTO actions (descr, enabled, activation, delay, rerun, rerundelay, timefilter, daymask, starthour, endhour, script) VALUES ('Default iPhone Push Notification Action', '1', '1', '0', '0', '0', '0', '127', '0', '0', 'push_alert.pl')");
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			[self reportError:@"Failed to add email alert record to database."];
			return;
		}
		
	}
	PQfinish (conn);
	conn = nil;	
	
	/* Add customer to list */
	LCCustomer *customer = [LCCustomer customerWithName:addCustomerDesc desc:addCustomerDesc];
	[self insertObject:customer inCustomersAtIndex:customers.count];
	
	/* Close sheet */
	[NSApp endSheet:addCustomerSheet];
	[addCustomerSheet close];	
}

- (IBAction) addCustomerCancelClicked:(id)sender
{
	/* Close sheet */
	[NSApp endSheet:addCustomerSheet];
	[addCustomerSheet close];	
}

#pragma mark "Delete Selected"

- (IBAction) deleteSelectedClicked:(id)sender
{
	if ([[customerArrayController selectedObjects] count] < 1) return;
	
	LCCustomer *selectedCustomer = [[customerArrayController selectedObjects] objectAtIndex:0];
	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm Customer Delete"
									 defaultButton:@"Delete Customer and Configuration"
								   alternateButton:@"Delete Customer Only"
									   otherButton:@"Cancel"
						 informativeTextWithFormat:[NSString stringWithFormat:@"Customer %@ will be deleted. This operation can not be undone.", selectedCustomer.desc]];
	[alert setAlertStyle:NSCriticalAlertStyle];
	[alert beginSheetModalForWindow:window
					  modalDelegate:self
					 didEndSelector:@selector(deleteAlertDidEnd:returnCode:contextInfo:)
						contextInfo:selectedCustomer];
}

- (void) deleteAlertDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	/* Perform Delete */
	LCCustomer *selectedCustomer = contextInfo;

	if (returnCode == NSAlertDefaultReturn || returnCode == NSAlertOtherReturn)
	{
		/* Connect to SQL */
		PGconn *conn;
		conn = PQsetdbLogin([[configController dbHostname] cStringUsingEncoding:NSUTF8StringEncoding], [[configController dbPort] cStringUsingEncoding:NSUTF8StringEncoding], NULL, NULL, "lithium", [[configController dbUsername] cStringUsingEncoding:NSUTF8StringEncoding], [[configController dbPassword] cStringUsingEncoding:NSUTF8StringEncoding]);
		if (PQstatus(conn) == CONNECTION_BAD)
		{
			[self reportError:@"Connection to Lithium Core SQL database failed."];
			return;
		}
		
		/* Delete from customers table */
		NSString *query = [NSString stringWithFormat:@"DELETE FROM customers WHERE name='%@'", selectedCustomer.name];
		PGresult *res = PQexec (conn, [query UTF8String]);
		
		/* Delete whole database if requested */
		if (returnCode == NSAlertDefaultReturn)
		{
			query = [NSString stringWithFormat:@"DROP DATABASE customer_%@", selectedCustomer.name];
			res = PQexec (conn, [query UTF8String]);
		}
		
		PQfinish (conn);
		conn = nil;
		
		/* Delete from list */
		[self removeObjectFromCustomersAtIndex:[customers indexOfObject:selectedCustomer]];
	}
}

#pragma mark "Edit Customer"

- (IBAction) editSelectedClicked:(id)sender
{
	if ([[customerArrayController selectedObjects] count] < 1) return;
	editCustomer = [[[customerArrayController selectedObjects] objectAtIndex:0] retain];
	
	[editCustomerDescField setStringValue:editCustomer.desc];
	
	[NSApp beginSheet:editCustomerSheet
	   modalForWindow:window
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (IBAction) editCustomerCancelClicked:(id)sender
{
	[NSApp endSheet:editCustomerSheet];
	[editCustomerSheet orderOut:self];
	[editCustomer release];
	editCustomer = nil;
}

- (IBAction) editCustomerSaveClicked:(id)sender
{
	
	if ([[editCustomerDescField stringValue] length] > 0)
	{
		editCustomer.desc = [editCustomerDescField stringValue];

		[NSApp endSheet:editCustomerSheet];
		[editCustomerSheet orderOut:self];
		
		NSAlert *alert = [NSAlert alertWithMessageText:@"Lithium Request Required"
										 defaultButton:@"Restart Now"
									   alternateButton:@"Later"
										   otherButton:nil
							 informativeTextWithFormat:@"Lithium Core must be restarted for the changes to take affect. Would you like to restart Lithium Core now or later?"];
		[alert beginSheetModalForWindow:window
						  modalDelegate:self
						 didEndSelector:@selector(restartAlertDidEnd:returnCode:contextInfo:)
							contextInfo:nil];
		[editCustomer release];
		editCustomer = nil;	
	}
	else
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Customer Description is Required"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"A Description must be set for the Customer"];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:editCustomerSheet
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
	}
}

- (void) restartAlertDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSAlertDefaultReturn)
	{
		AuthorizationRef authRef;
		OSStatus err;
		
		err = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef);
		if (err == 0)
		{
			FILE *commsPipe = NULL;
			char *args[] = { "lithium", "restart", NULL};
			char *myPath = (char *) [[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"HelperTool"] cStringUsingEncoding:NSUTF8StringEncoding];
			
			err = AuthorizationExecuteWithPrivileges(authRef,myPath,kAuthorizationFlagDefaults,args,&commsPipe);	
			err = AuthorizationFree(authRef,kAuthorizationFlagDefaults);
		}
	}
}

#pragma mark "Customer List"

@synthesize customers;
- (void) insertObject:(id)obj inCustomersAtIndex:(unsigned int)index
{
	LCCustomer *customer = obj;
	[customers insertObject:obj atIndex:index];
	[customerDict setObject:obj forKey:customer.name];
}
- (void) removeObjectFromCustomersAtIndex:(unsigned int)index
{
	LCCustomer *customer = [customers objectAtIndex:index];
	[customerDict removeObjectForKey:customer.name];
	[customers removeObjectAtIndex:index];
}
@synthesize customerDict;

- (void) refreshCustomerList
{
	/* Connect to SQL */
	PGconn *conn;
	conn = PQsetdbLogin([[configController dbHostname] cStringUsingEncoding:NSUTF8StringEncoding], [[configController dbPort] cStringUsingEncoding:NSUTF8StringEncoding], NULL, NULL, "lithium", [[configController dbUsername] cStringUsingEncoding:NSUTF8StringEncoding], [[configController dbPassword] cStringUsingEncoding:NSUTF8StringEncoding]);
	if (PQstatus(conn) == CONNECTION_BAD)
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Unable to connect to Database"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"A connection to Lithium's SQL database could not be established."];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:window
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
		return;
	}
	
	/* Get customer list */
	PGresult *res = PQexec (conn, "SELECT name, descr from customers ORDER BY name ASC");
	if (res && PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		int rows = PQntuples(res);
		int row;
		for (row=0; row < rows; row++)
		{
			char *name_str = PQgetvalue (res, row, 0);
			char *desc_str = PQgetvalue (res, row, 1);
			LCCustomer *customer = [LCCustomer customerWithName:[NSString stringWithCString:name_str encoding:NSUTF8StringEncoding]
														   desc:[NSString stringWithCString:desc_str encoding:NSUTF8StringEncoding]];
			if (![customerDict objectForKey:customer.name])
			{ [self insertObject:customer inCustomersAtIndex:customers.count]; } 
		}	
	}
	else 
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Unable to load the Customer List"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"The Customer List could not be loaded from Lithium's SQL Database"];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:window
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
	}

	PQfinish (conn);
	conn = nil;
}

#pragma mark "Properties"

@synthesize addCustomerDesc;
- (void) setAddCustomerDesc:(NSString *)value
{
	[addCustomerDesc release];
	addCustomerDesc = [value copy];
	
	if (addCustomerSetNameAutomatically)
	{
		self.addCustomerName = [LCName parse:addCustomerDesc];
	}
}
@synthesize addCustomerName;
- (void) setAddCustomerName:(NSString *)value
{
	[addCustomerName release];
	if (value)
	{
		addCustomerName = [[LCName parse:value] copy];
		self.addCustomerURL = [NSString stringWithFormat:@"http://server:51180/%@", addCustomerName];
	}
	else
	{ 
		addCustomerName = nil; 
		self.addCustomerURL = nil;
	}
}
@synthesize addCustomerURL;
@synthesize addCustomerSetNameAutomatically;
@synthesize addCustomerMailServer;
@synthesize addCustomerMailFrom;
@synthesize addCustomerMailTo;
@synthesize addCustomerEnablePush;
@synthesize customerSortDescriptors;

@end
