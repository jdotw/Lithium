//
//  LCDevice.m
//  Lithium Console
//
//  Created by James Wilson on 24/07/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCDevice.h"
#import "LCContainer.h"
#import "LCObject.h"
#import "LCService.h"
#import "LCEntityDescriptor.h"

@implementation LCDevice

- (LCDevice *) init
{
	[super init];
	
	services = [[NSMutableArray array] retain];
	serviceDict = [[NSMutableDictionary dictionary] retain];
	procProfiles = [[NSMutableArray array] retain];
	procProfileDict = [[NSMutableDictionary dictionary] retain];
	procNames = [[NSMutableArray array] retain];
		
	[self.xmlTranslation setObject:@"ipAddress" forKey:@"ip"];
	[self.xmlTranslation setObject:@"lomIpAddress" forKey:@"lom_ip"];
	[self.xmlTranslation setObject:@"snmpCommunity" forKey:@"snmpcomm"];
	[self.xmlTranslation setObject:@"snmpVersion" forKey:@"snmpversion"];
	[self.xmlTranslation setObject:@"snmpAuthMethod" forKey:@"snmpauthmethod"];
	[self.xmlTranslation setObject:@"snmpPrivacyEncryption" forKey:@"snmpprivenc"];
	[self.xmlTranslation setObject:@"snmpAuthPassword" forKey:@"snmpauthpassword"];
	[self.xmlTranslation setObject:@"snmpPrivPassword" forKey:@"snmpprivpassword"];
	[self.xmlTranslation setObject:@"profile" forKey:@"profile"];
	[self.xmlTranslation setObject:@"vendor" forKey:@"vendor"];
	[self.xmlTranslation setObject:@"mark" forKey:@"mark"];
	[self.xmlTranslation setObject:@"refreshInterval" forKey:@"refresh_interval"];
	[self.xmlTranslation setObject:@"resourceStarted" forKey:@"resource_started"];
	[self.xmlTranslation setObject:@"refreshCount" forKey:@"refresh_count"];
	[self.xmlTranslation setObject:@"refreshTimeStamp" forKey:@"refresh_tstamp_sec"];
	[self.xmlTranslation setObject:@"username" forKey:@"username"];
	[self.xmlTranslation setObject:@"password" forKey:@"password"];
	[self.xmlTranslation setObject:@"lomUsername" forKey:@"lom_username"];
	[self.xmlTranslation setObject:@"lomPassword" forKey:@"lom_password"];
	[self.xmlTranslation setObject:@"osxProtocol" forKey:@"protocol"];
	[self.xmlTranslation setObject:@"useIcmp" forKey:@"icmp"];
	[self.xmlTranslation setObject:@"useLithiumSnmpExtensions" forKey:@"lithiumsnmp"];
	[self.xmlTranslation setObject:@"monitorProcessList" forKey:@"swrun"];
	[self.xmlTranslation setObject:@"useNagios" forKey:@"nagios"];
	[self.xmlTranslation setObject:@"useLom" forKey:@"lom"];
	[self.xmlTranslation setObject:@"isXsanClient" forKey:@"xsan"];
	[self.xmlTranslation setObject:@"minimumActionSeverity" forKey:@"minimum_action_severity"];

	self.browserViewType = 2;
	self.willUseSNMP = YES;
	minimumActionSeverity = 1;
	
	return self;
}

- (void) dealloc
{
	[services release];
	[serviceDict release];
	[procProfiles release];
	[procProfileDict release];
	[procNames release];
	[ipAddress release];
	[lomIpAddress release];
	[snmpCommunity release];
	[snmpAuthPassword release];
	[snmpPrivPassword release];
	[username release];
	[password release];
	[lomUsername release];
	[lomPassword release];
	[vendor release];
	[profile release];
	[super dealloc];
}

- (Class) childClass
{ return [LCContainer class]; }

#pragma mark "XML Methods"

- (void) updateEntityUsingXMLNode:(LCXMLNode *)node
{
	[super updateEntityUsingXMLNode:node];
	
	/* Device-specific XML parsing */
	for (LCXMLNode *childNode in node.children)
	{
		if ([childNode.name isEqualToString:@"service"])
		{
			/* Interpret service */
			LCService *service = [serviceDict objectForKey:[childNode.properties objectForKey:@"id"]];
			if (!service)
			{
				LCObject *object = nil;
				for (LCXMLNode *entNode in childNode.children)
				{
					if ([entNode.name isEqualToString:@"entity_descriptor"])
					{
						object = (LCObject *) [[LCEntityDescriptor descriptorWithXmlNode:entNode] locateEntity:YES];
					}
				}
				service = [LCService serviceWithObject:object];
				service.taskID = [[childNode.properties objectForKey:@"id"] intValue];
				[self insertObject:service inServicesAtIndex:services.count];
			}
			[service setXmlValuesUsingXmlNode:childNode];
		}
		else if ([childNode.name isEqualToString:@"procpro"])
		{
			/* Interpret process */
			LCProcessProfile *procProfile = [procProfileDict objectForKey:[childNode.properties objectForKey:@"id"]];
			if (!procProfile)
			{
				LCObject *object = nil;
				for (LCXMLNode *entNode in childNode.children)
				{
					if ([entNode.name isEqualToString:@"entity_descriptor"])
					{
						object = (LCObject *) [[LCEntityDescriptor descriptorWithXmlNode:entNode] locateEntity:YES];
					}
				}
				procProfile = [LCProcessProfile new];
				procProfile.object = object;
				procProfile.profileID = [[childNode.properties objectForKey:@"id"] intValue];
				[self insertObject:procProfile inProcProfilesAtIndex:procProfiles.count];
			}																						   
			procProfile.desc = [childNode.properties objectForKey:@"desc"];
			procProfile.match = [childNode.properties objectForKey:@"match"];
			procProfile.argumentsMatch = [childNode.properties objectForKey:@"argmatch"];
		}
		else if ([childNode.name isEqualToString:@"procnames"])
		{
			[procNames removeAllObjects];
			for (LCXMLNode *procNode in childNode.children)
			{
				if ([procNode.name isEqualToString:@"proc"])
				{
					[procNames addObject:[procNode.properties objectForKey:@"name"]];
				}
			}
			[procNames sortUsingSelector:@selector(localizedCompare:)];
		}
	}	
}


#pragma mark "Device Entity Methods"

- (NSImage *) deviceIcon
{
	if (!vendor) return nil;
	
	if ([vendor isEqualToString:@"xraid"])
	{ return [NSImage imageNamed:@"xsrdevice.png"]; }
	
	return nil;
}

- (NSMutableArray *) sortedInterfaceObjects
{
	LCEntity *ifaceContainer = [[[self device] childrenDictionary] objectForKey:@"iface"];
	if (!ifaceContainer)
	{ ifaceContainer = [[[self device] childrenDictionary] objectForKey:@"xsnetwork"]; }
	if (!ifaceContainer)
	{ ifaceContainer = [[[self device] childrenDictionary] objectForKey:@"xsinetwork"]; }
	if (!ifaceContainer)
	{ ifaceContainer = [[[self device] childrenDictionary] objectForKey:@"xnetwork"]; }
	if (!ifaceContainer)
	{ ifaceContainer = [[[self device] childrenDictionary] objectForKey:@"apphysical"]; }
	NSArray *sortDescArray = [NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"name" ascending:YES selector:@selector(localizedCompare:)] autorelease]];
	NSMutableArray *ifaceArray = [NSMutableArray arrayWithArray:[[ifaceContainer children] sortedArrayUsingDescriptors:sortDescArray]];
	if ([[[self device] childrenDictionary] objectForKey:@"fcport"])
	{ [ifaceArray addObjectsFromArray:[[[[self device] childrenDictionary] objectForKey:@"fcport"] children]]; }
	if ([[[self device] vendor] isEqualToString:@"xraid"])
	{
		if ([[self device] valueForKeyPath:@"childrenDictionary.xrhostiface_1.childrenDictionary.master"])
		{ [ifaceArray addObject:[[self device] valueForKeyPath:@"childrenDictionary.xrhostiface_1.childrenDictionary.master"]]; }
		if ([[self device] valueForKeyPath:@"childrenDictionary.xrhostiface_2.childrenDictionary.master"])
		{ [ifaceArray addObject:[[self device] valueForKeyPath:@"childrenDictionary.xrhostiface_2.childrenDictionary.master"]]; }
	}
	return ifaceArray;
}

@synthesize services;
- (void) insertObject:(id)service inServicesAtIndex:(unsigned int)index
{
	[serviceDict setObject:service forKey:[NSString stringWithFormat:@"%i", ((LCService *)service).taskID]];
	[services insertObject:service atIndex:index];
}
- (void) removeObjectFromServicesAtIndex:(unsigned int)index
{
	LCService *service = [services objectAtIndex:index];
	[serviceDict removeObjectForKey:[NSString stringWithFormat:@"%i", service.taskID]];
	[services removeObjectAtIndex:index];
}
@synthesize serviceDict;

@synthesize procProfiles;
- (void) insertObject:(LCProcessProfile *)procProfile inProcProfilesAtIndex:(unsigned int)index
{
	[procProfileDict setObject:procProfile forKey:[NSString stringWithFormat:@"%i", procProfile.profileID]];
	[procProfiles insertObject:procProfile atIndex:index];
}
- (void) removeObjectFromProcProfilesAtIndex:(unsigned int)index
{
	[procProfileDict removeObjectForKey:[NSString stringWithFormat:@"%i", [[procProfiles objectAtIndex:index] profileID]]];
	[procProfiles removeObjectAtIndex:index];
}
@synthesize procProfileDict;
@synthesize procNames;

@synthesize ipAddress;
@synthesize lomIpAddress;
@synthesize snmpCommunity;
@synthesize snmpVersion;
@synthesize snmpAuthMethod;
@synthesize snmpPrivacyEncryption;
@synthesize snmpAuthPassword;
@synthesize snmpPrivPassword;
@synthesize username;
@synthesize password;
@synthesize lomUsername;
@synthesize lomPassword;
@synthesize vendor;
@synthesize profile;
@synthesize refreshInterval;
@synthesize osxProtocol;
- (void) setOsxProtocol:(int)value
{
	osxProtocol = value;
	if (osxProtocol == 1 && ([vendor hasPrefix:@"osx_"] || [vendor hasPrefix:@"xserve_"])) self.willUseSNMP = NO;
	else self.willUseSNMP = YES;
}
@synthesize useIcmp;
@synthesize useLithiumSnmpExtensions;
@synthesize monitorProcessList;
@synthesize useNagios;
@synthesize useLom;
@synthesize isXsanClient;
@synthesize mark, isInProduction, isInTesting, isOutOfService;
- (void) setMark:(int)value
{
	mark = value;
	self.isInProduction = (mark > -2) ? YES : NO;
	self.isInTesting = (mark == -2) ? YES : NO;
	self.isOutOfService = (mark == -3) ? YES : NO;
}
	
@synthesize recentlyRestarted;
@synthesize refreshCount;
@synthesize refreshTimeStamp;

@synthesize willUseSNMP;
@synthesize minimumActionSeverity;
- (void) setMinimumActionSeverity:(int)value
{
	minimumActionSeverity = value;
	if (!self.xmlUpdatingValues)
	{
		NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"update"];
		NSXMLDocument *xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];
		[xmlDoc setVersion:@"1.0"];
		[xmlDoc setCharacterEncoding:@"UTF-8"];
		[rootNode addChild:[NSXMLNode elementWithName:@"minimum_action_severity"
										  stringValue:[NSString stringWithFormat:@"%i", self.minimumActionSeverity]]]; 
		LCXMLRequest *xmlReq = [LCXMLRequest requestWithCriteria:self.customer
														resource:[self.customer resourceAddress]
														  entity:[self entityAddress]
														 xmlname:@"action_device_minimum_severity_update"
														  refsec:0
														  xmlout:xmlDoc];
		[xmlReq setPriority:XMLREQ_PRIO_HIGH];
		[xmlReq performAsyncRequest];
	}
}

@end
