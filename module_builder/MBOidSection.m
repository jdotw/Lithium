//
//  MBOidSection.m
//  ModuleBuilder
//
//  Created by James Wilson on 25/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MBOidSection.h"


@implementation MBOidSection

+ (MBOidSection *) sectionFromProperties:(NSMutableDictionary *)initProperties
{
	return [[[MBOidSection alloc] initWithProperties:initProperties] autorelease];
}

- (MBOidSection *) init
{
	[super init];
	self.properties = [NSMutableDictionary dictionary];
	self.children = [NSMutableArray array];
	return self;
}

- (MBOidSection *) initWithProperties:(NSMutableDictionary *)initProperties
{
	[self init];
	
	self.properties = initProperties;
	
	return self;
}

- (MBOidSection *) initWithCoder:(NSCoder *)decoder
{
	[self init];
	
	self.properties = [decoder decodeObjectForKey:@"properties"];

	return self;
}

- (void) encodeWithCoder:(NSCoder *)encoder
{
	[encoder encodeObject:properties forKey:@"properties"];
}

- (void) dealloc
{
	[properties release];
	[super dealloc];
}

#pragma mark "Properties"

@synthesize properties;
- (void) setProperties:(NSMutableDictionary *)value
{
	[properties release];
	properties = [value mutableCopy];
}

- (NSString *) name
{ return [properties objectForKey:@"name"]; }
- (void) setName:(NSString *)value
{ [properties setObject:value forKey:@"name"]; }

- (NSMutableArray *) children
{ return [properties objectForKey:@"children"]; }
- (void) setChildren:(NSMutableArray *)array
{ [properties setObject:array forKey:@"children"]; }
- (void) insertObject:(id)obj inChildrenAtIndex:(unsigned int)index
{ 
	[[self children] insertObject:obj atIndex:index];
}
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{ 
	[[self children] removeObjectAtIndex:index];
}

- (NSString *) displayName
{ return self.name; }

- (NSString *) value
{ return nil; }

@end
