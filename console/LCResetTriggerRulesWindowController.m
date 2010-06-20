//
//  LCResetTriggerRulesWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 28/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCResetTriggerRulesWindowController.h"
#import "LCEntityDescriptor.h"
#import "LCCustomer.h"

@implementation LCResetTriggerRulesWindowController

#pragma mark "Constructor"

- (LCResetTriggerRulesWindowController *) initForEntity:(LCEntity *)initEntity
{
	/* Load NIB */
	[super initWithWindowNibName:@"ResetTriggerRulesWindow"];
	[super window];

	/* Set Entity */
	self.entity = initEntity;
	
	return self; 
}

- (void) dealloc
{
	[entity release];
	[super dealloc];
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Autorelease */
	[controllerAlias setContent:nil];
	[self autorelease];
}

#pragma mark "UI Actions"

- (IBAction) cancelClicked:(id)sender
{
	[NSApp endSheet:[self window]];
	[[self window] close];
}

- (IBAction) resetClicked:(id)sender
{
	/* Update the device */
	NSXMLDocument *xmldoc;
	NSXMLElement *rootnode;
	
	/* Create XML */
	rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"entity_list"];
	xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[(LCEntityDescriptor *)[entity entityDescriptor] xmlNode]];
	
	xmlReq = [[LCXMLRequest requestWithCriteria:[entity customer]
									   resource:[[entity device] resourceAddress]
										 entity:[[entity device] entityAddress]
										xmlname:@"triggerset_reset_rules"
										 refsec:0
										 xmlout:xmldoc] retain];		
	[xmlReq setDelegate:self];
	[xmlReq setXMLDelegate:self];
	[xmlReq setPriority:XMLREQ_PRIO_HIGH];
	[xmlReq performAsyncRequest];
	[self setXmlOperationInProgress:YES];
}

#pragma mark "XML Request Delegate Methods"

- (void) XMLRequestPreParse:(LCXMLRequest *)sender
{
	xmlProperties = [[NSMutableDictionary dictionary] retain]; 
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Clear variables */
	[xmlProperties release];
	xmlProperties = nil;
	
	/* Release request */
	[sender release];
	
	/* Close */
	[NSApp endSheet:[self window]];
	[[self window] close];
	
	/* Call a refresh */
	[[entity device] highPriorityRefresh];
	[[(LCCustomer *)[entity customer] activeIncidentsList] highPriorityRefresh];	
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

#pragma mark "Properties"
@synthesize entity;
@synthesize xmlOperationInProgress;

@end
