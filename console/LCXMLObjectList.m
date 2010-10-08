//
//  LCXMLObjectList.m
//  Lithium Console
//
//  Created by James Wilson on 11/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//
//
//	LCXMLObjectList -- Generic XML-based object list
//


#import "LCXMLObjectList.h"
#import "LCXMLObject.h"
#import "LCEntity.h"
#import "LCEntityDescriptor.h"

@implementation LCXMLObjectList

#pragma mark "Constructors"

- (id) initWithSource:(LCEntity *)initSource
			  xmlName:(NSString *)initXmlName
			 forClass:(Class)initClass
			 usingIndexKey:(NSString *)initKey
{
	self = [super init];
	if (!self) return nil;
	
	self.source = initSource;
	self.xmlName = initXmlName;
	self.objectClass = initClass;
	self.indexKey = initKey;
	
	LCXMLObject *translator = [[objectClass new] autorelease];
	for (NSString *key in [translator.xmlTranslation allKeys])
	{
		if ([[translator.xmlTranslation objectForKey:key] isEqualToString:indexKey])
		{ self.xmlIndexKey = key; }
	}
	
	objects = [[NSMutableArray array] retain];
	objectDict = [[NSMutableDictionary dictionary] retain];
	
	return self;
}

- (void) dealloc
{
	if (refreshXMLRequest)
	{
		[refreshXMLRequest cancel];
		[refreshXMLRequest release];
	}
	[source release];
	[xmlName release];
	[indexKey release];
	[xmlIndexKey release];
	[objects release];
	[objectDict release];
	[super dealloc];
}

#pragma mark "XML Document Creation"

- (NSXMLDocument *) criteriaXmlDocument
{
	NSXMLElement *rootNode = [self criteriaXmlNode];
	if (rootNode)
	{
		/* Create XML */
		NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootNode];	
		[xmldoc setVersion:@"1.0"];
		[xmldoc setCharacterEncoding:@"UTF-8"];
		return xmldoc;
	}
	return nil;
}

- (NSXMLElement *) criteriaXmlNode
{
	return nil;
}

#pragma mark "Refresh methods"

- (void) refreshWithPriority:(int)priority
{
	/* Check state */
	if (!source.customer) return;
	if (refreshXMLRequest) 
	{
		/* Request already in progress */
		if (allowCancelOfExistingRefresh)
		{
			[refreshXMLRequest cancel];
			[refreshXMLRequest release];
		}
		else
		{ return; }
	}
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:source.customer
												  resource:[source resourceAddress] 
													entity:[source entityAddress] 
												   xmlname:xmlName
													refsec:0 
													xmlout:[self criteriaXmlDocument]] retain];
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
	id lastObject = nil;
	NSMutableArray *seenObjects = [NSMutableArray array];
	
	/* Loop through each object in the received XML */
	for (LCXMLNode *childNode in rootNode.children)
	{
		if ([childNode.name isEqualToString:@"entity_descriptor"])
		{
			/* Entity Object -- Add Entity Object from Descriptor */
			LCEntityDescriptor *entDesc = [LCEntityDescriptor descriptorWithXmlNode:childNode];
			LCEntity *entity = [entDesc locateEntity:YES];
			if (![objects containsObject:entity])
			{
				if (lastObject)
				{ [self insertObject:entity inObjectsAtIndex:[objects indexOfObject:lastObject]+1]; }
				else
				{ [self insertObject:entity inObjectsAtIndex:objects.count]; }
			}
			[seenObjects addObject:entity];
		}
		else
		{
			/* Standard Object */
			NSString *indexValue = [childNode.properties objectForKey:xmlIndexKey];
			LCXMLObject *object = [objectDict objectForKey:indexValue];
			if (!object)
			{
				/* New Object */
				object = (LCXMLObject *) [objectClass new];
				[object setValue:indexValue forKey:indexKey];
				if (lastObject)
				{ [self insertObject:object inObjectsAtIndex:[objects indexOfObject:lastObject]+1]; }
				else
				{ [self insertObject:object inObjectsAtIndex:objects.count]; }
				[object autorelease];
			}
			[object setXmlValuesUsingXmlNode:childNode];
			[seenObjects addObject:object];
		}
	}
	
	/* Check for Obsolete */
	NSMutableArray *removeArray = [NSMutableArray array];
	for (LCXMLObject *object in objects)
	{
		if (![seenObjects containsObject:object])
		{
			/* Obsolete */
			[removeArray addObject:object];			
		}
	}
	for (LCXMLObject *object in removeArray)
	{
		[self removeObjectFromObjectsAtIndex:[objects indexOfObject:object]];		
	}
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Free XML request */
	if (sender == refreshXMLRequest)
	{
		[refreshXMLRequest release];
		refreshXMLRequest = nil;
	}
	
	/* Set Flag */
	self.refreshInProgress = NO;
}

#pragma mark "Object Properties"

@synthesize objects;
- (void) insertObject:(id)obj inObjectsAtIndex:(unsigned int)index
{
	[objects insertObject:obj atIndex:index];
	if (indexKey) [objectDict setObject:obj forKey:[NSString stringWithFormat:@"%@", [obj valueForKey:indexKey]]];
}
- (void) removeObjectFromObjectsAtIndex:(unsigned int)index;
{
	NSAssert4 (index < objects.count, 
			   @"LCXMLObjectList (%@:%@) ERROR: Index %i not found (count=%i)", 
			   NSStringFromClass(objectClass), indexKey, index, objects.count);

	id obj = [objects objectAtIndex:index];
	if (indexKey) [objectDict removeObjectForKey:[NSString stringWithFormat:@"%@", [obj valueForKey:indexKey]]];
	[objects removeObjectAtIndex:index];
}
@synthesize objectDict;

#pragma mark "Properties"

@synthesize source;
@synthesize xmlName;
@synthesize objectClass;
@synthesize indexKey;
@synthesize xmlIndexKey;
@synthesize refreshInProgress;
@synthesize allowCancelOfExistingRefresh;


@end
