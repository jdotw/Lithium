//
//  LCXMLParseOperation.m
//  Lithium Console
//
//  Created by James Wilson on 4/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCXMLParseOperation.h"

@implementation LCXMLParseOperation

- (void) main
{
	/* Create stack */
	nodeStack = [[NSMutableArray array] retain];
	
	/* Parse XML */
	NSXMLParser *parser = [[[NSXMLParser alloc] initWithData:xmlData] autorelease];
	[parser setDelegate:self];
	[parser setShouldResolveExternalEntities:YES];
	[parser parse];

	/* Call delegate on main thread */
	if ([delegate respondsToSelector:@selector(xmlParserDidFinish:)])
	{ 
		/* Must wait for this selector to finish to ensure the 
		 * XMLRequestFinished: selector is sent after the 
		 * parsing/update has been done by out delegate
		 */
		[delegate performSelectorOnMainThread:@selector(xmlParserDidFinish:) withObject:rootNode waitUntilDone:YES];
	}
}

- (void) dealloc
{
	[nodeStack release];
	[xmlData release];
	[delegate release];
	[rootNode release];
	[super dealloc];
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict
{
	/* Create new node */
	LCXMLNode *node = [LCXMLNode new];
	node.name = element;
	
	/* Add to parent */
	LCXMLNode *parent = [nodeStack lastObject];
	if (parent)
	{ [parent.children addObject:node]; }
	else
	{ self.rootNode = node; }
	
	/* Push to stack */
	[nodeStack addObject:node];
	[node autorelease];
}

- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	/* Add to value string */
	LCXMLNode *node = [nodeStack lastObject];
	[node.value appendString:string];
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Pop node off stack */
	LCXMLNode *node = [nodeStack lastObject];
	[nodeStack removeLastObject];
	LCXMLNode *parent = [nodeStack lastObject];

	/* Determine node vs property */
	if ([node.children count] == 0 && [node.properties count] == 0)
	{
		/* Node is actually a property */
		[parent.properties setObject:node.value forKey:node.name];
		[parent.children removeObject:node];
	}
}

@synthesize delegate;
@synthesize xmlData;
@synthesize rootNode;

@end
