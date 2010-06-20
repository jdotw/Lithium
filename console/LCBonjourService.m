//
//  LCBonjourService.m
//  Lithium Console
//
//  Created by James Wilson on 30/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBonjourService.h"
#import "LCBonjourBrowser.h"

@implementation LCBonjourService

#pragma mark "Constructors"

- (id) init
{
	self = [super init];
	if (!self) return nil;
	
	if (!self.xmlTranslation)
	{ self.xmlTranslation = [NSMutableDictionary dictionary]; }
	[self.xmlTranslation setObject:@"name" forKey:@"name"];
	[self.xmlTranslation setObject:@"ip" forKey:@"ip"];

	return self;
}

- (void) dealloc
{ 
	[name release];
	[displayString release];
	[ip release];
	[super dealloc];
}

#pragma mark "Services Properties"

- (NSMutableArray *) children
{ return nil; }

#pragma mark "Device Template Methods"

- (LCDeviceEditTemplate *) deviceTemplate
{
	/* Create template */
	LCDeviceEditTemplate *template = [[LCDeviceEditTemplate new] autorelease];
	template.name = self.name;
	template.desc = self.name;
	template.ipAddress = self.ip;
	
	/* Determine preferred module */
	LCBonjourBrowser *selectedBrowser = self.parent;
	if ([selectedBrowser.service isEqualToString:@"_servermgr._tcp"]) template.preferredModule = @"osx_server";
	else if ([selectedBrowser.service isEqualToString:@"_airport._tcp"]) template.preferredModule = @"airport";
	else if ([selectedBrowser.service isEqualToString:@"_xserveraid._tcp"]) template.preferredModule = @"xraid";
	
	return template;	
}

#pragma mark "Properties"

@synthesize name;
- (void) setName:(NSString *)value
{
	[name release];
	name = [value copy];
	[self updateDisplayString];
}

@synthesize displayString;
@synthesize ip;

- (void) updateDisplayString
{ self.displayString = self.name; }

@synthesize parent;

- (NSString *) uniqueIdentifier
{ return [NSString stringWithFormat:@"%@-%@", [(LCBonjourBrowser *)parent uniqueIdentifier], self.name]; }

@end
