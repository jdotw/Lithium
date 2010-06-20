//
//  LCReviewActiveTriggersWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 27/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCReviewActiveTriggersWindowController.h"

#import "LCIncident.h"
#import "LCReviewActiveTriggersItem.h"
#import "LCTriggerTuningWindowController.h"
#import "LCCustomer.h"

@implementation LCReviewActiveTriggersWindowController

#pragma mark "Constructors"

- (LCReviewActiveTriggersWindowController *) initForDevice:(LCEntity *)initDevice
{
	/* Load NIB */
	[super initWithWindowNibName:@"ReviewActiveTriggersWindow"];
	[self window];
	
	/* Set device */
	self.device = initDevice;
	
	/* Reset items */
	[self resetItems];
	
	return self;
}

- (void) dealloc
{
	if (xmlReq)
	{
		[xmlReq cancel];
		[xmlReq release];
	}
	[device release];
	[items release];
	[super dealloc];
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Autorelease */
	[controllerAlias setContent:nil];
	[self autorelease];
}

#pragma mark Item Management

- (void) resetItems
{
	[self willChangeValueForKey:@"items"];
	
	[items release];
	items = [[NSMutableArray array] retain];
	
	NSEnumerator *incidentEnum = [[device incidents] objectEnumerator];
	LCIncident *inc;
	while (inc=[incidentEnum nextObject])
	{
		LCReviewActiveTriggersItem *item = [LCReviewActiveTriggersItem itemForEntity:[inc entity]];
		[item setSelected:YES];
		[items addObject:item];
	}
	
	[self didChangeValueForKey:@"items"];
}

#pragma mark UI Actions

- (IBAction) cancelClicked:(id)sender
{
	[NSApp endSheet:[self window]];
	[[self window] close];
}

- (IBAction) toggleAllClicked:(id)sender
{
	LCReviewActiveTriggersItem *item;
	for (item in items)
	{
		[item setSelected:toggleState];
	}
	if (toggleState) toggleState = NO;
	else toggleState = YES;
}

- (IBAction) toggleSelectedClicked:(id)sender
{
	NSEnumerator *itemEnum = [[itemArrayController selectedObjects] objectEnumerator];
	LCReviewActiveTriggersItem *item;
	while (item=[itemEnum nextObject])
	{
		if ([item selected]) [item setSelected:NO];
		else [item setSelected:YES];
	}
}

- (IBAction) tableViewDoubleClicked:(id)sender
{
	if (xmlOperationInProgress) return;
	
	if ([[itemArrayController selectedObjects] count] < 1) return;
	LCReviewActiveTriggersItem *item = [[itemArrayController selectedObjects] objectAtIndex:0];
	
	/* Close us */
	[NSApp endSheet:[self window]];
	[[self window] close];
	
	/* Open trigger tuning */
	LCTriggerTuningWindowController *controller;
	controller = (LCTriggerTuningWindowController *) [[LCTriggerTuningWindowController alloc] initWithObject:[[item entity] object]];
	[NSApp beginSheet:[controller window]
	   modalForWindow:parentWindow
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];	
}

#pragma mark Disable Selected

- (IBAction) disableSelectedClicked:(id)sender
{
	/* Update the device */
	NSXMLDocument *xmldoc;
	NSXMLElement *rootnode;
	
	/* Create XML */
	rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"entity_list"];
	xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	LCReviewActiveTriggersItem *item;
	for (item in items)
	{
		if ([item selected])
		{ [rootnode addChild:[[[item entity] entityDescriptor] xmlNode]]; }
	}

	xmlReq = [[LCXMLRequest requestWithCriteria:[device customer]
									resource:[device resourceAddress]
									  entity:[device entityAddress]
									 xmlname:@"triggerset_disable_by_trigger"
									  refsec:0
									  xmlout:xmldoc] retain];		
	[xmlReq setDelegate:self];
	[xmlReq setXMLDelegate:self];
	[xmlReq setPriority:XMLREQ_PRIO_HIGH];
	[xmlReq performAsyncRequest];
	self.xmlOperationInProgress = YES;
}

#pragma mark "XML Request Delegate Methods"

- (void) XMLRequestPreParse:(LCXMLRequest *)sender
{
	xmlProperties = [[NSMutableDictionary dictionary] retain]; 
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Clear variables */
	self.xmlOperationInProgress = YES;
	[xmlProperties release];
	xmlProperties = nil;
	
	/* Release request */
	if (sender == xmlReq)
	{
		[xmlReq release];
		xmlReq = nil;
	}	
	
	/* Close */
	[NSApp endSheet:[self window]];
	[[self window] close];
	
	/* Call a refresh */
	[device highPriorityRefresh];
	[[(LCCustomer *)[device customer] activeIncidentsList] highPriorityRefresh];
}

#pragma mark "XML Parser Delegate Methods"

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	/* Retain element */
	xmlElement = [element retain];
	
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
	if (xmlElement)
	{
		if (xmlString) { [xmlString appendString:string]; }
		else { xmlString = [[NSMutableString stringWithString:string] retain]; }
	}
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Update properties */
	if (xmlElement && xmlString)
	{ 
		[xmlProperties setObject:xmlString forKey:xmlElement]; 
		[xmlString release];
		xmlString = nil;
	}
	
	/* Release current element */
	if (xmlElement)
	{
		[xmlElement release];
		xmlElement = nil;
	}
}

#pragma mark Accessors

@synthesize device;
@synthesize parentWindow;
@synthesize xmlOperationInProgress;

@end
