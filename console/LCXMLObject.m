//
//  LCXMLObject.m
//  Lithium Console
//
//  Created by James Wilson on 6/10/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCXMLObject.h"

#import "LCCustomer.h"

@implementation LCXMLObject

static NSMutableDictionary *_StaticXmlTranslations = nil;
static NSMutableDictionary *_StaticValueTypes = nil;

#pragma mark "Constructors"

- (LCXMLObject *) init
{
	[super init];
	
	if (!_StaticXmlTranslations) _StaticXmlTranslations = [[NSMutableDictionary dictionary] retain];
	if (!_StaticValueTypes) _StaticValueTypes = [[NSMutableDictionary dictionary] retain];
	
	valueTypeCache = [_StaticValueTypes objectForKey:[self className]];
	if (!valueTypeCache)
	{
		valueTypeCache = [NSMutableDictionary dictionary];
		[_StaticValueTypes setObject:valueTypeCache forKey:[self className]];
	}
	
	return self;
}

- (void) dealloc
{
	[refreshXmlRequest cancel];
	[refreshXmlRequest release];
	[addXmlRequest cancel];
	[addXmlRequest release];
	[updateXmlRequest cancel];
	[updateXmlRequest release];
	[deleteXmlRequest setDelegate:nil];		/* Do not cancel the delete, just make sure we won't get the callback */
	[deleteXmlRequest release];
	[xmlStatusString release];
	[super dealloc];
}

#pragma mark "Copying"

- (id) mutableCopyWithZone:(NSZone *)zone
{
	LCXMLObject *copy = (LCXMLObject *) [[[self class] allocWithZone:zone] init];
	[copy copyXmlPropertiesFromObject:self];
	if ([self respondsToSelector:@selector(customer)])
	{ [copy setValue:[self valueForKey:@"customer"] forKey:@"customer"]; }
	return copy;
}

- (id) copyWithZone:(NSZone *)zone
{
	return [self mutableCopyWithZone:zone];
}

#pragma mark "Sub-Class Methods and Properties (to be overidden)"

- (NSString *) xmlRootElement
{ 
	NSAssert1 (0, @"%@ must implement '- (NSString *) xmlRootElement'", [self className]);		
	return nil; 
}

#pragma mark "XML Property Setting"

+ (NSDictionary *) xmlTranslationForClass:(Class)class
{
	return [_StaticXmlTranslations objectForKey:[class className]];
}

- (void) setXmlValueUsingProperties:(NSMutableDictionary *)valueProperties
{
	[self setXmlValue:[valueProperties objectForKey:@"value"] forKey:[valueProperties objectForKey:@"key"]];
}

- (void) setXmlValuesUsingXmlNode:(LCXMLNode *)node
{
	NSArray *keys = [node.properties allKeys];
	for (NSString *key in keys)
	{ [self setXmlValue:[node.properties objectForKey:key] forKey:key]; }
}

- (void) setXmlValue:(id)value forKey:(NSString *)key
{
	/* Get Translated Key */
	if ([self.xmlTranslation objectForKey:key])
	{ 
		key = [self.xmlTranslation objectForKey:key]; 
	}
	else
	{
		return;
	}
	
	/* Check for invocation cache */
	
	/* Get value */
	NSString *cachedType = [valueTypeCache objectForKey:key];
	if (!cachedType)
	{
		const char *valueType = [[self methodSignatureForSelector:NSSelectorFromString(key)] methodReturnType];
		if (valueType)
		{ cachedType = [NSString stringWithUTF8String:valueType]; }
		else
		{ return; }
		[valueTypeCache setObject:cachedType forKey:key];
	}
	const char *valueType = [cachedType cStringUsingEncoding:NSUTF8StringEncoding];
	if (valueType)
	{
		/* Check type of value */
		switch (valueType[0])
		{
			case 'c':
				value = [NSNumber numberWithInt:[value intValue]];
				break;
			case 'l':
			case 'L':
			case 'Q':
			case 'q':
				value = [NSNumber numberWithLongLong:[value longLongValue]];
				break;
			case 'f':
				value = [NSNumber numberWithFloat:[value floatValue]];
				break;
			case 'i':
			case '@':
				/* No special handling needed */
				if ([value isEqualToString:@"(null)"]) value = nil;
				break;
			default:
				NSLog (@"LCXMLObject:%@ WARNING: Value type %s is not explicity handled, assuming native (%@) will be OK.", [self class], valueType, [value class]);
		}
	}
	else
	{ NSLog (@"LCXMLObject:%@ WARNING: Failed to find valueType for key %@, no value type translation will be done.", [self class], key); }
	
	/* Set value */
	self.xmlUpdatingValues = YES;
	[self setValue:value forKey:key]; 	
	self.xmlUpdatingValues = NO;
}
	
+ (BOOL)accessInstanceVariablesDirectly
{
	return NO;
}

- (void) setValue:(id)value forUndefinedKey:(NSString *)key
{
	NSLog (@"LCXMLObject:%@ ERROR: tried to set value for undefined key %@", self, key);
}

- (void) copyXmlPropertiesToObject:(id)obj
{
	for (NSString *key in [self.xmlTranslation allKeys])
	{
		NSString *propertyName = [self.xmlTranslation objectForKey:key];
		if ([obj respondsToSelector:NSSelectorFromString(propertyName)])
		{ 
			NSLog (@"copyXmlPropertiesToObject:%@ copying %@=%@", obj, propertyName, [self valueForKey:propertyName]);
			[obj setValue:[self valueForKey:propertyName] forKey:propertyName];	
		}
	}
}

- (void) copyXmlPropertiesFromObject:(id)obj
{
	for (NSString *key in [self.xmlTranslation allKeys])
	{ 
		NSString *propertyName = [self.xmlTranslation objectForKey:key];
		if ([obj respondsToSelector:NSSelectorFromString(propertyName)])
		{ 
			NSLog (@"copyXmlPropertiesFromObject:%@ copying %@=%@", obj, propertyName, [obj valueForKey:propertyName]);
			[self setValue:[obj valueForKey:propertyName] forKey:propertyName];	
		}
 	}	
}

- (NSDictionary *) xmlPropertiesDictionary
{
	NSMutableDictionary *dict = [NSMutableDictionary dictionary];
	for (NSString *key in [self.xmlTranslation allKeys])
	{ 
		NSString *propertyName = [self.xmlTranslation objectForKey:key];
		if ([self respondsToSelector:NSSelectorFromString(propertyName)])
		{
			id value = [self valueForKey:propertyName];
			if (value) 
			{
				[dict setObject:value forKey:key];
			}
			else 
			{
				/* Starting from 5.0.10, if there's a nil value encountered, then
				 * a blank string is inserted into the XML properties desc. This ensure
				 * that values that have been erased/cleared are properly communicated
				 * instead of being nil'd and not present in the XML (and hence not changed
				 * when they arrive at Core 
				 */
				[dict setObject:@"" forKey:key];
			}

			NSLog (@"xmlPropertiesDictionary:%@ setting %@=%@ (IGNORESS NULL)", self, key, value);
		}
	}		
	return [[dict copy] autorelease];
}

#pragma mark "XML Document Creation"

- (NSXMLDocument *) xmlDocument
{
	/* Create XML */
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:[self xmlNode]];	
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	return xmldoc;
}

- (NSXMLElement *) xmlNode
{
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:[self xmlRootElement]];
	for (NSString *xmlElement in [self.xmlTranslation allKeys])
	{
		NSLog (@"xmlNode:%@ adding %@=%@", self, xmlEntity, [self valueForKey:[self.xmlTranslation objectForKey:xmlElement]]);
		[rootnode addChild:[NSXMLNode elementWithName:xmlElement
										  stringValue:[self valueForKey:[self.xmlTranslation objectForKey:xmlElement]]]];
	}	
	
	return rootnode;	
}

#pragma mark "XML Operations"

- (void) performXmlRefresh
{
	/* Create and perform request */
	refreshXmlRequest = [[LCXMLRequest requestWithCriteria:((LCEntity *)self.xmlEntity).customer
												  resource:[((LCEntity *)self.xmlEntity) resourceAddress]
													entity:[((LCEntity *)self.xmlEntity) entityAddress]
												   xmlname:[NSString stringWithFormat:@"%@_list", [self xmlRootElement]]
													refsec:0
													xmlout:[self xmlDocument]] retain];
	[refreshXmlRequest setDelegate:self];
	[refreshXmlRequest setThreadedXmlDelegate:self];
	[refreshXmlRequest setPriority:XMLREQ_PRIO_HIGH];
	[refreshXmlRequest setDebug:debug];
	
	[refreshXmlRequest performAsyncRequest];
	
	self.xmlRefreshInProgress = YES;
	self.xmlStatusString = @"Refreshing data from Lithium Core...";
}

- (void) performXmlAdd
{
	/* Create and perform request */
	addXmlRequest = [[LCXMLRequest requestWithCriteria:((LCEntity *)self.xmlEntity).customer
											  resource:[((LCEntity *)self.xmlEntity) resourceAddress]
												entity:[((LCEntity *)self.xmlEntity) entityAddress]
											   xmlname:[NSString stringWithFormat:@"%@_add", [self xmlRootElement]]
												refsec:0
												xmlout:[self xmlDocument]] retain];
	[addXmlRequest setDelegate:self];
	[addXmlRequest setThreadedXmlDelegate:self];
	[addXmlRequest setPriority:XMLREQ_PRIO_HIGH];
	[addXmlRequest setDebug:debug];
	
	[addXmlRequest performAsyncRequest];
	
	self.xmlAddInProgress = YES;
	self.xmlStatusString = @"Sending data to Lithium Core...";
}

- (void) performXmlUpdate
{
	/* Create and perform request */
	updateXmlRequest = [[LCXMLRequest requestWithCriteria:((LCEntity *)self.xmlEntity).customer
												 resource:[((LCEntity *)self.xmlEntity) resourceAddress]
												   entity:[((LCEntity *)self.xmlEntity) entityAddress]
												  xmlname:[NSString stringWithFormat:@"%@_update", [self xmlRootElement]]
												   refsec:0
												   xmlout:[self xmlDocument]] retain];
	[updateXmlRequest setDelegate:self];
	[updateXmlRequest setThreadedXmlDelegate:self];
	[updateXmlRequest setPriority:XMLREQ_PRIO_HIGH];
	[updateXmlRequest setDebug:debug];
	
	[updateXmlRequest performAsyncRequest];

	self.xmlUpdateInProgress = YES;
	self.xmlStatusString = @"Sending data to Lithium Core...";
}

- (void) performXmlDelete
{
	/* Create and perform request */
	deleteXmlRequest = [[LCXMLRequest requestWithCriteria:((LCEntity *)self.xmlEntity).customer
												 resource:[((LCEntity *)self.xmlEntity) resourceAddress]
												   entity:[((LCEntity *)self.xmlEntity) entityAddress]
												  xmlname:[NSString stringWithFormat:@"%@_delete", [self xmlRootElement]]
												   refsec:0
												   xmlout:[self xmlDocument]] retain];
	[deleteXmlRequest setDelegate:self];
	[deleteXmlRequest setPriority:XMLREQ_PRIO_HIGH];
	[deleteXmlRequest setDebug:debug];
	
	[deleteXmlRequest performAsyncRequest];

	self.xmlDeleteInProgress = YES;
	self.xmlStatusString = @"Sending data to Lithium Core...";
}

#pragma mark "XML Parser Delegate Methods"

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	if (rootNode.properties.count > 0)
	{
		[self setXmlValuesUsingXmlNode:rootNode];
	}
	else if (rootNode.children.count == 1)
	{
		LCXMLNode *childNode = [rootNode.children objectAtIndex:0];
		[self setXmlValuesUsingXmlNode:childNode];
	}
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	xmlOperationSuccess = sender.success;
	
	/* Hold on to ourselves, just in case the delegate release */
	[self retain];
	
	/* Set flags */
	if (sender == addXmlRequest)
	{ 
		self.xmlAddInProgress = NO; 
		addXmlRequest = nil;		
		if ([delegate respondsToSelector:@selector(xmlObjectAddFinished:)])
		{ 
			[delegate performSelector:@selector(xmlObjectAddFinished:) withObject:self]; 
		}
	}
	else if (sender == updateXmlRequest) 
	{ 
		self.xmlUpdateInProgress = NO; 
		updateXmlRequest = nil;
		if ([delegate respondsToSelector:@selector(xmlObjectUpdateFinished:)])
		{ [delegate performSelector:@selector(xmlObjectUpdateFinished:) withObject:self]; }
	}
	else if (sender == deleteXmlRequest)
	{ 
		self.xmlDeleteInProgress = NO; 
		deleteXmlRequest = nil;
		if ([delegate respondsToSelector:@selector(xmlObjectDeleteFinished:)])
		{ [delegate performSelector:@selector(xmlObjectDeleteFinished:) withObject:self]; }		
	}
	else if (sender == refreshXmlRequest)
	{
		self.xmlRefreshInProgress = NO;
		refreshXmlRequest = nil;
		if ([delegate respondsToSelector:@selector(xmlObjectRefreshFinished:)])
		{ [delegate performSelector:@selector(xmlObjectRefreshFinished:) withObject:self]; }		
	}
	
	self.xmlStatusString = nil;
	
	/* Release earlier self-hold */
	[self release];
}

#pragma mark "Property Methods"

@synthesize xmlEntity;
- (LCEntity *) xmlEntity
{
	if (xmlEntity) return xmlEntity;
	else return [self valueForKey:@"customer"];
}

- (NSMutableDictionary *) xmlTranslation
{ 
	return [_StaticXmlTranslations objectForKey:[self className]]; 
}

- (void) setXmlTranslation:(NSMutableDictionary *)value
{ 
	[_StaticXmlTranslations setObject:[[value mutableCopy] autorelease] forKey:[self className]];
} 

@synthesize xmlOperationInProgress;
@synthesize xmlOperationSuccess;
@synthesize xmlStatusString;

- (void) updateXmlInProgressFlag
{
	if (!xmlUpdateInProgress && !xmlAddInProgress && !xmlDeleteInProgress && !xmlRefreshInProgress)
	{ 
		self.xmlOperationInProgress = NO; 
	}
	else
	{ 
		self.xmlOperationInProgress = YES; 
	}
}

@synthesize xmlAddInProgress;

- (void) setXmlAddInProgress:(BOOL)value
{
	xmlAddInProgress = value;
	[self updateXmlInProgressFlag]; 
}

@synthesize xmlUpdateInProgress;

- (void) setXmlUpdateInProgress:(BOOL)value
{
	xmlUpdateInProgress = value;
	[self updateXmlInProgressFlag]; 
}

@synthesize xmlDeleteInProgress;

- (void) setXmlDeleteInProgress:(BOOL)value
{
	xmlDeleteInProgress = value;
	[self updateXmlInProgressFlag]; 
}

@synthesize xmlRefreshInProgress;

- (void) setXmlRefreshInProgress:(BOOL)value
{
	xmlRefreshInProgress = value;
	[self updateXmlInProgressFlag]; 
}

@synthesize delegate;

@synthesize xmlUpdatingValues;

@synthesize debug;

@synthesize sortString;

@end
