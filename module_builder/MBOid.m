//
//  MBOid.m
//  ModuleBuilder
//
//  Created by James Wilson on 9/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "MBOid.h"


@implementation MBOid

#pragma mark Constructors

+ (MBOid *) oidNamed:(NSString *)initName value:(NSString *)initValue pduType:(int)pduType objid:(oid *)initOid objid_len:(size_t) initSize
{
	return [[[MBOid alloc] initWithName:initName value:initValue pduType:pduType objid:initOid objid_len:initSize] autorelease];
}

+ (MBOid *) oidFromProperties:(NSMutableDictionary *)dictionary
{
	return [[[MBOid alloc] initWithProperties:dictionary] autorelease];
}

- (MBOid *) initWithName:(NSString *)initName value:(NSString *)initValue pduType:(int)pduType objid:(oid *)initOid objid_len:(size_t) initSize
{
	[self init];

	[self setProperties:[NSMutableDictionary dictionary]];
	[self setChildren:[NSMutableArray array]];
	[self setEnumerators:[NSMutableArray array]];
	[self setName:initName];
	[self setValue:initValue];
	[self setPduType:pduType];
	[self setOid:[MBOid oidString:initOid length:initSize]];	
	
	
	/* Set name with value */
	self.displayName = [NSString stringWithFormat:@"%@ = %@", self.displayName, self.value];
	
	return self;
}

- (MBOid *) initWithProperties:(NSMutableDictionary *)dictionary
{
	[self init];
	[self setProperties:dictionary];
	return self;
}

- (MBOid *) initWithCoder:(NSCoder *)decoder
{
	[self init];
	[self setProperties:[decoder decodeObjectForKey:@"properties"]];
	return self;
}

- (MBOid *) init
{
	[super init];
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

- (id)copyWithZone:(NSZone *)zone
{
	NSData *data = [NSKeyedArchiver archivedDataWithRootObject:self];
	MBOid *copy = [NSKeyedUnarchiver unarchiveObjectWithData:data];
	return copy;
}

#pragma mark OID String

+ (NSString *) oidString:(oid *)objid length:(size_t)len
{
	size_t i;
	NSMutableString *string = [NSMutableString stringWithString:@""];
	for (i=0; i < len; i++)
	{
		[string appendFormat:@".%i", (int)objid[i]];
	}
	return string;
}

#pragma mark Accessors

- (NSMutableDictionary *) properties
{ return properties; }

- (void) setProperties:(NSMutableDictionary *)dict
{ 
	[properties release];
	properties = [dict retain];
}

- (NSString *) name
{ return [properties objectForKey:@"name"]; }
- (void) setName:(NSString *)string
{ 
	/* Set name property */
	[properties setObject:string forKey:@"name"]; 
	[self setDisplayName:string];

	/* Derive the non-specific portion of the OID */
	NSArray *parts = [[self name] componentsSeparatedByString:@"."];
	NSEnumerator *partEnum = [parts objectEnumerator];
	string = [partEnum nextObject];
	NSMutableString *baseOid = [NSMutableString stringWithString:string];
	while (string=[partEnum nextObject])
	{
		/* Check to see if it's all numeric */
		const char *str = [string cStringUsingEncoding:NSUTF8StringEncoding];
		int i;
		int hasAlpha = 0;
		for (i=0; i < strlen(str); i++)
		{ if (isalpha(str[i])) hasAlpha = 1; }
		if (hasAlpha)
		{ [baseOid appendFormat:@".%@", string]; }
	}
	[self setBaseName:baseOid];

	/* Resolve the base OID */
	oid name[MAX_OID_LEN];
	size_t name_size = MAX_OID_LEN;
	read_objid ([baseOid cStringUsingEncoding:NSUTF8StringEncoding], name, &name_size);
	NSString *resolved = [MBOid oidString:name length:name_size];
	[self setBaseOid:resolved];
	
	/* Find MIB Name */
	NSArray *mibComponents = [[self name] componentsSeparatedByString:@"::"];
	if ([mibComponents count] > 0)
	{ 
		[self setMibName:[mibComponents objectAtIndex:0]]; 
		if ([mibComponents count] > 1) [self setDisplayName:[mibComponents objectAtIndex:1]];
	}
}

- (NSString *) baseName
{ return [properties objectForKey:@"base_name"]; }
- (void) setBaseName:(NSString *)string
{ 
	if (string) [properties setObject:string forKey:@"base_name"]; 
	else [properties removeObjectForKey:@"base_name"];
}

- (NSString *) value
{ return [properties objectForKey:@"curval"]; }
- (void) setValue:(NSString *)string
{ 
	if (string) [properties setObject:string forKey:@"curval"]; 
	else [properties removeObjectForKey:@"curval"];
}

- (int) pduType
{ return [[properties objectForKey:@"pdu_type"] intValue]; }
- (void) setPduType:(int)value
{ [properties setObject:[NSNumber numberWithInt:value] forKey:@"pdu_type"]; }

- (NSString *) oid
{ return [properties objectForKey:@"oid"]; }
- (void) setOid:(NSString *)string
{ 
	if (string) [properties setObject:string forKey:@"oid"]; 
	else [properties removeObjectForKey:@"oid"];
}

- (NSString *) baseOid
{ return [properties objectForKey:@"base_oid"]; }
- (void) setBaseOid:(NSString *)string
{ 
	if (string) [properties setObject:string forKey:@"base_oid"]; 
	else [properties removeObjectForKey:@"base_oid"];
}

- (NSString *) description
{ 
	return [self baseOid]; 
}

- (NSMutableArray *) children
{ return [properties objectForKey:@"children"]; }
- (void) setChildren:(NSMutableArray *)value
{ [properties setObject:[value mutableCopy] forKey:@"children"]; }
- (void) insertObject:(id)obj inChildrenAtIndex:(unsigned int)index
{
	[[self children] insertObject:obj atIndex:index];
}
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{
	[[self children] removeObjectAtIndex:index];
}

- (NSMutableArray *) enumerators
{ return [properties objectForKey:@"enumerators"]; }
- (void) setEnumerators:(NSMutableArray *)value
{ [properties setObject:[value mutableCopy] forKey:@"enumerators"]; }
- (void) insertObject:(id)obj inEnumeratorsAtIndex:(unsigned int)index
{
	[[self enumerators] insertObject:obj atIndex:index];
}
- (void) removeObjectFromEnumeratorsAtIndex:(unsigned int)index
{
	[[self enumerators] removeObjectAtIndex:index];
}


- (NSString *) mibName
{ return [properties objectForKey:@"mibName"]; }
- (void) setMibName:(NSString *)string
{ 
	if (string) [properties setObject:string forKey:@"mibName"]; 
	else [properties removeObjectForKey:@"mibName"];
	
}

- (BOOL) startOfSection
{ return [[properties objectForKey:@"startOfSection"] boolValue]; }
- (void) setStartOfSection:(BOOL)value
{ [properties setObject:[NSNumber numberWithBool:value] forKey:@"startOfSection"]; }

- (NSString *) commonPrefix
{ return [properties objectForKey:@"commonPrefix"]; }
- (void) setCommonPrefix:(NSString *)value
{ 
	if (value) [properties setObject:[value copy] forKey:@"commonPrefix"]; 
	else [properties removeObjectForKey:@"commonPrefix"];
}

- (NSString *) displayName
{ return [properties objectForKey:@"displayName"]; }
- (void) setDisplayName:(NSString *)value
{ 
	if (value) [properties setObject:[value copy] forKey:@"displayName"]; 
	else [properties removeObjectForKey:@"displayName"];
}

- (NSImage *) displayImage
{ return [NSImage imageNamed:@"marker_16.png"]; }

@end
