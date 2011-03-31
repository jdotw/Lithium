//
//  LCCustomer.m
//  Lithium Console
//
//  Created by James Wilson on 20/06/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCustomer.h"
#import "LCCustomerList.h"
#import "LCConsoleController.h"
#import "LCCase.h"
#import "LCServiceList.h"
#import "LCSite.h"
#import "LCCoreSetupWindowController.h"

@implementation LCCustomer

#pragma mark "Initialisation"

+ (id) customerWithName:(NSString *)initName cluster:(NSString *)initCluster node:(NSString *)initNode url:(NSString *)initBaseURL
{
	return [[[LCCustomer alloc] initWithName:initName cluster:initCluster node:initNode url:initBaseURL] autorelease];
}

- (id) initWithName:(NSString *)initName cluster:(NSString *)initCluster node:(NSString *)initNode url:(NSString *)initBaseURL
{
	[super initWithType:1 name:initName desc:initName resourceAddressString:[NSString stringWithFormat:@"%@:%@:5:0:%@", initCluster, initNode, initName]];;

	/* Set entity properties */
	self.cluster = initCluster;
	self.node = initNode;
	self.url = initBaseURL;
	self.browserViewType = 1;
	self.isConfigured = YES;
	
	/* Setup XML Translations */
	[self.xmlTranslation setObject:@"licenseType" forKey:@"license_type"];
	[self.xmlTranslation setObject:@"limitedLicense" forKey:@"license_limited"];
	[self.xmlTranslation setObject:@"coreVersion" forKey:@"core_version"];
	[self.xmlTranslation setObject:@"inductionVersion" forKey:@"induction_version"];
	[self.xmlTranslation setObject:@"userAuthLevel" forKey:@"auth_level"];

	/* Create persistentWindow array */
	persistentWindows = [[NSMutableArray array] retain];
		
	/* Create global entity list */
	globalEntityArray = [[NSMutableArray array] retain];		
	
	/* Create Open Cases List */
	openCasesList = [LCCaseList new];
	openCasesList.customer = self;
	openCasesList.stateInteger = [NSNumber numberWithInt:CASESTATE_OPEN];
	openCasesList.delegate = self;
	[self setOpenCasesAutoRefresh:YES];
	[[self openCasesList] normalPriorityRefresh];
	
	/* Create Active Incident List */
	activeIncidentsList = [LCIncidentList new];
	activeIncidentsList.customer = self;
	activeIncidentsList.isLiveList = YES;
	activeIncidentsList.delegate = self;
	[self setActiveIncidentsAutoRefresh:YES];
	[[self activeIncidentsList] normalPriorityRefresh];
	
	/* Create other lists, but don't refresh them yet.
	 * All child list refreshes should be done only
	 * after a successful refresh of the customer itself
	 */
	userList = [[LCUserList userListWithCustomer:self] retain];
	vendorList = [[LCVendorList vendorListWithCustomer:self] retain];
	serviceList = [[LCServiceList serviceListForCustomer:self] retain];
	processProfileList = [[LCProcessProfileList profileListForCustomer:self] retain];
	xsanList = [[LCXsanList xsanListForCustomer:self] retain];
	lunList = [[LCLunList lunListForCustomer:self] retain];
	documentList = (LCDocumentList *) [[LCDocumentList alloc] initWithCustomer:self];
	groupTree = (LCGroupTree *) [[LCGroupTree alloc] initWithCustomer:self];
	
	/* Fire refresh */
	[self normalPriorityRefresh];
	
	/* Auto-Refresh */
	customerRefreshTimer = [[NSTimer scheduledTimerWithTimeInterval:30.0
															 target:self
														   selector:@selector(highPriorityRefresh)
														   userInfo:nil
															repeats:YES] retain];
	
	return self;
}

- (void) dealloc
{
	if (activeIncidentsRefreshTimer) [activeIncidentsRefreshTimer invalidate];
	if (openCasesRefreshTimer) [openCasesRefreshTimer invalidate];
	if (customerRefreshTimer)
	{ [customerRefreshTimer invalidate]; [customerRefreshTimer release]; }
	[activeIncidentsList release];
	[openCasesList release];
	[userList release];
	[vendorList release];
	[serviceList release];
	[processProfileList release];
	[xsanList release];
	[lunList release];
	[cluster release];
	[node release];
	[url release];
	[coreVersion release];
	[inductionVersion release];
	[persistentWindows release];
	[super dealloc];
}

#pragma mark "Refresh Manipuilation Methods"

- (void) setOpenCasesAutoRefresh:(BOOL)flag
{
	if (flag == YES)
	{
		if (!openCasesRefreshTimer)
		{
			openCasesRefreshTimer = [NSTimer scheduledTimerWithTimeInterval:[[NSUserDefaults standardUserDefaults] floatForKey:@"refOpenCaseRefreshInterval"]
																	 target:openCasesList
																   selector:@selector(lowPriorityRefresh) 
																   userInfo:nil 
																	repeats:YES];
		}
	}
	else
	{
		if (openCasesRefreshTimer) [openCasesRefreshTimer invalidate];
		openCasesRefreshTimer  = nil;
	}
}

- (void) setActiveIncidentsAutoRefresh:(BOOL)flag
{
	if (flag == YES)
	{
		if (!activeIncidentsRefreshTimer)
		{
			activeIncidentsRefreshTimer = [NSTimer scheduledTimerWithTimeInterval:[[NSUserDefaults standardUserDefaults] floatForKey:@"refActiveIncidentRefreshInterval"] 
																		   target:activeIncidentsList
																		 selector:@selector(normalPriorityRefresh) 
																		 userInfo:nil 
																		  repeats:YES];	
		}		
	}
	else
	{
		if (activeIncidentsRefreshTimer) [activeIncidentsRefreshTimer invalidate];
		activeIncidentsRefreshTimer = nil;
	}
}


- (void) caselistRefreshFinished:(id)sender
{
}

- (void) inclistRefreshFinished:(id)sender
{
}

#pragma mark "Deployment Tree Methods"

- (NSArray *) customers
{ return nil; }

- (NSString *) status
{ return nil; }

- (int) statusInteger
{ return -2; }

- (BOOL) isCustomer
{ return YES; }

- (NSImage *) discoveredIcon
{ return nil; }

#pragma mark "Global Entity Array"

- (void) insertObject:(LCEntity *)entity inGlobalEntityArrayAtIndex:(unsigned int)index
{
	[globalEntityArray insertObject:entity atIndex:index];
}

- (void) removeObjectFromGlobalEntityArrayAtIndex:(unsigned int)index
{
	[globalEntityArray removeObjectAtIndex:index];
}

#pragma mark "Persistent Windows"

- (void) insertObject:(NSWindowController *)windowController inPersistentWindowsAtIndex:(unsigned int)index
{
//	if ([persistentWindows containsObject:windowController]) return;
//	[persistentWindows insertObject:windowController atIndex:index];
//	[[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:persistentWindows] forKey:[NSString stringWithFormat:@"persistentWindows_%@", [self name]]];
//	[[NSUserDefaults standardUserDefaults] synchronize];
}

- (void) removeObjectFromPersistentWindowsAtIndex:(unsigned int)index
{
//	[persistentWindows removeObjectAtIndex:index];
//	[[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:persistentWindows] forKey:[NSString stringWithFormat:@"persistentWindows_%@", [self name]]];
//	[[NSUserDefaults standardUserDefaults] synchronize];
}

- (void) removePersistentWindow:(NSWindowController *)windowController
{
//	if ([persistentWindows containsObject:windowController])
//	{ [self removeObjectFromPersistentWindowsAtIndex:[persistentWindows indexOfObject:windowController]]; }
}

- (void) thawPersistentWindows
{
	/* Clear timer */
//	[thawTimer release];
//	thawTimer = nil;
	
	/* Load persistent windows */
//	NSData *windowData = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithFormat:@"persistentWindows_%@", [self name]]];
//	if (windowData)
//	{ persistentWindows = [[NSKeyedUnarchiver unarchiveObjectWithData:windowData] retain]; }
//	if (!persistentWindows)
//	{ persistentWindows = [[NSMutableArray array] retain]; }
//	[[NSUserDefaults standardUserDefaults] removeObjectForKey:[NSString stringWithFormat:@"persistentWindows_%@", [self name]]];
//	NSEnumerator *windowEnum = [persistentWindows objectEnumerator];
//	NSWindowController *windowController;
//	while (windowController = [windowEnum nextObject])
//	{
//		[windowController retain];
//	}
}

#pragma mark "Refresh"

- (void) refreshWithPriority:(int)priority
{
	[super refreshWithPriority:priority];
}

- (void) XMLRequestFinished:(id)sender
{
	/* Check to see if this was a refresh that finished */
	BOOL successfulRefresh;
	if (sender == refreshXMLRequest && [sender success]) successfulRefresh = YES;
	else successfulRefresh = NO;
	
	/* Perform super-class handling of XMLRequestFinished:(id)sender */
	[super XMLRequestFinished:sender];
	
	/* Check to see if we are configured */
	if (self.isConfigured)
	{
		/* Perform related-list refresh operations if this refresh was successful */
		if (successfulRefresh)
		{
			[userList refreshWithPriority:XMLREQ_PRIO_NORMAL];
			[vendorList refreshWithPriority:XMLREQ_PRIO_NORMAL];
			[serviceList refreshWithPriority:XMLREQ_PRIO_NORMAL];
			[processProfileList refreshWithPriority:XMLREQ_PRIO_NORMAL];
			[xsanList refreshWithPriority:XMLREQ_PRIO_NORMAL];		
			[lunList refreshWithPriority:XMLREQ_PRIO_NORMAL];
			[documentList refreshWithPriority:XMLREQ_PRIO_NORMAL];
			[groupTree refreshWithPriority:XMLREQ_PRIO_NORMAL];
		}
	}
	else 
	{
		/* This customer is not yet configured; the LCCoreSetupWindowController
		 * should be used to configure it 
		 */
		
		NSLog (@"UNCONFIGURED CUSTOMER: %@", self.name);
		
		if ([[LCCustomerList masterArray] count] == 1)
		{
			/* We're the only customer present; show the sheet to 
			 * perform the initial setup
			 */
			LCCoreSetupWindowController *wc = [[[LCCoreSetupWindowController alloc] initWithCustomer:self] autorelease];
			[NSApp beginSheet:[wc window]
			   modalForWindow:[[[LCConsoleController masterController] browserForSheet] window]
				modalDelegate:nil
			   didEndSelector:nil
				  contextInfo:nil];
		}
	}
}

#pragma mark "User Auth Levels"

@synthesize userAuthLevel;
- (void) setUserAuthLevel:(int)value
{
	userAuthLevel = value;
	
	if ([self userAuthLevel] >= 40) self.userIsAdmin = YES;
	else self.userIsAdmin = NO; 
	
	if ([self userAuthLevel] >= 30) self.userIsNormal = YES;
	else self.userIsNormal = NO; 

	if ([self userAuthLevel] >= 20) self.userIsReadOnly = YES;
	else self.userIsReadOnly = NO;
}

@synthesize userIsAdmin;
@synthesize userIsNormal;
@synthesize userIsReadOnly;

#pragma mark "Accessor Methods"

- (Class) childClass
{ return [LCSite class]; }

- (BOOL) isDynamic
{ return NO; }

- (BOOL) isDeployment
{ return NO; }

- (NSString *) address
{ return nil; }

- (int) port
{ return 0; }

@synthesize cluster;
@synthesize node;
@synthesize url;
@synthesize coreVersion;
@synthesize inductionVersion;
@synthesize licenseType;
@synthesize username;
@synthesize password;

@synthesize openCasesList;
@synthesize openCasesRefreshTimer;
@synthesize activeIncidentsList;
@synthesize activeIncidentsRefreshTimer;
@synthesize vendorList;
@synthesize userList;
@synthesize serviceList;
@synthesize processProfileList;
@synthesize xsanList;
@synthesize lunList;
@synthesize documentList;
@synthesize globalEntityArray;
@synthesize thawTimer;
@synthesize persistentWindows;
@synthesize customerRefreshTimer;
@synthesize disabled;
@synthesize setupController;
@synthesize groupTree;
@synthesize limitedLicense;

@synthesize coreDeployment;

@end
