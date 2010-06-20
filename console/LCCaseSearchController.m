//
//  LCCaseSearchController.m
//  Lithium Console
//
//  Created by James Wilson on 10/01/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCaseSearchController.h"

#import "LCCase.h"
#import "LCCaseController.h"

@implementation LCCaseSearchController

#pragma mark "Initialisation"

- (LCCaseSearchController *) init
{
	/* Create case list */
	caseList = [[LCCaseList alloc] init];
	
	/* Set Defaults */
	self.stateInteger = -1;
	NSString *lastCustomerName = [[NSUserDefaults standardUserDefaults] objectForKey:@"LCCaseSearchControllerLastCustomerName"];
	self.customer = [[LCCustomerList masterDict] objectForKey:lastCustomerName];
	if (!self.customer && [[LCCustomerList masterArray] count])
	{ self.customer = [[LCCustomerList masterArray] objectAtIndex:0]; }
	self.startUpper = [NSDate date];
	self.startLower = [NSDate dateWithNaturalLanguageString:@"Last Week"];
	self.endUpper = [NSDate date];
	self.endLower = [NSDate dateWithNaturalLanguageString:@"Last Week"];
	self.openedBetweenChecked = NO;
	self.closedBetweenChecked = NO;
	
	/* Sort items */
	NSMutableDictionary *sortDict;
	sortItems = [[NSMutableArray array] retain];
	sortDict = [NSMutableDictionary dictionary];
	[sortDict setObject:@"Case ID" forKey:@"desc"];
	[sortDict setObject:[NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"caseID" ascending:YES] autorelease]]
				 forKey:@"array"];
	[sortItems addObject:sortDict];
	sortDict = [NSMutableDictionary dictionary];
	[sortDict setObject:@"State" forKey:@"desc"];
	[sortDict setObject:[NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"state" ascending:YES] autorelease]]
				 forKey:@"array"];
	[sortItems addObject:sortDict];
	sortDict = [NSMutableDictionary dictionary];
	[sortDict setObject:@"Headline" forKey:@"desc"];
	[sortDict setObject:[NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"headline" ascending:YES] autorelease]]
				 forKey:@"array"];
	[sortItems addObject:sortDict];
	sortDict = [NSMutableDictionary dictionary];
	[sortDict setObject:@"Owner" forKey:@"desc"];
	[sortDict setObject:[NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"owner" ascending:YES] autorelease]]
				 forKey:@"array"];
	[sortItems addObject:sortDict];
	sortDict = [NSMutableDictionary dictionary];
	[sortDict setObject:@"Requester" forKey:@"desc"];
	[sortDict setObject:[NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"requester" ascending:YES] autorelease]]
				 forKey:@"array"];
	[sortItems addObject:sortDict];
	sortDict = [NSMutableDictionary dictionary];
	[sortDict setObject:@"Opened Date" forKey:@"desc"];
	[sortDict setObject:[NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"openDateSeconds" ascending:YES] autorelease]]
				 forKey:@"array"];
	[sortItems addObject:sortDict];
	sortDescriptorArray = [sortDict objectForKey:@"array"];
	sortDict = [NSMutableDictionary dictionary];
	[sortDict setObject:@"Closed Date" forKey:@"desc"];
	[sortDict setObject:[NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"closeDateSeconds" ascending:YES] autorelease]]
				 forKey:@"array"];
	[sortItems addObject:sortDict];
	
	self = [super initWithWindowNibName:@"CaseSearchWindow"];
	[self window];
	
	return self;
}

- (void) dealloc
{
	[caseList release];
	[headline release];
	[requester release];
	[startLower release];
	[startUpper release];
	[endLower release];
	[endUpper release];
	[super dealloc];
}

#pragma mark "UI Action Methods"

- (IBAction) searchClicked:(id)sender
{
	/* Check customer */
	if (![caseList customer])
	{
		/* Invalid customer */
		NSAlert *alert = [[NSAlert alloc] init];
		[alert addButtonWithTitle:@"OK"];
		[alert setMessageText:@"Error: Invalid customer selected"];
		[alert setInformativeText:@"A valid customer name must be selected"];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert runModal];
		[alert autorelease];
	
		return;
	}
		
	/* Check at-least one criteria is set */
	if (!caseIDChecked &&
		!stateChecked &&
		!headlineChecked &&
		!ownerChecked &&
		!requesterChecked && 
		!openedBetweenChecked &&
		!closedBetweenChecked) 
	{
		/* No criteria selected */
		NSAlert *alert = [[NSAlert alloc] init];
		[alert addButtonWithTitle:@"OK"];
		[alert setMessageText:@"Error: No criteria specified"];
		[alert setInformativeText:@"Atleast one of search criteria must be specified"];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert runModal];
		[alert autorelease];
		
		return;
	}

	/* Set criteria */
	[caseList removeAllCriteria];
	if (caseIDChecked == YES) [caseList setCaseID:[NSNumber numberWithInt:[caseID intValue]]];
	if (stateChecked == YES) [caseList setStateInteger:[NSNumber numberWithInt:stateInteger]];
	if (headlineChecked == YES) [caseList setHeadline:headline];
//	if (ownerChecked == YES) [caseList setOwner:owner];
	if (requesterChecked == YES) [caseList setRequester:requester];
	if (openedBetweenChecked) 
	{
		[caseList setStartLower:startLower];
		[caseList setStartUpper:startUpper]; 
	}
	if (closedBetweenChecked)
	{
		[caseList setEndLower:endLower];
		[caseList setEndUpper:endUpper];
	}
	
	/* Execute search */
	[caseList highPriorityRefresh];
}

- (IBAction) cancelClicked:(id)sender
{
	/* Close sheet */
	[NSApp endSheet:[self window]];
	[[self window] close];
	
	/* Remove bindings/observers */
	[controllerAlias setContent:nil];
	
	/* Autorelease */
	[self autorelease];
}

- (IBAction) caseTableViewDoubleClicked:(NSArray *)selectedObjects
{
	LCCase *cas;
	for (cas in selectedObjects)
	{
		[[LCCaseController alloc] initWithCase:cas];
	}
}

#pragma mark "Accessor Methods"

- (LCCustomerList *) customerList
{ return [LCCustomerList masterList]; }

@synthesize customer;
- (void) setCustomer:(LCCustomer *)value
{
	[customer release];
	customer = [value retain];
	
	caseList.customer = customer;
	
	[[NSUserDefaults standardUserDefaults] setObject:customer.name forKey:@"LCCaseSearchControllerLastCustomerName"];
}

@synthesize sortDescriptorArray;
- (void) setSortDescriptorArray:(NSArray *)array
{
	sortDescriptorArray = array;
	[tableView reloadData];
}

@synthesize caseIDChecked;
@synthesize stateChecked;
@synthesize headlineChecked;
@synthesize ownerChecked;
@synthesize requesterChecked;
@synthesize openedBetweenChecked;
@synthesize closedBetweenChecked;
@synthesize sortItems;
@synthesize caseList;

@synthesize stateInteger;
- (void) setStateInteger:(int)value
{
	stateInteger = value;
	if (stateInteger == -1) self.stateChecked = NO;
	else self.stateChecked = YES;
}
@synthesize caseID;
- (void) setCaseID:(NSString *)value
{
	[caseID release];
	caseID = [value copy];	
	self.caseIDChecked = [caseID length] ? YES : NO;
}
@synthesize headline;
- (void) setHeadline:(NSString *)value
{
	[headline release];
	headline = [value copy];
	self.headlineChecked = [headline length] ? YES : NO;
}
@synthesize requester;
- (void) setRequester:(NSString *)value
{
	[requester release];
	requester = [value copy];
	self.requesterChecked = [requester length] ? YES : NO;
}
@synthesize caseOwner;
- (void) setCaseOwner:(NSString *)value
{
	[caseOwner release];
	caseOwner = [value copy];
	self.ownerChecked = [caseOwner length] ? YES : NO;
}
@synthesize startLower;
- (void) setStartLower:(NSDate *)value
{
	[startLower release];
	startLower = [value copy];
	self.openedBetweenChecked = startLower ? YES : NO;
}
@synthesize startUpper;
- (void) setStartUpper:(NSDate *)value
{
	[startUpper release];
	startUpper = [value copy];
	self.openedBetweenChecked = startUpper ? YES : NO;
}
@synthesize endLower;
- (void) setEndLower:(NSDate *)value
{
	[endLower release];
	endLower = [value copy];
	self.closedBetweenChecked = endLower ? YES : NO;
}
@synthesize endUpper;
- (void) setEndUpper:(NSDate *)value
{
	[endUpper release];
	endUpper = [value copy];
	self.closedBetweenChecked = endUpper ? YES : NO;
}

@end
