//
//  LCDeviceEditTemplate.m
//  Lithium Console
//
//  Created by James Wilson on 9/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCDeviceEditTemplate.h"


@implementation LCDeviceEditTemplate

+ (LCDeviceEditTemplate *) templateWithProperties:(NSDictionary *)initProperties
{
	return [[[LCDeviceEditTemplate alloc] initWithProperties:initProperties] autorelease];
}

- (LCDeviceEditTemplate *) initWithProperties:(NSDictionary *)initProperties
{
	self = [self init];
	if (!self) return nil;
	
	self.name = [initProperties objectForKey:@"name"];
	self.desc = [initProperties objectForKey:@"desc"];
	self.ipAddress = [initProperties objectForKey:@"ipAddress"];
	self.lomIpAddress = [initProperties objectForKey:@"lomIpAddress"];
	self.preferredModule = [initProperties objectForKey:@"preferredModule"];
	
	return self;
}

- (id) init
{
	self = [super init];
	if (self != nil) 
	{
		self.enabled = YES;
		CFUUIDRef uuidRef = CFUUIDCreate(nil);
		self.name = (NSString *)CFUUIDCreateString(nil, uuidRef);
		CFRelease(uuidRef);	
	}
	return self;
}


- (void) dealloc
{
	[name release];
	[desc release];
	[ipAddress release];
	[lomIpAddress release];
	[preferredModule release];
	[resultIcon release];
	[super dealloc];
}

@synthesize name;
@synthesize desc;
@synthesize ipAddress;
@synthesize lomIpAddress;
@synthesize enabled;

@synthesize preferredModule;

@synthesize xmlReq;
@synthesize xmlOperationInProgress;

@synthesize resultIcon;

- (NSDictionary *) properties
{
	NSMutableDictionary *properties = [NSMutableDictionary dictionary];
	if (self.name) [properties setObject:self.name forKey:@"name"];
	if (self.desc) [properties setObject:self.desc forKey:@"desc"];
	if (self.ipAddress) [properties setObject:self.ipAddress forKey:@"ipAddress"];
	if (self.lomIpAddress) [properties setObject:self.lomIpAddress forKey:@"lomIpAddress"];
	if (self.preferredModule) [properties setObject:self.preferredModule forKey:@"preferredModule"];
	return properties;
}

@end
