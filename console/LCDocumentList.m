//
//  LCDocumentList.m
//  Lithium Console
//
//  Created by James Wilson on 16/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCDocumentList.h"

#import "LCCustomer.h"
#import "LCSSceneDocument.h"
#import "LCVRackDocument.h"

@implementation LCDocumentList

#pragma mark "Constructors"

- (id) initWithCustomer:(id)initCustomer
{
	self = [super init];
	if (!self) return nil;
	
	self.customer = initCustomer;
	documents = [[NSMutableArray array] retain];
	documentDictionary = [[NSMutableDictionary dictionary] retain];
	
	return self;
}

- (void) dealloc
{
	[documents release];
	[documentDictionary release];
	[super dealloc];
}

#pragma mark "Refresh"

- (void) refreshWithPriority:(int)priority
{
	/* Check state */
	if (!customer) return;
	if (refreshXMLRequest) return;
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:customer
												  resource:[(LCCustomer *)customer resourceAddress] 
													entity:[(LCCustomer *)customer entityAddress] 
												   xmlname:@"document_list" 
													refsec:0 
													xmlout:nil] retain];
	[refreshXMLRequest setDelegate:self];
	[refreshXMLRequest setThreadedXmlDelegate:self];
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

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	NSDate *refreshVersion = [NSDate date];
	
	/* Parse received XML */
	for (LCXMLNode *childNode in rootNode.children)
	{ 
		if ([childNode.name isEqualToString:@"document"])
		{
			LCDocument *document = [documentDictionary objectForKey:[childNode.properties objectForKey:@"id"]];
			if (!document)
			{
				/* Create new */
				NSString *type = [childNode.properties objectForKey:@"type"];
				if ([type isEqualToString:@"scene"])
				{
					document = (LCDocument *) [LCSSceneDocument new];
					document.customer = self.customer;
				}
				else if ([type isEqualToString:@"vrack"])
				{
					document = (LCDocument *) [LCVRackDocument new];
					document.customer = self.customer;
				}
				
				/* Set static valyues */
				document.documentID = [[childNode.properties objectForKey:@"id"] intValue];
				document.type = type;
			}
			[document setXmlValuesUsingXmlNode:childNode];
			document.refreshVersion = refreshVersion;
			
			/* Add document if it's new */
			if (![documents containsObject:document])
			{
				/* This is done *after* a full setXmlValues has been called
				 * to ensure that the document is added with all properties set 
				 */
				[self insertObject:document inDocumentsAtIndex:documents.count];				
			}
		}
	}
	
	/* Check for obsolete docments */
	for (LCDocument *document in documents)
	{
		if (!document.refreshVersion || [document.refreshVersion compare:refreshVersion] == NSOrderedAscending)
		{
			/* Document is no longer present */
			[self removeObjectFromDocumentsAtIndex:[documents indexOfObject:document]];
		}
	}
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Free XML request */
	[sender release];
	refreshXMLRequest = nil;
	
	/* Set refresh flag */
	self.refreshInProgress = NO;
}


#pragma mark "Properties"

@synthesize customer;

@synthesize documents;
- (void) insertObject:(LCDocument *)document inDocumentsAtIndex:(unsigned int)index
{
	[documents insertObject:document atIndex:index];
	[documentDictionary setObject:document forKey:[NSString stringWithFormat:@"%i", document.documentID]];
}
- (void) removeObjectFromDocumentsAtIndex:(unsigned int)index;
{
	[documentDictionary removeObjectForKey:[NSString stringWithFormat:@"%i", [(LCDocument *)[documents objectAtIndex:index] documentID]]];
	[documents removeObjectAtIndex:index];
}
@synthesize documentDictionary;

@synthesize refreshInProgress;

@end
