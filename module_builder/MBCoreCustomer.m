//
//  MBCoreCustomer.m
//  ModuleBuilder
//
//  Created by James Wilson on 22/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MBCoreCustomer.h"


@implementation MBCoreCustomer

+ (MBCoreCustomer *) customerWithName:(NSString *)initName cluster:(NSString *)initCluster node:(NSString *)initNode url:(NSString *)initUrl
{
	return [[[MBCoreCustomer alloc] initWithName:initName cluster:initCluster node:initNode url:initUrl] autorelease];
}

- (MBCoreCustomer *) initWithName:(NSString *)initName cluster:(NSString *)initCluster node:(NSString *)initNode url:(NSString *)initUrl
{
	[super init];
	
	self.name = initName;
	self.cluster = initCluster;
	self.node = initNode;
	self.url = initUrl;
	
	return self;
}

- (MBCoreCustomer *) initWithCoder:(NSCoder *)decoder
{
	[self init];
	
	self.name = [decoder decodeObjectForKey:@"name"];
	self.cluster = [decoder decodeObjectForKey:@"cluster"];
	self.node = [decoder decodeObjectForKey:@"node"];
	self.url = [decoder decodeObjectForKey:@"url"];
	
	return self;
}

- (void) encodeWithCoder:(NSCoder *)encoder
{
	[encoder encodeObject:name forKey:@"name"];
	[encoder encodeObject:cluster forKey:@"cluster"];
	[encoder encodeObject:node forKey:@"node"];
	[encoder encodeObject:url forKey:@"url"];
}

- (NSString *) entityAddressString
{
	return [NSString stringWithFormat:@"1:%@", name];
}

- (NSString *) resourceAddressString
{
	return [NSString stringWithFormat:@"%@:%@:5:0:%@", cluster, node, name];	
}

@synthesize name;
@synthesize cluster;
@synthesize node;
@synthesize url;

@end
