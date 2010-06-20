//
//  LCDocument.m
//  Lithium Console
//
//  Created by James Wilson on 16/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCDocument.h"
#import "LCCustomer.h"

@interface LCDocument (private)

- (void) performXmlAction:(NSString *)xmlName;

@end

@implementation LCDocument

#pragma mark "Constructors"

- (id) init
{
	self = [super init];
	if (!self) return nil;
	
	/* Setup XML Translation */
	if (!self.xmlTranslation)
	{
		self.xmlTranslation = [NSMutableDictionary dictionary];
		[self.xmlTranslation setObject:@"documentID" forKey:@"id"];
		[self.xmlTranslation setObject:@"type" forKey:@"type"];
		[self.xmlTranslation setObject:@"desc" forKey:@"desc"];
		[self.xmlTranslation setObject:@"state" forKey:@"state"];
		[self.xmlTranslation setObject:@"editor" forKey:@"editor"];
		[self.xmlTranslation setObject:@"version" forKey:@"version"];
	}	
	
	return self;
}

- (void) dealloc
{
	[desc release];
	[editor release];
	[super dealloc];
}

- (id) copyDocument
{
	LCDocument *copy = [[self class] new];
	[copy copyXmlPropertiesFromObject:self];
	copy.customer = self.customer;
	return copy;
}

#pragma mark "Browser Tree Properties"

- (BOOL) isBrowserTreeLeaf
{
	return YES;
}	

- (int) opState
{ return -1; }

- (BOOL) refreshInProgress
{ return NO; }

- (BOOL) selectable
{ return YES; }

#pragma mark "XML Operations"

- (void) getDocument
{
	[self performXmlAction:@"document_get"];
}

- (void) performXmlAction:(NSString *)xmlName
{
	/* Check state */
	if (xmlOperationInProgress) return;
	
	/* Create XML */
	NSXMLDocument *xmldoc = [self xmlDocument];
	
	/* Create XML Request */
	LCXMLRequest *xmlReq = [[LCXMLRequest requestWithCriteria:customer
													 resource:[(LCCustomer *)customer resourceAddress] 
													   entity:[(LCCustomer *)customer entityAddress] 
													  xmlname:xmlName
													   refsec:0 
													   xmlout:xmldoc] retain];
	[xmlReq setDelegate:self];
	[xmlReq setThreadedXmlDelegate:self];
	[xmlReq setPriority:XMLREQ_PRIO_HIGH];
	[xmlReq performAsyncRequest];
	self.xmlOperationInProgress = YES;
}

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	/* Update Properties */
	[self setXmlValuesUsingXmlNode:rootNode];
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Free XML request */
	[sender release];
	
	/* Set refresh flag */
	self.xmlOperationInProgress = NO;
}

#pragma mark "Properties"

@synthesize documentID;
@synthesize type;
@synthesize desc;
- (void) setDesc:(NSString *)value
{
	[desc release];
	desc = [value copy];
	self.displayString = desc;
}
@synthesize displayString;
@synthesize state;
@synthesize editor;
@synthesize version; 
@synthesize refreshVersion;
@synthesize customer;
@synthesize editing;

@end
