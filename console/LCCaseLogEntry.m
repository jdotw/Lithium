//
//  LCCaseLogEntry.m
//  Lithium Console
//
//  Created by James Wilson on 25/07/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCaseLogEntry.h"

#import "LCCase.h"

@implementation LCCaseLogEntry

#pragma mark "Initialisation"

- (LCCaseLogEntry *) initWithString:(NSString *)text cas:(id)cas timespent:(time_t)inittimespent
{
	[self init];
	
	self.caseID = ((LCCase *)cas).caseID;
	self.timestampSeconds = 0;
	self.timespentSeconds = inittimespent;
	self.author = [[LCAuthenticator authForCustomer:[cas customer]] username];
	self.entry = text;
	
	return self;
}

- (LCCaseLogEntry *) init
{
	[super init];

	if (!self.xmlTranslation)
	{
		self.xmlTranslation = [NSMutableDictionary dictionary];
		[self.xmlTranslation setObject:@"entryID" forKey:@"entryid"];
		[self.xmlTranslation setObject:@"caseID" forKey:@"caseid"];
		[self.xmlTranslation setObject:@"type" forKey:@"type_num"];
		[self.xmlTranslation setObject:@"author" forKey:@"author"];
		[self.xmlTranslation setObject:@"timestampSeconds" forKey:@"tstamp_sec"];
		[self.xmlTranslation setObject:@"entry" forKey:@"entry"];
	}
	
	return self;
}

- (void) dealloc
{
	[typeString release];
	[author release];
	[timestamp release];
	[entry release];
	[super dealloc];
}

#pragma mark "Recording Methods"

- (void) record:(id)cas
{
	/* Record log entry */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"logentry"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	if (self.entryID > 0)
	{ [rootnode addChild:[NSXMLNode elementWithName:@"entryid" 
										stringValue:[NSString stringWithFormat:@"%i", self.entryID]]]; }
	if (self.caseID > 0)
	{ [rootnode addChild:[NSXMLNode elementWithName:@"caseid" 
										stringValue:[NSString stringWithFormat:@"%i", self.caseID]]]; }
	[rootnode addChild:[NSXMLNode elementWithName:@"type_num" 
									  stringValue:[NSString stringWithFormat:@"%i", self.type]]];
	[rootnode addChild:[NSXMLNode elementWithName:@"timespent_sec" 
									  stringValue:[NSString stringWithFormat:@"%l", self.timespentSeconds]]];
	[rootnode addChild:[NSXMLNode elementWithName:@"entry" 
									  stringValue:self.entry]];
	/* Perform XML request */
	LCXMLRequest *xmlreq = [[LCXMLRequest requestWithCriteria:[cas customer] 
													 resource:[[cas customer] resourceAddress] 
													   entity:[[cas customer] entityAddress] 
													  xmlname:@"case_logentry_insert" 
													   refsec:0 
													   xmlout:xmldoc] retain];
	[xmlreq setDelegate:self];
	[xmlreq setPriority:XMLREQ_PRIO_HIGH];

	[xmlreq performAsyncRequest];
	[self retain];
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* XML Request finished */

	/* Inform delegate */
	if (delegate) 
	{
		SEL finishedsel = NSSelectorFromString(@"logEntryRecordFinished:");
		if (finishedsel && [delegate respondsToSelector:finishedsel])
		{ [delegate performSelector:finishedsel withObject:self]; }
	}

	/* Free XML request */
	[sender release];
	[self release];
}

#pragma mark "Accessor methods"
@synthesize entryID;
@synthesize caseID;
@synthesize type;
@synthesize typeString;
@synthesize author;
- (void) setAuthor:(NSString *)value
{
	[author release];
	author = [value copy];
	[self updateDisplayString];
}
@synthesize timestamp;
- (void) setTimestamp:(NSDate *)value
{
	[timestamp release];
	timestamp = [value copy];
	[self updateDisplayString];
}
@synthesize timestampSeconds;
- (void) setTimestampSeconds:(long)value
{ 
	timestampSeconds = value;
	self.timestamp = [NSDate dateWithTimeIntervalSince1970:(float)self.timestampSeconds];
}
@synthesize timespentSeconds;
@synthesize entry;
- (void) setEntry:(NSString *)value
{
	[entry release];
	entry = [value copy];
	[self updateDisplayString];
}
@synthesize displayString;
- (void) updateDisplayString
{
	self.displayString = [NSString stringWithFormat:@"%@ - %@\n\n%@",
						  [self.timestamp description], self.author, self.entry];	
}
@end
