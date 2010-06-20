//
//  LCLicenseController.m
//  LCAdminTools
//
//  Created by James Wilson on 25/09/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCLicenseController.h"
#import "LCLicenseKey.h"
#import "LCZeroIsUnlimitedTransformer.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <libpq-fe.h>
#include <SecurityFoundation/SFAuthorization.h>

@interface LCLicenseController (private)

- (void) activateLicenseKey;
- (void) addLicenseKeyToSQL:(NSString *)keyString customerName:(NSString *)custName;
 
@end

@implementation LCLicenseController

#pragma mark "Constructors"

- (void) awakeFromNib
{
	licenses = [[NSMutableArray array] retain];
	licenseDict = [[NSMutableDictionary dictionary] retain];
	
	LCZeroIsUnlimitedTransformer *zeroUnlimitedTransformer = [[[LCZeroIsUnlimitedTransformer alloc] init] autorelease];
	[NSValueTransformer setValueTransformer:zeroUnlimitedTransformer forName:@"LCZeroIsUnlimitedTransformer"];
}

#pragma mark "Add New Key"

- (IBAction) addNewKeyClicked:(id)sender
{
	/* Reset state */
	self.firstname = nil;
	self.lastname = nil;
	self.company = nil;
	self.email = nil;
	self.keyAlreadyActivated = NO;
	self.licenseKeyString = nil;
	self.signedKeyString = nil;
	urlConn = nil;
	receivedData = nil;
	
	/* Open Sheet */
	[tabView selectTabViewItemWithIdentifier:@"start"];
	[NSApp beginSheet:addKeySheet
	   modalForWindow:window
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];	
	
}

- (IBAction) addSheetAddKeyClicked:(id)sender
{
	/* The first-stage 'Add' button */
	if ([licenseKeyString length] > 100 && [licenseKeyString length] < 190)
	{
		/* Signed-key Entered, decode it  */
		LCLicenseKey *key = [LCLicenseKey keyWithID:0 customerName:nil string:licenseKeyString];
		if (key.status != KEY_VALID)
		{
			NSAlert *alert = [NSAlert alertWithMessageText:@"Invalid License Key"
											 defaultButton:@"OK"
										   alternateButton:nil
											   otherButton:nil
								 informativeTextWithFormat:@"The license key entered is invalid. Please double check that the key was entered exactly as shown on your invoice or product packaging."];
			[alert setAlertStyle:NSCriticalAlertStyle];
			[alert beginSheetModalForWindow:addKeySheet
							  modalDelegate:self
							 didEndSelector:nil
								contextInfo:nil];
			return;
		}
		
		/* Check a customer matches */
		LCCustomer *licensedCustomer = nil;
		for (LCCustomer *existingCustomer in customerController.customers)
		{
			if ([key licensedForCustomerName:existingCustomer.name])
			{
				licensedCustomer = existingCustomer;
				break;
			}
		}
		if (!licensedCustomer)
		{
			NSAlert *alert = [NSAlert alertWithMessageText:@"Licensed Customer Not Found"
											 defaultButton:@"OK"
										   alternateButton:nil
											   otherButton:nil
								 informativeTextWithFormat:@"The license key entered is bound to a particular Lithum Core Customer Name. The licensed customer is not present on this Lithium Core deployment. Please contact support@lithiumcorp.com for assistance."];
			[alert setAlertStyle:NSCriticalAlertStyle];
			[alert beginSheetModalForWindow:addKeySheet
							  modalDelegate:self
							 didEndSelector:nil
								contextInfo:nil];
			return;
		}
		
		/* Add to SQL */
		[self addLicenseKeyToSQL:licenseKeyString customerName:licensedCustomer.name];
		
		/* Finished */
		[tabView selectTabViewItemWithIdentifier:@"finished"];
		
		return;
	}
	else
	{
		/* Unsigned eKey Entered */

		/* Create URL String */
		NSString *urlString = [NSString stringWithFormat:@"https://secure.lithiumcorp.com.au/vince/validate_key5.php?key=%@", 
							   [licenseKeyString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
		
		/* Setup URL request */
		NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
																  cachePolicy:NSURLRequestReloadIgnoringCacheData
															  timeoutInterval:30.0];
		/* Perform request */
		self.xmlOperationInProgress = YES;
		self.progressString = @"Validating License Key...";
		self.errorMessage = nil;
		[tabView selectTabViewItemWithIdentifier:@"validating_progress"];
		urlConn = [[NSURLConnection connectionWithRequest:urlRequest delegate:self] retain];
		if (urlConn)
		{
			/* Connection proceeding */
			receivedData = [[NSMutableData data] retain];
		}
		else
		{ 
			/* Connection failed */
			[self connection:nil didFailWithError:nil];
		}	
	}
}

- (IBAction) addSheetAddCustomerClicked:(id)sender
{
	/* Add Customer */
	/* FIX Implement */
}

- (IBAction) addSheetActivateClicked:(id)sender
{
	/* After entering the activation info */

	/* Validate */
	BOOL valid = YES;	
	if ([[firstnameTextField stringValue] length] < 1)
	{ [firstnameTextField setBackgroundColor:[NSColor redColor]]; valid = NO; }
	else
	{ [firstnameTextField setBackgroundColor:[NSColor whiteColor]]; }
	if ([[lastnameTextField stringValue] length] < 1)
	{ [lastnameTextField setBackgroundColor:[NSColor redColor]]; valid = NO; }
	else
	{ [lastnameTextField setBackgroundColor:[NSColor whiteColor]]; }
	if ([[companyTextField stringValue] length] < 1)
	{ [companyTextField setBackgroundColor:[NSColor redColor]]; valid = NO; }
	else
	{ [companyTextField setBackgroundColor:[NSColor whiteColor]]; }
	if ([[emailTextField stringValue] length] < 1)
	{ [emailTextField setBackgroundColor:[NSColor redColor]]; valid = NO; }
	else if (!strstr([[emailTextField stringValue] UTF8String], "@"))
	{ [emailTextField setBackgroundColor:[NSColor redColor]]; valid = NO; }
	else
	{ [emailTextField setBackgroundColor:[NSColor whiteColor]]; }	
	if (!valid) return;
	
	/* Check if the key will be customer bounds */
	if (keyIsBoundToCustomer)
	{
		/* Alert customer to binding */
		NSAlert *alert = [NSAlert alertWithMessageText:[NSString stringWithFormat:@"License Key will be bound to Customer %@ (%@)", self.customer.name, self.customer.desc]
										 defaultButton:@"Activate"
									   alternateButton:@"Cancel"
										   otherButton:nil
							 informativeTextWithFormat:[NSString stringWithFormat:@"The License Key will be activated and bound to the Lithium Core Customer '%@'. Licenses can not be transferred to or re-used with different Customers once activated. Please be sure this is the Customer Name you want associated with this License Key before proceeding.",
														self.customer.name]];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:addKeySheet
						  modalDelegate:self
						 didEndSelector:@selector(boundAlertDidEnd:returnCode:contextInfo:)
							contextInfo:nil];
	}
	else
	{
		/* Activate */
		[tabView selectTabViewItemWithIdentifier:@"activate_progress"];	
		[self activateLicenseKey];			
	}

}

- (void) boundAlertDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSAlertDefaultReturn)
	{
		/* Activate */
		[tabView selectTabViewItemWithIdentifier:@"activate_progress"];	
		[self activateLicenseKey];
	}
}

- (void) activateLicenseKey
{
	/* Create URL String */
	NSString *urlString = [NSString stringWithFormat:@"https://secure.lithiumcorp.com.au/vince/activate_license5.php?firstname=%@&lastname=%@&company=%@&email=%@&custname=%@&host=%@&key=%@", 
						   [firstname stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [lastname stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [company stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [email stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [customer.name stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [@"N/A" stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [licenseKeyString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
	
	/* Setup URL request */
	NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
															  cachePolicy:NSURLRequestReloadIgnoringCacheData
														  timeoutInterval:30.0];
	/* Perform request */
	self.xmlOperationInProgress = YES;
	self.progressString = @"Requesting License Activation...";
	self.errorMessage = @"";
	[tabView selectTabViewItemWithIdentifier:@"activate_progress"];
	urlConn = [[NSURLConnection connectionWithRequest:urlRequest delegate:self] retain];
	if (urlConn)
	{
		/* Connection proceeding */
		receivedData = [[NSMutableData data] retain];
	}
	else
	{ 
		/* Connection failed */
		[self connection:nil didFailWithError:nil];
	}
}

- (void) connection:(NSURLConnection *)connection 
 didReceiveResponse:(NSURLResponse *)response
{
	/* Received non-data response */
	[receivedData setLength:0];
	self.progressString = @"Received non-data response";
}

-(void)	connection:(NSURLConnection *) connection
	didReceiveData:(NSData *) data
{
	/* Received the actual data */
	[receivedData appendData:data];
	self.progressString = @"Receiving data";
}

-(NSURLRequest *)	connection:(NSURLConnection *)connection
			 willSendRequest:(NSURLRequest *)request
			redirectResponse:(NSURLResponse *)redirectResponse
{
	/* Allow redirects */
	self.progressString = @"Received re-direction";
    return request;
}

-(void) connectionDidFinishLoading:(NSURLConnection *) connection
{
	/* Demo key Download finished */
	
	/* Validate & Parse */
	NSString *xml = [[[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding] autorelease];
	if ([xml hasPrefix:@"<?xml"])
	{
		/* Parse XML doc */
		NSXMLParser *parser = [[[NSXMLParser alloc] initWithData:receivedData] autorelease];
		[parser setDelegate:self];
		[parser setShouldResolveExternalEntities:YES];
		[parser parse];
		
		/* Check state */
		if ([[[tabView selectedTabViewItem] identifier] isEqualToString:@"validating_progress"])
		{
			/* Validating */
			if (self.result == 1)
			{
				/* Key was activated successfully */
				self.progressString = @"License Validated.";
				
				/* Check Type */
				if ([licenseType isEqualToString:@"RACN"] || [licenseType isEqualToString:@"RMSP"])
				{ self.keyIsBoundToCustomer = NO; }
				else 
				{ 
					self.keyIsBoundToCustomer = YES; 
					if (!self.customer && customerController.customers.count > 0)
					{
						self.customer = [customerController.customers objectAtIndex:0];
					}
				}

				/* Move to activation */
				[tabView selectTabViewItemWithIdentifier:@"activate"];				
			}
			else
			{
				self.xmlOperationInProgress = NO;
				[tabView selectTabViewItemWithIdentifier:@"start"];
				NSAlert *alert = [NSAlert alertWithMessageText:@"Invalid License Key"
												 defaultButton:@"OK"
											   alternateButton:nil
												   otherButton:nil
									 informativeTextWithFormat:@"The license key entered was not recognized by LithiumCorp's License Server. Please double check that you have entered it exactly as it appears on your invoice or product packaging"];
				[alert setAlertStyle:NSCriticalAlertStyle];
				[alert beginSheetModalForWindow:addKeySheet
								  modalDelegate:self
								 didEndSelector:nil
									contextInfo:nil];
			}				
		}
		else if ([[[tabView selectedTabViewItem] identifier] isEqualToString:@"activate_progress"])
		{
			/* Activating */
			if (self.result == 1)
			{
				/* Key was activated successfully */
				self.progressString = @"License Activated.";

				/* Add to SQL */
				if (keyIsBoundToCustomer)
				{ [self addLicenseKeyToSQL:self.signedKeyString customerName:keyIsBoundToCustomer ? customer.name : nil]; }
				
				/* Finished */
				[tabView selectTabViewItemWithIdentifier:@"finished"];
			}
			else
			{
				/* Failed */
				self.xmlOperationInProgress = NO;
				self.progressString = @"License Key Activation Failed.";
				self.errorMessage = nil;
				self.keyAlreadyActivated = YES;
				[tabView selectTabViewItemWithIdentifier:@"failed"];
			}		
		}
	}
	else
	{
		/* Go to failed, response invalid */
		self.xmlOperationInProgress = NO;
		self.progressString = @"Error in response from Registration Server. Try again later.";
		self.errorMessage = @"If this problem persists, please email support@lithiumcorp.com.";
		self.keyAlreadyActivated = NO;
		[tabView selectTabViewItemWithIdentifier:@"failed"];
	}
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Release connection */
	urlConn = nil;
	[connection release];	
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	/* Report status */
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Check status */
	[tabView selectTabViewItemWithIdentifier:@"start"];
	NSAlert *alert = [NSAlert alertWithMessageText:@"Unable to contact License Server"
									 defaultButton:@"OK"
								   alternateButton:nil
									   otherButton:nil
						 informativeTextWithFormat:@"The LithiumCorp License Server could not be contacted to complete the license key validation. Please contact support@lithiumcorp.com to request a manual license key activation."];
	[alert setAlertStyle:NSCriticalAlertStyle];
	[alert beginSheetModalForWindow:addKeySheet
					  modalDelegate:self
					 didEndSelector:nil
						contextInfo:nil];
	
	/* Release connection */
	urlConn = nil;
	[connection release];
}


- (IBAction) cancelAddClicked:(id)sender
{
	/* Global cancel of adding a new license */
	
	/* Cancel XML operation, if any */
	[urlConn cancel];
	[urlConn release];
	urlConn = nil;
	[receivedData release];
	receivedData = nil;
	
	/* Close sheet */
	[NSApp endSheet:addKeySheet];
	[addKeySheet close];
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	/* Release previous string */
	if (xmlString)
	{
		[xmlString release];
		xmlString = nil;
	}
}

- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	/* Create new string or append string to existing */
	if (xmlString) { [xmlString appendString:string]; }
	else { xmlString = [[NSMutableString stringWithString:string] retain]; }
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Update properties */
	if (xmlString)
	{ 
		/*
		 * Update our properties 
		 */
		if ([element isEqualToString:@"message"])
		{ self.errorMessage = xmlString; }
		else if ([element isEqualToString:@"result"])
		{ self.result = [xmlString intValue]; }
		else if ([element isEqualToString:@"key"])
		{ self.signedKeyString = xmlString; }
		else if ([element isEqualToString:@"type"])
		{ self.licenseType = xmlString; }
	}
	
	/* Release current element */
	[xmlString release];
	xmlString = nil;
}

- (void) addLicenseKeyToSQL:(NSString *)keyString customerName:(NSString *)custName
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
	
	/* Insert Key */
	NSString *custNameSql = @"NULL";
	if (custName)
	{ custNameSql = [NSString stringWithFormat:@"'%@'", custName]; }
	NSString *query = [NSString stringWithFormat:@"INSERT INTO license_keys (key, cust_name) VALUES ('%@', %@)", keyString, custNameSql];
	PQexec (conn, [query UTF8String]);
	
	/* DB Cleanup */
	PQfinish (conn);
	conn = nil;	
	
	/* Call refresh */
	[self refreshLicenseKeyList];
}

#pragma mark "Delete Selected"

- (IBAction) deleteSelectedKeyClicked:(id)sender
{
	if ([[licenseArrayController selectedObjects] count] < 1) return;
	
	LCLicenseKey *selectedKey = [[licenseArrayController selectedObjects] objectAtIndex:0];
	
	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm License Key Delete"
									 defaultButton:@"Delete"
								   alternateButton:@"Cancel"
									   otherButton:nil
						 informativeTextWithFormat:@"This action can not be undone."];
	[alert setAlertStyle:NSCriticalAlertStyle];
	[alert beginSheetModalForWindow:window
					  modalDelegate:self
					 didEndSelector:@selector(deleteAlertDidEnd:returnCode:contextInfo:)
						contextInfo:selectedKey];
}

- (void) deleteAlertDidEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSAlertDefaultReturn)
	{
		LCLicenseKey *selectedKey = contextInfo;

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
		
		/* Insert Key */
		NSString *query = [NSString stringWithFormat:@"DELETE FROM license_keys WHERE id='%i'", selectedKey.keyID];
		PQexec (conn, [query UTF8String]);
		
		/* DB Cleanup */
		PQfinish (conn);
		conn = nil;			
		
		/* Remove from list */
		[self removeObjectFromLicensesAtIndex:[licenses indexOfObject:selectedKey]];
		[self refreshLicenseKeyList];
	}
}

#pragma mark "Key List"

@synthesize licenses;
- (void) insertObject:(id)obj inLicensesAtIndex:(unsigned int)index
{
	LCLicenseKey *key = obj;
	[licenses insertObject:obj atIndex:index];
	[licenseDict setObject:obj forKey:[NSNumber numberWithInt:key.keyID]];
}
- (void) removeObjectFromLicensesAtIndex:(unsigned int)index
{
	LCLicenseKey *key = [licenses objectAtIndex:index];
	[licenseDict removeObjectForKey:[NSNumber numberWithInt:key.keyID]];
	[licenses removeObjectAtIndex:index];
}
@synthesize licenseDict;

- (void) refreshLicenseKeyList
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
	NSMutableArray *seenKeys = [NSMutableArray array];
	PGresult *res = PQexec (conn, "SELECT id, key, cust_name FROM license_keys ORDER BY id ASC");
	if (res && PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		int rows = PQntuples(res);
		int row;
		for (row=0; row < rows; row++)
		{
			char *id_str = PQgetvalue (res, row, 0);
			char *key_str = PQgetvalue (res, row, 1);
			char *cust_str = PQgetvalue(res, row, 2);
			LCLicenseKey *key = [licenseDict objectForKey:[NSNumber numberWithInt:atoi(id_str)]];
			if (!key)
			{
				key = [LCLicenseKey keyWithID:atoi(id_str) 
								 customerName:[NSString stringWithUTF8String:cust_str]
									   string:[NSString stringWithUTF8String:key_str]];
				[self insertObject:key inLicensesAtIndex:licenses.count];
			}			
			[seenKeys addObject:key];
		}	
	}
	else 
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Unable to load the License Key List"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"The License Key List could not be loaded from Lithium's SQL Database"];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:window
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
	}
	
	PQfinish (conn);
	conn = nil;	
	
	/* Remove Obsolete */
	NSMutableArray *removeKeys = [NSMutableArray array];
	for (LCLicenseKey *key in licenses)
	{
		if (![seenKeys containsObject:key])
		{ [removeKeys addObject:key]; }
	}
	for (LCLicenseKey *key in removeKeys)
	{ [self removeObjectFromLicensesAtIndex:[licenses indexOfObject:key]]; }								  
	
	/* Check for duplicates */
	for (LCLicenseKey *key in licenses)
	{
		if (key.status != KEY_VALID) continue;
		for (LCLicenseKey *dup in licenses)
		{ 
			if (dup.status == KEY_VALID && dup.serial == key.serial && dup != key)
			{ 
				key.status = KEY_INVALID_DUPLICATE; 
			}
		}
	}
}

#pragma mark "Properties"

@synthesize firstname;
@synthesize lastname;
@synthesize company;
@synthesize email;
@synthesize keyAlreadyActivated;
@synthesize licenseKeyString;
@synthesize signedKeyString;	
@synthesize xmlOperationInProgress;	
@synthesize progressString;
@synthesize errorMessage;
@synthesize result;
@synthesize licenseType;
@synthesize keyIsBoundToCustomer;
@synthesize customer;
- (void) setCustomer:(id)value
{
	[customer release];
	customer = [value retain];
}

@end
