//
//  MBMib.m
//  ModuleBuilder
//
//  Created by James Wilson on 9/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "MBMib.h"


@implementation MBMib

#pragma mark Constructors

+ (MBMib *) mibFromFile:(NSString *)path
{
	return [[[MBMib alloc] initWithFile:path] autorelease];
}

- (MBMib *) initWithFile:(NSString *)path
{
	[super init];

	[self setProperties:[NSMutableDictionary dictionary]];
	[self setMibData:[NSData dataWithContentsOfFile:path]];
	[self setFilename:[path lastPathComponent]];
	
	return self;
}

- (MBMib *) initWithCoder:(NSCoder *)decoder
{
	[self init];
	[self setProperties:[[decoder decodeObjectForKey:@"properties"] retain]];
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

#pragma mark Accessors

- (NSMutableDictionary *) properties
{ return properties; }

- (void) setProperties:(NSMutableDictionary *)dict
{
	[properties release];
	properties = [dict retain];
}

- (NSString *) filename
{ return [properties objectForKey:@"filename"]; }

- (void) setFilename:(NSString *)string
{
	[properties setObject:string forKey:@"filename"];
}

- (NSData *) mibData
{ return [properties objectForKey:@"data"]; }

- (void) setMibData:(NSData *)data
{
	[properties setObject:data forKey:@"data"];
}

@end
