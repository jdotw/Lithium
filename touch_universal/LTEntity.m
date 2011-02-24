//
//  LTEntity.m
//  Lithium
//
//  Created by James Wilson on 26/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTEntity.h"

#import "LTAuthenticationTableViewController.h"
#import "AppDelegate.h"
#import "LTCustomer.h"
#import "LTCoreDeployment.h"
#import "LTFavoritesTableViewController.h"
#import "TBXML-Lithium.h"
#import "LTTriggerSet.h"
#import "LTTrigger.h"

static NSMutableDictionary *_xmlTranslation = nil;

@interface LTEntity (Privaye)
- (void) setXmlValueUsingNode:(TBXMLElement *)node forKey:(NSString *)key;
@end

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

		[_xmlTranslation setObject:@"deviceIpAddress" forKey:@"ip"];
		[_xmlTranslation setObject:@"lomIpAddress" forKey:@"lom_ip"];
		[_xmlTranslation setObject:@"snmpVersion" forKey:@"snmpversion"];
		[_xmlTranslation setObject:@"snmpCommunity" forKey:@"snmpcomm"];
		[_xmlTranslation setObject:@"snmpAuthMethod" forKey:@"snmpauthmethod"];
		[_xmlTranslation setObject:@"snmpAuthPassword" forKey:@"snmpauthpassword"];
		[_xmlTranslation setObject:@"snmpPrivacyMethod" forKey:@"snmpprivenc"];
		[_xmlTranslation setObject:@"snmpPrivacyPassword" forKey:@"snmpprivpassword"];
		[_xmlTranslation setObject:@"deviceUsername" forKey:@"username"];
		[_xmlTranslation setObject:@"devicePassword" forKey:@"password"];
		[_xmlTranslation setObject:@"lomUsername" forKey:@"lom_username"];
		[_xmlTranslation setObject:@"lomPassword" forKey:@"lom_password"];
		[_xmlTranslation setObject:@"vendorModule" forKey:@"vendor"];
		[_xmlTranslation setObject:@"deviceProtocol" forKey:@"protocol"];
		[_xmlTranslation setObject:@"useICMP" forKey:@"icmp"];
		[_xmlTranslation setObject:@"useProcessList" forKey:@"swrun"];
		[_xmlTranslation setObject:@"useLOM" forKey:@"lom"];
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
    [currentValue release];
    [values release];
    [maxValue release];
    [units release];
    [uuidString release];
    [deviceIpAddress release];
    [lomIpAddress release];
    [snmpCommunity release];
    [snmpAuthPassword release];
    [snmpPrivacyPassword release];
    [deviceUsername release];
    [devicePassword release];
    [lomUsername release];
    [lomPassword release];
    [vendorModule release];
	[resourceAddress release];
    [entityAddress release];
    [entityDescriptor release];
    
	[children release];
	[childDict release];
    
    [lastRefresh release];
	[xmlStatus release];
    
    [groupParent release];
    
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

- (void) _refresh
{
	/* 
     * Refresh the entity and tree below 
     */

	/* Check state */
	if (refreshInProgress || ![(LTCoreDeployment *)coreDeployment enabled])
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
		/* Always load authorative tree for iPad */
        url = [self urlForXml:@"entity_tree_authorative_mobile" timestamp:0]; 
	}
	else if (self.type == 3 && UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		/* iPad Loading Device, use Mobile Summary */
		url = [self urlForXml:@"entity_tree_summary_mobile" timestamp:0];
	}
	else
	{ 
		url = [self urlForXml:@"entity_tree_one_level_mobile" timestamp:0];
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

- (void) refresh
{
    /* Only performs a refresh if there is likely to be
     * new data available from Core
     */
    if (!lastRefresh || [[NSDate date] timeIntervalSinceDate:lastRefresh] > (self.refreshInterval / 2.0f))
	{
		[self _refresh];
	}
}

- (void) forceRefresh
{
    /* Performs a refresh regardless */
    [self _refresh];
}

- (void) postXmlToResource:(NSString *)destResourceAddress 
			 entityAddress:(NSString *)destEntityAddress 
				   xmlName:(NSString *)xmlName 
					   xml:(NSString *)xml
{
	/* Posts XML to the given resource and assumes that
	 * the returned XML will be parsed like a refresh
	 */
	
	/* Check state */
	if (refreshInProgress || ![(LTCoreDeployment *)coreDeployment enabled])
	{
		return;
	}
	
	/* Create request */
	NSString *urlString = [NSString stringWithFormat:@"%@/xml.php?action=xml_get&resaddr=%@&entaddr=%@&xmlname=%@&refsec=%i", 
						   [self urlPrefix], destResourceAddress ? : self.resourceAddress, 
						   destEntityAddress ? : self.entityAddress, xmlName, 0];
	NSMutableURLRequest *theRequest= [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
															 cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
														 timeoutInterval:60.0];
	
	/* Outbound XML doc to be sent */
	NSMutableString *xmlString = [[xml mutableCopy] autorelease];
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
	/* Set flags */
	self.lastRefresh = [NSDate date];
	self.xmlStatus = @"Failed to download data.";
    self.lastRefreshFailed = YES;
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTEntityXmlStatusChanged" object:self];

	/* Post Notification */
	self.refreshInProgress = NO;
	self.hasBeenRefreshed = YES;
	[[NSNotificationCenter defaultCenter] postNotificationName:@"RefreshFinished" object:self];	

    /* Call super to clean up and post any failure alerts */
	[super connection:connection didFailWithError:error];

    /* Local clean up */
    [connection release];   // Our connection is alloc/init'ed locally 
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection

{
	/* Set flags */
	self.lastRefresh = [NSDate date];
	self.xmlStatus = @"Download Completed.";
    self.lastRefreshFailed = NO;
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTEntityXmlStatusChanged" object:self];

	/* DEBUG */
//	[receivedData writeToFile:[NSString stringWithFormat:@"/Users/jwilson/%i-%@.xml", self.type, self.desc] atomically:NO];

	/* Update status */
	self.xmlStatus = @"Processing Data...";
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTEntityXmlStatusChanged" object:self];
    
    /* Parse XML using TBXML */
    TBXML *tbxml = [[TBXML alloc] initWithXMLData:receivedData];
    if (tbxml.rootXMLElement) [self updateEntityUsingXML:tbxml];
    else
    {
        /* Bad XML Received */
        self.lastRefreshFailed = YES;
    }
    [tbxml release];
    
	/* Update Status */
	self.xmlStatus = @"Done.";
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTEntityXmlStatusChanged" object:self];

	/* Post Notification */
	self.refreshInProgress = NO;
	self.hasBeenRefreshed = YES;
	[[NSNotificationCenter defaultCenter] postNotificationName:@"RefreshFinished" object:self];	

    /* Call super (it will cleanup receivedData) */
    [super connectionDidFinishLoading:connection];
    
    /* Local clean up */
    [connection release];   // Our connection is alloc/init'ed locally 
}

- (void) updateEntityUsingXML:(TBXML *)xml
{
    TBXMLElement *entity = [TBXML childElementNamed:@"entity" parentElement:xml.rootXMLElement];
    if (entity) [self updateEntityUsingXMLNode:entity];
}

- (void) updateEntityUsingXMLNode:(TBXMLElement *)node
{
    if (!node) return;
    TBXMLElement *rootNode = node;
    
    /* Create local autorelease pool */
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
	/* Update local properties and value */
    BOOL currentValueSet = NO;
    for (node=node->firstChild; node; node = node->nextSibling)
    {
        if (strcmp(node->name, "value")==0)
        {
            /* Create Value */
            LTMetricValue *curValue = [LTMetricValue new];
            curValue.stringValue = [TBXML textForElementNamed:@"valstr" parentElement:node];
            curValue.floatValue = [[TBXML textForElementNamed:@"valstr_raw" parentElement:node] floatValue];
            curValue.timestamp = [NSDate dateWithTimeIntervalSince1970:[[TBXML textForElementNamed:@"tstamp_sec" parentElement:node] floatValue]];
            
            /* Set entity current value */
            BOOL currentValueChanged = NO;      // Changes to YES if currentValue is present and updated
            if (!currentValueSet)
            {
                if (![self.currentValue isEqualToString:curValue.stringValue] && !self.isNew)
                { 
                    /* A currentValue was set and has now changed, always notify */
                    currentValueChanged = YES; 
                }
                self.currentValue = [[curValue.stringValue copy] autorelease];  // Done the long way round to catch a mem leak
                currentValueSet = YES;
            }
            if (currentValueChanged)
            {
                [[NSNotificationCenter defaultCenter] postNotificationName:kLTEntityValueChanged object:self];
            }
			
            /* Enqueue */
            if (!self.values)
            { values = [[NSMutableArray array] retain]; }
            [values addObject:curValue];
            [curValue release];
        }
        else
        {
            /* Update local properties */
            [self setXmlValueUsingNode:node forKey:[TBXML elementName:node]];
        }
    }

    /* Drain autorelease pool */
    [pool drain];
    pool = nil;
    
    /* Create new autorelease pool for child parsing */
    pool = [[NSAutoreleasePool alloc] init];

    /* Recursively parse children */
    BOOL childrenChanged = NO;
    for (node=rootNode->firstChild; node; node = node->nextSibling)
    {
        if (strcmp(node->name, "entity")==0)
        {
            /* Locate/Create Child */
            LTEntity *childEntity = [[childDict objectForKey:[TBXML textForElement:[TBXML childElementNamed:@"name" parentElement:node]]] retain];
            if (!childEntity)
            {
                childEntity = [LTEntity new];
                childEntity.parent = self;
                childEntity.isNew = YES;
            }
            
            /* Clear all current values */
            [childEntity.values removeAllObjects];
            
            /* Perform update recursively */
            [childEntity updateEntityUsingXMLNode:node];
            
            /* Add new child if necessary */
            if (![childDict objectForKey:childEntity.name])
            {
                [children addObject:childEntity];
                [childDict setObject:childEntity forKey:childEntity.name];
                
                childrenChanged = YES;  // Ensure notification is sent 
                childEntity.isNew = NO; // Entity is not new now that it's in the parents children list
            }
            
            /* Release childEntity */
            [childEntity release];
        }
    }
    if (childrenChanged)
    {
        /* Post notification for children list change */
        [[NSNotificationCenter defaultCenter] postNotificationName:kLTEntityChildrenChanged object:self];
        
        /* Check to see if we're a location */
        if (self.type == ENT_SITE)
        {
            /* We're a location, the device list must be sorted manually to
             * ensure that when the "Grouped by Location" view mode is used
             * in the entity table view that the device list is sorted properly
             */
            NSSortDescriptor *sortDesc = [[[NSSortDescriptor alloc] initWithKey:@"desc" ascending:YES selector:@selector(localizedCompare:)] autorelease];
            [children sortUsingDescriptors:[NSArray arrayWithObjects:sortDesc, nil]];			
        }
    }
    
    /* Drain autorelease pool */
    [pool drain];
    pool = nil;
}

#pragma mark "XML Property Setting"

- (NSMutableDictionary *) xmlTranslation
{ return [LTEntity xmlTranslation]; }

- (void) _setXmlValue:(id)value forKey:(NSString *)key
{
	/* Get Translated Key */
    key = [self.xmlTranslation objectForKey:key]; 
	
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
				NSLog (@"WARNING: LTEntity:%@ Value type %s for %@ is not explicity handled, assuming native (%@) will be OK.", [self class], valueType, key, [value class]);
		}
	}
	else
	{ NSLog (@"WARNING: LTEntity:%@  Failed to find valueType for key %@, no value type translation will be done.", [self class], key); }
	
	/* Set value */
	[self setValue:value forKey:key]; 	
}

- (void) setXmlValueUsingNode:(TBXMLElement *)node forKey:(NSString *)key
{
    if (![self.xmlTranslation objectForKey:key])
    {
        return;
    }
    [self _setXmlValue:[TBXML textForElement:node] forKey:key];
    
}
- (void) setXmlValue:(id)value forKey:(NSString *)key
{
    if (![self.xmlTranslation objectForKey:key]) return;
    [self _setXmlValue:value forKey:key];
}


+ (BOOL)accessInstanceVariablesDirectly
{
	return NO;
}

- (void) setValue:(id)value forUndefinedKey:(NSString *)key
{
	NSLog (@"ERROR: LCXMLObject:%@ tried to set value for undefined key %@", [self class], key);
}

#pragma mark Graphable Metrics

- (NSArray *) graphableMetrics
{
	NSMutableArray *graphableMetrics = [NSMutableArray array];
	if (self.type == 4)
	{
		/* Container -- provide a single metric per object */
		for (LTEntity *object in self.children)
		{
			LTEntity *firstPercentage = nil;
			NSMutableArray *objGraphableMetrics = [NSMutableArray array];
			for (LTEntity *metric in object.children)
			{
				if (metric.recordMethod == 1 && metric.recordEnabled)
				{ 
					[objGraphableMetrics addObject:metric]; 
					if ([metric.units isEqualToString:@"%"] && !firstPercentage) firstPercentage = metric;
				}
			}
			if (firstPercentage) [graphableMetrics addObject:firstPercentage];
			else
			{
				if (objGraphableMetrics.count > 0) [graphableMetrics addObject:[objGraphableMetrics objectAtIndex:0]];
			}
		}
	}
	else if (self.type == 5)
	{
		/* Object -- provide all graphable metrics */
		for (LTEntity *metric in self.children)
		{
			if (metric.recordMethod == 1 && metric.recordEnabled) [graphableMetrics addObject:metric]; 
		}
		
	}
	else if (self.type == 6)
	{
		/* Metric -- self! */
		if (self.recordMethod == 1 && self.recordEnabled) [graphableMetrics addObject:self];
	}
	return graphableMetrics;
}

#pragma mark -
#pragma mark Entity Copying

/* Entities are copied to make a 'working copy' of an
 * entity that can be freed when the data is no longer needed
 */

- (id)copyWithZone:(NSZone *)zone
{
    LTEntity *copy = [[LTEntity allocWithZone:zone] init];
    
    /* Copy the entity using the xmlTranslation */
    for (NSString *key in [[self xmlTranslation] allValues])
    {
        [copy setValue:[self valueForKey:key] forKey:key];
    }
    if (self.parent)
    {
        copy.parent = self.parent; // Also takes care of customer, coreDeployment, etc
    }
    else
    {
        copy.ipAddress = self.ipAddress;
        copy.customerName = self.customerName;
        copy.username = self.username;
        copy.password = self.password;
        copy.customer = self.customer;
        copy.coreDeployment = self.coreDeployment;
    }
    copy.entityDescriptor = self.entityDescriptor;
    copy.entityAddress = self.entityAddress;
    copy.resourceAddress = self.resourceAddress;
    
    return copy;
}

#pragma mark "Properties"

@synthesize type;
@synthesize name;
@synthesize desc;
@synthesize deviceResourceAddress;
- (NSString *) deviceResourceAddress
{
    /* Dynamically creates/assumes the device resource address
     * by creating the deviceEntityAddress and then using the devname
     * to create the res addr as <Cust>:<Cust>:7:0:<Name>
     */
    NSArray *parts = [[self deviceEntityAddress] componentsSeparatedByString:@":"];
    if (parts.count > 3)
    {
        NSString *devName = [parts objectAtIndex:3];
        NSString *custResAddr = [self.customer resourceAddress];
        NSArray *custParts = [custResAddr componentsSeparatedByString:@":"];
        if (custParts.count > 1)
        {
            return [NSString stringWithFormat:@"%@:%@:7:0:%@", [custParts objectAtIndex:0], [custParts objectAtIndex:0], devName];
        }
        else return nil;
    }
    else return nil;
}
@synthesize resourceAddress;
- (NSString *) resourceAddress
{
	if (resourceAddress) return resourceAddress;
	else
	{
        /* Otherwise, use customer or device depending 
         * on our entity type 
         */
        if (self.type < 3) return self.customer.resourceAddress;
        else return [self deviceResourceAddress];
	}
}
@synthesize entityAddress;
- (NSString *) entityAddress
{
    if (entityAddress)
    {
        return entityAddress;
    }
    else
    {
        /* Recursively build an entityAddress using our parent */
        NSString *parentAddress = [self.parent entityAddress];
        if (parentAddress)
        {
            NSArray *parts = [parentAddress componentsSeparatedByString:@":"];
            NSMutableString *localAddress = [NSMutableString stringWithFormat:@"%i", self.type];
            for (NSString *part in parts)
            {
                if ([parts indexOfObject:part] == 0) continue;
                [localAddress appendFormat:@":%@", part];
            }
            [localAddress appendFormat:@":%@", self.name];
            self.entityAddress = localAddress;
            return localAddress;
        }
        else
        {
            NSLog(@"ERROR: Failed to assemble dynamic entity address for %p %i:%@", self, self.type, self.name);
            return nil;
        }
    }
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
		if (self.parent)
		{
			/* A parent is present, this must be a live entity */
			while (entity.type > 3)
			{ 
				entity = entity.parent; 
			}
			return entity;
		}
		else if (self.customer)
		{
			/* Entity must be a stand-alone (such as off an incident) */
			LTEntity *site = [self.customer.childDict objectForKey:self.entityDescriptor.siteName];
			LTEntity *device = [site.childDict objectForKey:self.entityDescriptor.devName];
			return device;
		}
		else return nil;
	}
    else if ([self isMemberOfClass:[LTTriggerSet class]] || [self isMemberOfClass:[LTTrigger class]])
    {
        return [self.parent parentOfType:3];
    }
	else
	{ return nil; }
}
- (LTEntity *) parentOfType:(int)parentType
{
    /* Set initial entity */
	LTEntity *entity = self;
    
    /* Special handling for triggersets */
    if ([self isMemberOfClass:[LTTriggerSet class]])
    {
        /* Start at the metric */
        LTTriggerSet *tset = (LTTriggerSet *)self;
        entity = tset.parent;
        NSLog (@"parentOfType on %@ using entity %@ from tset.parent", self, entity);
    }

    /* Special handling for triggers (created by triggersetlist) */
    if ([self isMemberOfClass:[LTTrigger class]])
    {
        /* Start at the metric */
        LTTrigger *trg = (LTTrigger *)self;
        entity = trg.parent;
        NSLog (@"parentOfType on %@ using entity %@ from trg.parent", self, entity);
    }

	/* Will return self if self.type == type */
	if (parentType > entity.type) return nil;    
    
    /* Loop up the hierarchy to find parent */
	while (entity && entity.type > parentType)
	{ entity = entity.parent; }
	return entity;
}
- (LTEntity *) site
{ return [self parentOfType:2]; }
- (LTEntity *) container 
{ return [self parentOfType:4]; }
- (LTEntity *) object
{ return [self parentOfType:5]; }
- (LTEntity *) metric
{ return [self parentOfType:6]; }
@synthesize adminState;
- (void) setAdminState:(int)value
{
    if (adminState != value)
    {
        adminState = value;
        if (!self.isNew)
        {
            [[NSNotificationCenter defaultCenter] postNotificationName:kLTEntityStateChanged object:self];
        }
    }
}
@synthesize opState;
- (void) setOpState:(int)value
{
    if (opState != value)
    {
        opState = value;
        if (!self.isNew)
        {
            [[NSNotificationCenter defaultCenter] postNotificationName:kLTEntityStateChanged object:self];    
        }
    }
}
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

@synthesize deviceIpAddress;
@synthesize lomIpAddress;
@synthesize snmpVersion;
@synthesize snmpCommunity;
@synthesize snmpAuthMethod;
@synthesize snmpAuthPassword;
@synthesize snmpPrivacyMethod;
@synthesize snmpPrivacyPassword;
@synthesize deviceUsername;
@synthesize devicePassword;
@synthesize lomUsername;
@synthesize lomPassword;
@synthesize vendorModule;
@synthesize deviceProtocol;
@synthesize useICMP;
@synthesize useProcessList;
@synthesize useLOM;

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

- (UIImage *) icon
{
//	return [UIImage imageNamed:@"computer.png"];
	return nil;
}

- (LTEntity *) locateChildUsingEntityDescriptor:(LTEntityDescriptor *)entDesc 
{
	return [self locateChildType:entDesc.type usingEntityDescriptor:entDesc];
}

- (LTEntity *) locateChildType:(int)childType usingEntityDescriptor:(LTEntityDescriptor *)entDesc 
{
	if (childType < self.type) return nil;
	LTEntity *child = self;
	while (child && child.type < childType)
	{
		NSString *childName = nil;
		switch (child.type) {
			case 1:
				childName = entDesc.siteName;
				break;
			case 2:
				childName = entDesc.devName;
				break;
			case 3:
				childName = entDesc.cntName;
				break;
			case 4:
				childName = entDesc.objName;
				break;
			case 5:
				childName = entDesc.metName;
				break;
			case 6:
				childName = entDesc.trgName;
				break;
		}
		if (childName)
		{
			child = [child.childDict objectForKey:childName];
		}
		else return nil;
	}
	return child;
}

- (NSString *) description
{
	return [[super description] stringByAppendingFormat:@"%i:%@:%@", self.type, self.name, self.desc];
}

- (void) setRefreshInProgress:(BOOL)value
{
	[super setRefreshInProgress:value];
	AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
	if (self.refreshInProgress) [appDelegate entityRefreshDidBegin:self];
	else [appDelegate entityRefreshDidFinish:self];
}

- (BOOL) isPercentage
{
    if (self.type == 6 && [self.units isEqualToString:@"%"]) return YES;
    else return NO;
}

@synthesize isNew;

- (UIColor *) opStateTintColor
{
    UIColor *tintColor = [UIColor colorWithWhite:0.29 alpha:1.0];
    switch (self.opState) 
    {
        case 3:
            tintColor = [UIColor colorWithRed:130./255. green:66./255. blue:63./255. alpha:1.0];
            break;
        case 2:
        case 1:
            tintColor = [UIColor colorWithRed:112./255. green:93./255. blue:63./255. alpha:1.0];
            break;
        case 0:
            tintColor = [UIColor colorWithRed:0.29 green:0.35 blue:0.29 alpha:1.0];
            break;
        default:
            break;
    }
    return tintColor;
}

@synthesize lastRefreshFailed;

- (LTEntity *) valueMetric
{
    /* Returns the 'best' or most appropriate metric entity
     * that can be used to show a value for the given entity
     */
    if (self.type == 6) return self;
    
    /* Locate an object */
    LTEntity *obj = nil;
    if (self.type == 4 && self.children.count == 1) 
    { obj = [self.children objectAtIndex:0]; }
    else if (self.type == 5) obj = self;
    
    /* Iterate through metrics */
    LTEntity *valueMetric = nil;
    for (LTEntity *metric in obj.children)
    {
        /* Prefer metrics that are a percentage, and have triggers */
        if (metric.hasTriggers && metric.isPercentage) 
        {
            /* Best match found */
            valueMetric = metric;
            break;
        }
        else if (!valueMetric && metric.isPercentage)
        {
            /* First percentage */
            valueMetric = metric;
            break;
        }
        else if (!valueMetric && metric.hasTriggers)
        {
            /* First non-percentage with triggers */
            valueMetric = metric;
            break;
        }
    }
    return valueMetric;
}

@synthesize  groupParent;

@end
