//
//  LCCase.m
//  Lithium Console
//
//  Created by James Wilson on 25/07/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCase.h"
#import "LCCaseLogEntry.h"
#import "LCEntityDescriptor.h"
#import "LCCaseSmallViewController.h"

@implementation LCCase

#pragma mark "Initialisation"

- (LCCase *) init
{
	[super init];
	
	entityList = [LCCaseEntityList new];
	entityList.cas = self;
	logEntryList = [LCCaseLogEntryList new];
	logEntryList.cas = self;
	
	if (!self.xmlTranslation)
	{
		self.xmlTranslation = [NSMutableDictionary dictionary];
		[self.xmlTranslation setObject:@"caseID" forKey:@"id"];
		[self.xmlTranslation setObject:@"stateString" forKey:@"state"];
		[self.xmlTranslation setObject:@"state" forKey:@"state_int"];
		[self.xmlTranslation setObject:@"headline" forKey:@"hline"];
		[self.xmlTranslation setObject:@"owner" forKey:@"owner"];
		[self.xmlTranslation setObject:@"requester" forKey:@"requester"];
		[self.xmlTranslation setObject:@"openDateSeconds" forKey:@"start_sec"];
		[self.xmlTranslation setObject:@"closeDateSeconds" forKey:@"end_sec"];
	}	

	return self;
}

- (void) dealloc
{
	if (entityListRefreshTimer) [entityListRefreshTimer invalidate];
	if (logEntryListRefreshTimer) [logEntryListRefreshTimer invalidate];
	[customer release];
	[stateString release];
	[headline release];
	[requester release];
	[owner release];
	[openDate release];
	[closeDate release];
	[openDateShortString release];
	[closeDateShortString release];
	[fullIDString release];
	[entityList release];
	[logEntryList release];
	[super dealloc];
}

#pragma mark "Case Manipulation Methods"

- (void) openCaseWithInitialLogEntry:(NSString *)logentry delegate:(id)aDelegate
{
	/* Set delegate */
	openDelegate = aDelegate;

	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"case"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"hline" stringValue:self.headline]];
	[rootnode addChild:[NSXMLNode elementWithName:@"requester" stringValue:self.requester]];
	[rootnode addChild:[NSXMLNode elementWithName:@"owner" stringValue:self.owner]];
	[rootnode addChild:[NSXMLNode elementWithName:@"logentry" stringValue:logentry]];
	
	/* Add entities to XML */
	for (LCEntity *ent in entityList.entities)
	{ [rootnode addChild:[[ent entityDescriptor] xmlNode]];	}
	
	/* Create and perform request */
	openXMLReq = [[LCXMLRequest requestWithCriteria:customer
											resource:[customer resourceAddress]
											  entity:[customer entityAddress]
											 xmlname:@"case_open"
											  refsec:0
											  xmlout:xmldoc] retain];
	[openXMLReq setDelegate:self];
	[openXMLReq setXMLDelegate:self];
	[openXMLReq setPriority:XMLREQ_PRIO_HIGH];

	[openXMLReq performAsyncRequest];
}

- (void) closeCaseWithFinalLogEntry:(NSString *)logentry delegate:(id)aDelegate
{
	/* Set delegate */
	closeDelegate = aDelegate;
	
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"case"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"caseid" stringValue:[NSString stringWithFormat:@"%i", self.caseID]]];
	[rootnode addChild:[NSXMLNode elementWithName:@"logentry" stringValue:logentry]];
	
	/* Create and perform request */
	closeXMLReq = [[LCXMLRequest requestWithCriteria:customer
											resource:[customer resourceAddress]
											  entity:[customer entityAddress]
											 xmlname:@"case_close"
											  refsec:0
											  xmlout:xmldoc] retain];
	[closeXMLReq setDelegate:self];
	[closeXMLReq setXMLDelegate:self];
	[closeXMLReq setPriority:XMLREQ_PRIO_HIGH];
	
	[closeXMLReq performAsyncRequest];
}

- (void) updateCase
{
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"case"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"caseid" stringValue:[NSString stringWithFormat:@"%i", self.caseID]]];
	[rootnode addChild:[NSXMLNode elementWithName:@"hline" stringValue:self.headline]];
	[rootnode addChild:[NSXMLNode elementWithName:@"owner" stringValue:self.owner]];
	
	/* Create and perform request */
	updateXMLReq = [[LCXMLRequest requestWithCriteria:customer
											resource:[customer resourceAddress]
											  entity:[customer entityAddress]
											 xmlname:@"case_update"
											  refsec:0
											  xmlout:xmldoc] retain];
	[updateXMLReq setDelegate:self];
	[updateXMLReq setXMLDelegate:self];
	[updateXMLReq setPriority:XMLREQ_PRIO_HIGH];
	
	[updateXMLReq performAsyncRequest];	
}

- (void) reOpenCaseWithLogEntry:(NSString *)logentry delegate:(id)aDelegate
{
	/* Set delegate */
	reOpenDelegate = aDelegate;
	
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"case"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"caseid" stringValue:[NSString stringWithFormat:@"%i", self.caseID]]];
	[rootnode addChild:[NSXMLNode elementWithName:@"logentry" stringValue:logentry]];
	
	/* Create and perform request */
	reOpenXMLReq = [[LCXMLRequest requestWithCriteria:customer
											resource:[customer resourceAddress]
											  entity:[customer entityAddress]
											 xmlname:@"case_reopen"
											  refsec:0
											  xmlout:xmldoc] retain];
	[reOpenXMLReq setDelegate:self];
	[reOpenXMLReq setXMLDelegate:self];
	[reOpenXMLReq setPriority:XMLREQ_PRIO_HIGH];
	
	[reOpenXMLReq performAsyncRequest];
}


#pragma mark "XML Request Delegate Methods"

- (void) XMLRequestPreParse:(LCXMLRequest *)sender
{
	
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	if (sender == openXMLReq)
	{
		/* Open op finished, the case is now open */
		if (openDelegate) 
		{
			SEL finishedsel = NSSelectorFromString(@"caseOpenFinished:");
			if (finishedsel && [openDelegate respondsToSelector:finishedsel])
			{ [openDelegate performSelector:finishedsel withObject:self]; }
		}
		
		/* Add to customer's openCaseList */
		[[customer openCasesList] insertObject:self inCasesAtIndex:0];
		
		openXMLReq = nil;
	}
	else if (sender == closeXMLReq)
	{
		/* Close op finished, the case is now closed */
		if (closeDelegate) 
		{
			SEL finishedsel = NSSelectorFromString(@"caseClosedFinished:");
			if (finishedsel && [closeDelegate respondsToSelector:finishedsel])
			{ [closeDelegate performSelector:finishedsel withObject:self]; }
		}
		
		/* Remove from customer's openCaseList */
		if ([[[customer openCasesList] cases] indexOfObject:self] != NSNotFound)
		{ [[customer openCasesList] removeObjectFromCasesAtIndex:[[[customer openCasesList] cases] indexOfObject:self]]; }
		
		closeXMLReq = nil;
	}
	else if (sender == reOpenXMLReq)
	{
		/* Re-open finished, the case is now open */
		if (reOpenDelegate) 
		{
			SEL finishedsel = NSSelectorFromString(@"reOpenCaseFinished:");
			if (finishedsel && [reOpenDelegate respondsToSelector:finishedsel])
			{ [reOpenDelegate performSelector:finishedsel withObject:self]; }
		}
		
		/* Add to customer's openCaseList */
		if ([[[customer openCasesList] cases] containsObject:self] == NO)
		{ [[customer openCasesList] insertObject:self inCasesAtIndex:0]; }
		
		closeXMLReq = nil;
	}
	else if (sender == updateXMLReq)
	{
		reOpenXMLReq = nil;
	}
	
	/* Release request */
	[sender release];
}

#pragma mark "XML Parser Delegate Methods"

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	xmlString = [[NSMutableString string] retain];
}

- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	[xmlString appendString:string];
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Update properties */
	if (xmlString)
	{ [self setXmlValue:xmlString forKey:element]; }

	[xmlString release];
	xmlString = nil;
}

#pragma mark "Refresh Methods"

- (void) setEntityListAutoRefresh:(BOOL)flag
{
	if (flag == YES && !entityListRefreshTimer)
	{
		entityListRefreshTimer = [NSTimer scheduledTimerWithTimeInterval:300.0 
																  target:entityList 
																selector:@selector(lowPriorityRefresh)
																userInfo:nil 
																 repeats:YES];
		entityListAutoRefresh = YES;
	}
	if (flag == NO && entityListRefreshTimer)
	{
		[entityListRefreshTimer invalidate];
		entityListRefreshTimer = nil;
		entityListAutoRefresh = NO;
	}
}

- (BOOL) entityListAutoRefresh { return entityListAutoRefresh; }
- (NSTimer *) entityListRefreshTimer { return entityListRefreshTimer; }

- (void) setLogEntryListAutoRefresh:(BOOL)flag
{
	if (flag == YES && !logEntryListRefreshTimer)
	{
		logEntryListRefreshTimer = [NSTimer scheduledTimerWithTimeInterval:60.0 
																  target:logEntryList 
																selector:@selector(lowPriorityRefresh)
																userInfo:nil 
																 repeats:YES];
		logEntryListAutoRefresh = YES;
	}
	if (flag == NO && logEntryListRefreshTimer)
	{
		[logEntryListRefreshTimer invalidate];
		logEntryListRefreshTimer = nil;
		logEntryListAutoRefresh = NO;
	}
}

- (BOOL) logEntryListAutoRefresh { return logEntryListAutoRefresh; }
- (NSTimer *) logEntryListRefreshTimer { return logEntryListRefreshTimer; }

#pragma mark "View Controller Methods"

- (id) viewController
{ return [LCCaseSmallViewController controllerForCase:self]; }

#pragma mark "Accessor Methods"

@synthesize customer;
@synthesize entityList;
@synthesize logEntryList;
@synthesize lastLogEntry;
@synthesize caseID;
- (void) setCaseID:(unsigned long)value
{
	caseID = value;
	self.fullIDString = [NSString stringWithFormat:@"%@%i", [[customer name] uppercaseString], self.caseID];
}
@synthesize state;
@synthesize stateString;
@synthesize headline;
@synthesize requester;
@synthesize owner;
@synthesize openDateSeconds;
- (void) setOpenDateSeconds:(long)value
{
	openDateSeconds = value;
	self.openDate = [NSDate dateWithTimeIntervalSince1970:(float)self.openDateSeconds];
	self.openDateShortString = [self.openDate descriptionWithCalendarFormat:@"%Y-%m-%d %H:%M:%S %z" timeZone:nil locale:nil];
}
@synthesize openDate;
@synthesize closeDateSeconds;
- (void) setCloseDateSeconds:(long)value
{
	closeDateSeconds = value;
	self.closeDate = [NSDate dateWithTimeIntervalSince1970:(float)self.closeDateSeconds]; 
	self.closeDateShortString = [self.closeDate descriptionWithCalendarFormat:@"%Y-%m-%d %H:%M:%S %z" timeZone:nil locale:nil]; 
	self.isClosed = YES;
}
@synthesize closeDate;
@synthesize openDateShortString;
@synthesize closeDateShortString;
@synthesize isClosed;
@synthesize fullIDString;

- (NSImage *) smallIcon
{ return [NSImage imageNamed:@"folder_16.tif"]; }

@synthesize entityListRefreshTimer;
@synthesize logEntryListRefreshTimer;



@end
