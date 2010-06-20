//
//  LCCoreLicenseEntitlement.m
//  Lithium Console
//
//  Created by James Wilson on 6/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCoreLicenseEntitlement.h"


@implementation LCCoreLicenseEntitlement

#pragma mark "Constructors"

+ (LCCoreLicenseEntitlement *) entitlementForCustomer:(LCCustomer *)initCustomer
{
	return [[[LCCoreLicenseEntitlement alloc] initForCustomer:initCustomer] autorelease];
}

- (LCCoreLicenseEntitlement *) initForCustomer:(LCCustomer *)initCustomer
{
	self = [super init];
	if (!self) return nil;
	
	self.customer = initCustomer;
	
	if (!self.xmlTranslation)
	{
		self.xmlTranslation = [NSMutableDictionary dictionary];
		[self.xmlTranslation setObject:@"isLicensed" forKey:@"customer_licensed"];
		[self.xmlTranslation setObject:@"maxDevices" forKey:@"devices_max"];
		[self.xmlTranslation setObject:@"deviceCount" forKey:@"devices_used"];
		[self.xmlTranslation setObject:@"excessDevices" forKey:@"devices_excess"];
		[self.xmlTranslation setObject:@"expiry" forKey:@"expiry"];
		[self.xmlTranslation setObject:@"type" forKey:@"type"];
		[self.xmlTranslation setObject:@"ifNFR" forKey:@"nfr"];
		[self.xmlTranslation setObject:@"demoType" forKey:@"demo"];
		[self.xmlTranslation setObject:@"isFree" forKey:@"free"];
		[self.xmlTranslation setObject:@"isLimited" forKey:@"limited"];
	}		

	return self;
}

- (void) dealloc
{
	if (refreshXMLRequest)
	{ 
		[refreshXMLRequest cancel]; 
		[refreshXMLRequest release]; 
	}
	[customer release];
	[super dealloc];
}

#pragma mark "Refresh methods"

- (void) refreshWithPriority:(int)priority
{
	/* Check state */
	if (refreshXMLRequest) return;
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:customer
												  resource:[customer resourceAddress] 
													entity:[customer entityAddress] 
												   xmlname:@"lic_entitlement"
													refsec:0 
													xmlout:nil] retain];
	[refreshXMLRequest setDelegate:self];
	[refreshXMLRequest setThreadedXmlDelegate:self];
	[refreshXMLRequest setPriority:priority];
	[refreshXMLRequest performAsyncRequest];
	[self setRefreshInProgress:YES];
}

- (void) highPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_HIGH]; }

- (void) normalPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_NORMAL]; }

- (void) lowPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_LOW]; }

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	[self setXmlValuesUsingXmlNode:rootNode];
}

- (void) updateTypeString
{
	if (self.isDemo)
	{
		if (self.isExpiredDemo) self.typeString = @"Expired Demo License";
		else self.typeString = @"Demo License";
	}
	else if (self.isFree)
	{
		if (self.isFree) self.typeString = @"Free Demo License";
	}
	else
	{
		if (self.maxDevices == 0) self.typeString = @"Lithium (Unlimited)";
		else if (self.isLimited) self.typeString = @"Lithium 25-LE";
		else self.typeString = [NSString stringWithFormat:@"Lithium %i", self.maxDevices];
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
	[self updateTypeString];
}


#pragma mark "Accessors"

@synthesize customer;
@synthesize refreshInProgress;
@synthesize isLicensed;
@synthesize maxDevices;
@synthesize deviceCount;
@synthesize excessDevices;
@synthesize expiry;
- (void) setExpiry:(int)value
{
	expiry = value;
	if (expiry == 0)
	{
		self.expiryString = @"Does not expire.";
	}
	else
	{
		self.expiryString = [[NSDate dateWithTimeIntervalSince1970:(NSTimeInterval)expiry] description];
	}
}
	
@synthesize expiryString;
@synthesize type;
@synthesize typeString;
@synthesize isNFR;
@synthesize demoType;
- (void) setDemoType:(int)value
{
	demoType = value;
	if (demoType == 0) 
	{
		isDemo = NO;
		isExpiredDemo = NO;
	}
	else if (demoType == 1)
	{
		isDemo = YES;
		isExpiredDemo = NO;
	}
	else if (demoType == 2)
	{
		isDemo = YES;
		isExpiredDemo = YES;
	}
}
	
@synthesize isDemo;
@synthesize isExpiredDemo;
@synthesize isFree;
@synthesize isLimited;

@end
