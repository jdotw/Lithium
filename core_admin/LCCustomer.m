//
//  LCCustomer.m
//  LCAdminTools
//
//  Created by James Wilson on 23/09/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCCustomer.h"
#import "LCConfigController.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <libpq-fe.h>

@implementation LCCustomer

- (id) initWithName:(NSString *)initName desc:(NSString *)initDesc
{
	self = [super init];
	name = [initName copy];
	desc = [initDesc copy];
	return self;
}

+ (LCCustomer *) customerWithName:(NSString *)initName desc:(NSString *)initDesc
{
	return [[[LCCustomer alloc] initWithName:initName desc:initDesc] autorelease];
}

- (void) dealloc
{
	[name release];
	[desc release];
	[super dealloc];
}

@synthesize name;
@synthesize desc;
- (void) setDesc:(NSString *)string
{
	[desc release];
	desc = [string copy];
	
	LCConfigController *configController = [LCConfigController masterController];
	PGconn *conn = PQsetdbLogin([[configController dbHostname] UTF8String],
						[[configController dbPort] UTF8String], 
						NULL, NULL, "lithium", 
						[[configController dbUsername] UTF8String], 
						[[configController dbPassword] UTF8String]);
	NSString *query = [NSString stringWithFormat:@"UPDATE customers SET descr='%@' WHERE name='%@'", self.desc, self.name];
	PQexec (conn, [query cStringUsingEncoding:NSUTF8StringEncoding]);
	PQfinish (conn);
}

@end
