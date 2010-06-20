//
//  LCIPRegistryEntry.m
//  Lithium Console
//
//  Created by James Wilson on 2/10/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCIPRegistryEntry.h"

#import "LCIPRegistryNetwork.h"



@implementation LCIPRegistryEntry

#pragma mark "Constructors"

- (id) init
{
	self = [super init];
	if (!self) return nil;
	
	if (!self.xmlTranslation)
	{ self.xmlTranslation = [NSMutableDictionary dictionary]; }
	[self.xmlTranslation setObject:@"ip" forKey:@"ip"];
	[self.xmlTranslation setObject:@"mask" forKey:@"mask"];
	[self.xmlTranslation setObject:@"network" forKey:@"network"];
	[self.xmlTranslation setObject:@"hostname" forKey:@"dns"];
	[self.xmlTranslation setObject:@"boundToDevice" forKey:@"bound"];
	
	return self;
}

- (void) dealloc
{
	[displayString release];
	[ip release];
	[mask release];
	[network release];
	[hostname release];
	[super dealloc];
}

#pragma mark "Dummy Properties"

- (NSMutableArray *) children
{ return nil; }

#pragma mark "Device Template Methods"

- (LCDeviceEditTemplate *) deviceTemplate
{
	LCDeviceEditTemplate *template = [LCDeviceEditTemplate new];
	if (self.hostname)
	{
		template.name = self.hostname;
		template.desc = self.hostname;
	}
	else
	{
		template.name = self.ip;
		template.desc = self.ip;
	}
	template.ipAddress = self.ip;
	return [template autorelease];	
}

#pragma mark "Accessors"

@synthesize displayString;
@synthesize ip;
- (void) setIp:(NSString *)value
{
	[ip release];
	ip = [value copy];
	self.displayString = value;
}
@synthesize mask;
@synthesize network;
@synthesize hostname;
@synthesize boundToDevice;
@synthesize parent;

- (NSString *) uniqueIdentifier
{ return [NSString stringWithFormat:@"%@-%@", [(LCIPRegistryNetwork *)parent uniqueIdentifier], self.ip]; }

@end
