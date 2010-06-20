//
//  LCCoreInfoResource.m
//  Lithium Console
//
//  Created by James Wilson on 26/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCoreInfoResource.h"


@implementation LCCoreInfoResource

+ (LCCoreInfoResource *) resourceWithProperties:(NSMutableDictionary *)initProperties
{
	return [[[LCCoreInfoResource alloc] initWithProperties:initProperties] autorelease];
}

- (LCCoreInfoResource *) initWithProperties:(NSMutableDictionary *)initProperties
{
	[super init];
	[self setProperties:[NSMutableDictionary dictionaryWithDictionary:initProperties]];
	return self;
}

#pragma mark "Accessors"

- (NSMutableDictionary *) properties
{ return properties; }

- (void) setProperties:(NSMutableDictionary *)dict
{
	[properties release];
	properties = [dict retain];
}

- (NSString *) id
{ return [properties objectForKey:@"id"]; }

@end
