//
//  MBOid.h
//  ModuleBuilder
//
//  Created by James Wilson on 9/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#import <Cocoa/Cocoa.h>

@interface MBOid : NSObject 
{
	NSMutableDictionary *properties;
}

#pragma mark Constructors
+ (MBOid *) oidNamed:(NSString *)initName value:(NSString *)initValue pduType:(int)pduType objid:(oid *)initOid objid_len:(size_t) initSize;
+ (MBOid *) oidFromProperties:(NSMutableDictionary *)dictionary;
- (MBOid *) initWithName:(NSString *)initName value:(NSString *)initValue pduType:(int)pduType objid:(oid *)initOid objid_len:(size_t) initSize;
- (MBOid *) initWithProperties:(NSMutableDictionary *)dictionary;
- (void) dealloc;

#pragma mark OID String
+ (NSString *) oidString:(oid *)objid length:(size_t)len;

#pragma mark Accessors
- (NSMutableDictionary *) properties;
- (void) setProperties:(NSMutableDictionary *)dict;
- (NSString *) name;
- (void) setName:(NSString *)string;
- (NSString *) baseName;
- (void) setBaseName:(NSString *)string;
- (NSString *) value;
- (void) setValue:(NSString *)string;
- (int) pduType;
- (void) setPduType:(int)value;
- (NSString *) oid;
- (void) setOid:(NSString *)string;
- (NSString *) baseOid;
- (void) setBaseOid:(NSString *)string;
@property (copy) NSMutableArray *children;
- (void) insertObject:(id)obj inChildrenAtIndex:(unsigned int)index;
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index;
@property (copy) NSMutableArray *enumerators;
- (void) insertObject:(id)obj inEnumeratorsAtIndex:(unsigned int)index;
- (void) removeObjectFromEnumeratorsAtIndex:(unsigned int)index;
@property (retain) NSString *mibName;
@property (assign) BOOL startOfSection;
@property (copy) NSString *commonPrefix;
@property (copy) NSString *displayName;
@property (readonly) NSImage *displayImage;

@end
