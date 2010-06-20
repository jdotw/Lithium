//
//  LCVendor.m
//  Lithium Console
//
//  Created by James Wilson on 7/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCVendor.h"


@implementation LCVendor

#pragma mark "Constructors"

+ (LCVendor *) vendorWithProperties:(NSMutableDictionary *)initProperties
{
	return [[[LCVendor alloc] initWithProperties:initProperties] autorelease];
}

- (LCVendor *) initWithProperties:(NSMutableDictionary *)initProperties
{
	[self init];
	[self setProperties:initProperties];
	return self;
}

- (LCVendor *) init
{
	[super init];
	[self setProperties:[NSMutableDictionary dictionary]];
	return self;
}

- (void) dealloc
{
	[properties release];
	[super dealloc];
}

#pragma mark "Properties Accessors"

- (NSMutableDictionary *) properties
{ return properties; }

- (void) setProperties:(NSMutableDictionary *)value
{
	[properties release];
	properties = [value retain];
}

- (NSString *) name
{ 
	return [properties objectForKey:@"name"]; 
}

- (NSString *) desc
{ 
	return [properties objectForKey:@"desc"]; 
}


@end
