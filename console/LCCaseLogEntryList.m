//
//  LCCaseLogEntryList.m
//  Lithium Console
//
//  Created by James Wilson on 14/08/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCaseLogEntryList.h"

#import "LCCase.h"

@implementation LCCaseLogEntryList

#pragma mark "Initialisation"

- (LCCaseLogEntryList *) init
{
	[super init];
	
	/* Create entry array/dict */
	logEntries = [[NSMutableArray array] retain];
	logEntryDictionary = [[NSMutableDictionary dictionary] retain];
	
	return self;
}

- (void) dealloc 
{
	[lastUpdated release];
	[lastUpdatedShortString release];
	[logEntries release];
	[logEntryDictionary release];
	if (refreshXMLRequest) 
	{ 
		[refreshXMLRequest cancel]; 
		[refreshXMLRequest release]; 
	}
	[super dealloc];
}

#pragma mark "Refresh methods"

- (void) refreshWithPriority:(int)priority
{
	/* Refresh the list of log entries */
	if (!cas || self.refreshInProgress) return;
		
	/* Create request XML document */
	NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"criteria"];
	NSXMLDocument *xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];
	[xmlDoc setVersion:@"1.0"];
	[xmlDoc setCharacterEncoding:@"UTF-8"];
	[rootNode addChild:[NSXMLNode elementWithName:@"caseid"
									  stringValue:[NSString stringWithFormat:@"%i", [(LCCase *)cas caseID]]]];
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:[cas customer]
												  resource:[[cas customer] resourceAddress] 
													entity:[[cas customer] entityAddress] 
												   xmlname:@"case_logentry_list" 
													refsec:0 
													xmlout:xmlDoc] retain];
	[refreshXMLRequest setDelegate:self];
	[refreshXMLRequest setXMLDelegate:self];
	[refreshXMLRequest setPriority:priority];

	[refreshXMLRequest performAsyncRequest];
	self.refreshInProgress = YES;
}
- (void) highPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_HIGH]; }
- (void) normalPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_NORMAL]; }
- (void) lowPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_LOW]; }

- (void) XMLRequestPreParse:(id)sender
{
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	/* Check for start of a case */
	if ([element isEqualToString:@"log_entry"])
	{
		curEntry = [LCCaseLogEntry new];
	}

	[curXMLString release];
	curXMLString = [[NSMutableString string] retain];
}

- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	/* Append characters to curXMLString */
	[curXMLString appendString:string];
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Add element string to dict */
	if (curEntry && curXMLString)
	{ 
		[curEntry setXmlValue:curXMLString forKey:element];
	}
		
	/* Check for end of a case */
	if ([element isEqualToString:@"entryid"])
	{
		/* Check for existing */
		NSString *logID = [NSString stringWithFormat:@"%i", curEntry.entryID];
		if ([logEntryDictionary objectForKey:logID])
		{
			/* Swich to existing */
			[curEntry release];
			curEntry = [logEntryDictionary objectForKey:logID];
		}
		else
		{
			/* Insert new log entry */
			[self insertObject:curEntry inLogEntriesAtIndex:0];
		}		
	}
	if ([element isEqualToString:@"log_entry"])
	{ curEntry = nil; }
	
	[curXMLString release];
	curXMLString = nil;
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Set last update values */
	if ([logEntries count] > 0)
	{
		LCCaseLogEntry *entry = [logEntries objectAtIndex:0];
		self.lastUpdated = entry.timestamp;
		self.lastUpdatedShortString = [self.lastUpdated descriptionWithCalendarFormat:@"%Y-%m-%d %H:%M:%S %z" timeZone:nil locale:nil];
		LCCase *theCase = cas;
		theCase.lastLogEntry = entry;
	}
	else
	{ 
		self.lastUpdated = nil; 
		self.lastUpdatedShortString = nil;
	}
	
	/* Call delegate */
	if (delegate) 
	{
		SEL finishedsel = NSSelectorFromString(@"caseLogEntryListRefreshFinished:");
		if (finishedsel && [delegate respondsToSelector:finishedsel])
		{ [delegate performSelector:finishedsel withObject:self]; }
		else
		{ NSLog (@"Warning: LCCaseLogEntryList delegate does not respond to caseLogEntryListRefreshFinished"); }
	}

	/* Release sender */
	[sender release];
	refreshXMLRequest = nil;

	/* Set flag */
	self.refreshInProgress = NO;
}

#pragma mark "Accessor Methods"
@synthesize delegate;
@synthesize cas;
@synthesize lastUpdated;
@synthesize lastUpdatedShortString;
@synthesize logEntries;
- (void) insertObject:(id)logentry inLogEntriesAtIndex:(unsigned int)index
{ 
	[logEntries insertObject:logentry atIndex:index];
	[logEntryDictionary setObject:logentry forKey:[NSString stringWithFormat:@"%i", ((LCCaseLogEntry *)logentry).entryID]];
}
- (void) removeObjectFromLogEntriesAtIndex:(unsigned int)index
{
	[logEntryDictionary removeObjectForKey:[NSString stringWithFormat:@"%i", ((LCCaseLogEntry *)[logEntries objectAtIndex:index]).entryID]];
	[logEntries removeObjectAtIndex:index];
}
@synthesize logEntryDictionary;
@synthesize refreshInProgress;

@end
