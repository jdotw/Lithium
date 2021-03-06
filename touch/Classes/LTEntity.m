//
//  LTEntity.m
//  Lithium
//
//  Created by James Wilson on 26/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTEntity.h"

#import "LTAuthenticationTableViewController.h"
#import "LithiumAppDelegate.h"
#import "LTCustomer.h"
#import "LCXMLParseOperation.h"

static NSMutableDictionary *_xmlTranslation = nil;

@implementation LTEntity

#pragma mark "Class Methods"

+ (NSMutableDictionary *) xmlTranslation
{
	if (!_xmlTranslation)
	{
		_xmlTranslation = [[NSMutableDictionary dictionary] retain];
		[_xmlTranslation setObject:@"type" forKey:@"type_num"];
		[_xmlTranslation setObject:@"name" forKey:@"name"];
		[_xmlTranslation setObject:@"desc" forKey:@"desc"];
		[_xmlTranslation setObject:@"adminState" forKey:@"adminstate_num"];
		[_xmlTranslation setObject:@"opState" forKey:@"opstate_num"];
		[_xmlTranslation setObject:@"resourceAddress" forKey:@"resaddr"];
		[_xmlTranslation setObject:@"maxValue" forKey:@"max_valstr"];
		[_xmlTranslation setObject:@"refreshInterval" forKey:@"refresh_interval"];
		[_xmlTranslation setObject:@"recordMethod" forKey:@"record_method"];
		[_xmlTranslation setObject:@"recordEnabled" forKey:@"record_enabled"];
		[_xmlTranslation setObject:@"showInSummary" forKey:@"summary_flag"];
		[_xmlTranslation setObject:@"allocUnit" forKey:@"alloc_unit"];
		[_xmlTranslation setObject:@"units" forKey:@"units"];
		[_xmlTranslation setObject:@"kbase" forKey:@"kbase"];
		[_xmlTranslation setObject:@"hasTriggers" forKey:@"has_triggers"];
		[_xmlTranslation setObject:@"version" forKey:@"version"];
		[_xmlTranslation setObject:@"syncVersion" forKey:@"sync_version"];
		[_xmlTranslation setObject:@"uuidString" forKey:@"uuid"];
	}
	return _xmlTranslation;
}

#pragma mark "Constructors"

- (LTEntity *) init
{
	[super init];
	opState = -1;
	children = [[NSMutableArray array] retain];
	childDict = [[NSMutableDictionary dictionary] retain];
	refreshInterval = 60.0f; /* Default */
	return self;
}

- (void) dealloc
{
	[name release];
	[desc release];
	[resourceAddress release];
	[children release];
	[childDict release];
	[xmlStatus release];
	[super dealloc];
}

#pragma mark "URL Methods"

- (NSString *) urlPrefix
{
	NSString *protocol = self.customer.coreDeployment.useSSL ? @"https" : @"http";
	NSString *port =  self.customer.coreDeployment.useSSL ? @"51143" : @"51180";
	return [NSString stringWithFormat:@"%@://%@:%@/%@", protocol, self.customer.ipAddress, port, self.customer.name];
}

- (NSURL *) urlForXml:(NSString *)xmlName timestamp:(int)urlRefSec
{
	NSString *urlString = [NSString stringWithFormat:@"%@/xml.php?action=xml_get&resaddr=%@&entaddr=%@&xmlname=%@&refsec=%i", 
						   [self urlPrefix], self.resourceAddress, self.entityAddress, xmlName, urlRefSec];
	return [NSURL URLWithString:urlString];
	
}

#pragma mark "Refresh (Entity Tree)"

- (void) refresh
{
	/* Refresh the entity and tree below */

	/* Check state */
	if (refreshInProgress || ![(LTCoreDeployment *)coreDeployment enabled])
	{
		return;
	}
	if (self.type == 2 && parent)
	{
		[parent refresh];
		return;
	}
	if (lastRefresh && [[NSDate date] timeIntervalSinceDate:lastRefresh] < (self.refreshInterval / 2.0f))
	{
		return;
	}

	/* Create outbound XML */
	NSMutableString *xmlString;
	xmlString = [NSMutableString stringWithFormat:@"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"];
	[xmlString appendString:@"<tree_request>"];
	[xmlString appendFormat:@"<sync_version>%i</sync_version>", self.syncVersion];
	[xmlString appendString:@"</tree_request>"];	
	
	/* Create request */
	NSURL *url;
	if (self.type == 1)
	{
		url = [self urlForXml:@"entity_tree_authorative" timestamp:0];
	}
	else
	{
		url = [self urlForXml:@"entity_tree_one_level" timestamp:0];
	}
	NSMutableURLRequest *theRequest= [NSMutableURLRequest requestWithURL:url
															 cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
														 timeoutInterval:60.0];

	/* Outbound XML doc to be sent */
	NSString *formBoundary = [[NSProcessInfo processInfo] globallyUniqueString];
	NSString *boundaryString = [NSString stringWithFormat: @"multipart/form-data; boundary=%@", formBoundary];
	[theRequest addValue:boundaryString forHTTPHeaderField:@"Content-Type"];
	[theRequest setHTTPMethod: @"POST"];
	NSMutableData *postData = [NSMutableData data];
	[postData appendData:[[NSString stringWithFormat:@"--%@\r\n", formBoundary] dataUsingEncoding:NSUTF8StringEncoding]];
	[postData appendData:[@"Content-Disposition: form-data; name=\"xmlfile\"; filename=\"ltouch.xml\"\r\n" dataUsingEncoding:NSUTF8StringEncoding]];
	[postData appendData:[@"Content-Type: text/plain\r\n\r\n" dataUsingEncoding:NSUTF8StringEncoding]];
	[xmlString replaceOccurrencesOfString:@"\r\n" withString:@"&#xD;&#xA;" options:NSCaseInsensitiveSearch range:NSMakeRange(0,[xmlString length])];
	[xmlString replaceOccurrencesOfString:@"\n" withString:@"&#xD;&#xA;" options:NSCaseInsensitiveSearch range:NSMakeRange(0,[xmlString length])];
	[postData appendData:[xmlString dataUsingEncoding:NSUTF8StringEncoding]];
	[postData appendData:[[NSString stringWithFormat:@"\r\n--%@--\r\n", formBoundary] dataUsingEncoding:NSUTF8StringEncoding]];
	[theRequest setHTTPBody:postData];
	
	/* Begin download */
	NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
	if (theConnection) 
	{
		self.refreshInProgress = YES;
		receivedData=[[NSMutableData data] retain];
		self.xmlStatus = @"Connecting to Lithium Core...";
		[[NSNotificationCenter defaultCenter] postNotificationName:@"LTEntityXmlStatusChanged" object:self];
	} 
	else 
	{
		/* FIX */
		NSLog (@"ERROR: Failed to download console.php");
		self.xmlStatus = @"Unable to connect to Lithium Core...";
		[[NSNotificationCenter defaultCenter] postNotificationName:@"LTEntityXmlStatusChanged" object:self];
	}	
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
	[super connection:connection didReceiveResponse:response];
	self.xmlStatus = @"Received initial response...";
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTEntityXmlStatusChanged" object:self];
}

-(void) connection:(NSURLConnection *)connection didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *) challenge
{
	self.xmlStatus = @"Authenticating...";
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTEntityXmlStatusChanged" object:self];
	
	[super connection:connection didReceiveAuthenticationChallenge:challenge];
}		

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
	self.xmlStatus = @"Receiving data...";
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTEntityXmlStatusChanged" object:self];

	[super connection:connection didReceiveData:data];
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	/* FIX */
    NSLog(@"ERROR: Connection failed! Error - %@ %@",
          [error localizedDescription],
          [[error userInfo] objectForKey:NSErrorFailingURLStringKey]);
	
	self.xmlStatus = @"Failed to download data.";
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTEntityXmlStatusChanged" object:self];

	[super connection:connection didFailWithError:error];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection

{
	/* Set flags */
	self.lastRefresh = [NSDate date];
	self.xmlStatus = @"Download Completed.";
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTEntityXmlStatusChanged" object:self];

	/* Create entity stack */
//	entityStack = [[NSMutableArray array] retain];
	
	/* Parse XML */
	LithiumAppDelegate *appDelegate = (LithiumAppDelegate *) [[UIApplication sharedApplication] delegate];
	LCXMLParseOperation *xmlParser = [[LCXMLParseOperation new] autorelease];
	xmlParser.xmlData = receivedData;
	xmlParser.delegate = self;
	[appDelegate.operationQueue addOperation:xmlParser];
	self.xmlStatus = @"Processing Data...";
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTEntityXmlStatusChanged" object:self];
	
	/* Cleanup */
	[connection release];
}

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	/* Check Thread */
	if ([NSThread currentThread] != [NSThread mainThread])
	{
		[NSException raise:@"LTEntity-parserDidFinish-IncorrectThread"
					format:@"An instance of LTEntity received a message to parserDidFinish on a thread that was NOT that main thread"];
	}
	
	/* Interpret */
	for (LCXMLNode *childNode in rootNode.children)
	{ 
		[self updateEntityUsingXMLNode:childNode]; 
	}	
	
	/* Clean-up */
    [receivedData release];
	
	/* Post Notification */
	self.refreshInProgress = NO;
	[[NSNotificationCenter defaultCenter] postNotificationName:@"RefreshFinished" object:self];
	
	/* Update Status */
	self.xmlStatus = @"Done.";
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTEntityXmlStatusChanged" object:self];
}

- (void) updateEntityUsingXMLNode:(LCXMLNode *)node
{
	/* Update local properties */
	for (NSString *key in [node.properties allKeys])
	{
		[self setXmlValue:[node.properties objectForKey:key] forKey:key];
	}
	
	/* Recursively update */
	BOOL currentValueSet = NO;
	for (LCXMLNode *childNode in node.children)
	{
		if ([childNode.name isEqualToString:@"entity"])
		{
			/* Locate/Create Child */
			LTEntity *childEntity = [childDict objectForKey:[childNode.properties objectForKey:@"name"]];
			if (!childEntity)
			{
				childEntity = [[LTEntity new] autorelease];
				childEntity.parent = self;
			}
			
			/* Clear all current values */
			[childEntity.values removeAllObjects];
			
			/* Perform update */
			[childEntity updateEntityUsingXMLNode:childNode];
			
			/* Add new child if necessary */
			if (![childDict objectForKey:childEntity.name])
			{
				[children addObject:childEntity];
				[childDict setObject:childEntity forKey:childEntity.name];
				
				if (childEntity.type == 3)
				{ 
					LithiumAppDelegate *appDelegate = (LithiumAppDelegate *) [[UIApplication sharedApplication] delegate];
					[appDelegate.favoritesController bindFavoritesFromDevice:childEntity];
				}					
			}
		}
		else if ([childNode.name isEqualToString:@"value"])
		{
			/* Create Value */
			LTMetricValue *curValue = [[[LTMetricValue alloc] init] autorelease];
			curValue.stringValue = [childNode.properties objectForKey:@"valstr"];
			curValue.floatValue = [[childNode.properties objectForKey:@"valstr_raw"] floatValue];
			curValue.timestamp = [NSDate dateWithTimeIntervalSince1970:[[childNode.properties objectForKey:@"tstamp_sec"] floatValue]];

			/* Set entity current value */
			if (!currentValueSet)
			{
				self.currentValue = [childNode.properties objectForKey:@"valstr"];
				currentValueSet = YES;
			}
			
			/* Enqueue */
			if (!self.values)
			{ values = [[NSMutableArray array] retain]; }
			[values addObject:curValue];
			
		}
	}
}

#pragma mark "XML Property Setting"

- (NSMutableDictionary *) xmlTranslation
{ return [LTEntity xmlTranslation]; }

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
	
	/* Get value */
	const char *valueType = [[self methodSignatureForSelector:NSSelectorFromString(key)] methodReturnType];
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
				value = [NSNumber numberWithLongLong:[value longLongValue]];
				break;
			case 'f':
				value = [NSNumber numberWithFloat:[value floatValue]];
				break;
			case 'd':
				value = [NSNumber numberWithDouble:[value doubleValue]];
				break;
			case 'i':
			case '@':
				/* No special handling needed */
				break;
			default:
				NSLog (@"LTEntity:%@ WARNING: Value type %s for %@ is not explicity handled, assuming native (%@) will be OK.", [self class], valueType, key, [value class]);
		}
	}
	else
	{ NSLog (@"LTEntity:%@ WARNING: Failed to find valueType for key %@, no value type translation will be done.", [self class], key); }
	
	/* Set value */
	[self setValue:value forKey:key]; 	
}

+ (BOOL)accessInstanceVariablesDirectly
{
	return NO;
}

- (void) setValue:(id)value forUndefinedKey:(NSString *)key
{
	NSLog (@"LCXMLObject:%@ ERROR: tried to set value for undefined key %@", [self class], key);
}

#pragma mark "Properties"

@synthesize type;
@synthesize name;
@synthesize desc;
@synthesize resourceAddress;
- (NSString *) resourceAddress
{
	if (resourceAddress) return resourceAddress;
	else
	{
		LTEntity *entityParent = self.parent;
		while (entityParent)
		{
			if (entityParent.resourceAddress) return entityParent.resourceAddress;
			else entityParent = entityParent.parent;
		}
		return nil;
	}
}
@synthesize entityAddress;
- (NSString *) entityAddress
{
	if (!entityAddress)
	{
		NSMutableArray *addressStack = [NSMutableArray array];
		LTEntity *entity = self;
		while (entity.type > 0)
		{
			[addressStack insertObject:entity atIndex:0];
			entity = entity.parent;
		}

		NSMutableString *dynamicEntityAddress = [NSMutableString stringWithFormat:@"%i", self.type];
		for (LTEntity *stackEntity in addressStack)
		{
			[dynamicEntityAddress appendFormat:@":%@", stackEntity.name];
		}

		return dynamicEntityAddress;
	}
	else
	{ return entityAddress; }
}

- (NSString *) deviceEntityAddress
{
	/* Returns the entity address for the device
	 * entity parent of self
	 */
	
	NSArray *parts = [self.entityAddress componentsSeparatedByString:@":"];
	if (parts.count < 4) return NULL;
	else return [NSString stringWithFormat:@"3:%@:%@:%@", [parts objectAtIndex:1], [parts objectAtIndex:2], [parts objectAtIndex:3]];
}

@synthesize entityDescriptor;
- (id) entityDescriptor
{
	if (!entityDescriptor)
	{ 
		return [[[LTEntityDescriptor alloc] initWithEntity:self] autorelease]; 
	}
	else
	{ 
		return entityDescriptor; 
	}
}
@synthesize ipAddress;
@synthesize customerName;
@synthesize username;
@synthesize password;
@synthesize children;
@synthesize childDict;
@synthesize parent;
- (void) setParent:(LTEntity *)value
{
	parent = value;
	self.ipAddress = parent.ipAddress;
	self.customerName = parent.customerName;
	self.username = parent.username;
	self.password = parent.password;
	self.customer = parent.customer;
	self.coreDeployment = parent.coreDeployment;
}
- (LTEntity *) device
{
	LTEntity *entity = self;
	if (entity.type >= 3)
	{
		while (entity.type > 3)
		{ 
			entity = entity.parent; 
		}
		return entity;
	}
	else
	{ return nil; }
}
@synthesize adminState;
@synthesize opState;
@synthesize currentValue;
@synthesize maxValue;
@synthesize hasBeenRefreshed;
@synthesize coreDeployment;
@synthesize refreshInterval;
@synthesize recordMethod;
@synthesize recordEnabled;
@synthesize units;
@synthesize kbase;
@synthesize allocUnit;
@synthesize lastRefresh;
@synthesize values;
@synthesize hasTriggers;
@synthesize showInSummary;
@synthesize version;
@synthesize syncVersion;
@synthesize uuidString;

@synthesize indentLevel;

@synthesize xmlStatus;

- (NSString *) longDisplayString
{
	LTEntityDescriptor *entDesc = self.entityDescriptor;
	switch (self.type)
	{
		case 1:
			return [NSString stringWithFormat:@"%@", entDesc.custDesc];
		case 2:
			return [NSString stringWithFormat:@"%@", entDesc.custDesc, entDesc.siteDesc];
		case 3:
			return [NSString stringWithFormat:@"%@ at %@", entDesc.devDesc, entDesc.siteDesc];
		case 4:
			return [NSString stringWithFormat:@"%@", entDesc.cntDesc];
		case 5:
			return [NSString stringWithFormat:@"%@", entDesc.objDesc];
		case 6:
			return [NSString stringWithFormat:@"%@ %@", entDesc.objDesc, entDesc.metDesc];
		default:
			return self.desc;
	}
}

- (NSString *) longLocationString
{
	LTEntityDescriptor *entDesc = self.entityDescriptor;
	if (self.type <= 3)	return @"";
	else return [NSString stringWithFormat:@"%@ %@ at %@ (%@)", entDesc.devDesc, entDesc.cntDesc, entDesc.siteDesc, entDesc.custDesc];
}

@end
