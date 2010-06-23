//
//  ModuleDocument.h
//  ModuleBuilder
//
//  Created by James Wilson on 9/02/08.
//  Copyright LithiumCorp Pty Ltd 2008 . All rights reserved.
//


#import <Cocoa/Cocoa.h>

#import "MBCoreDeployment.h"
#import "MBOidSection.h"

@class MBContainer;

@interface ModuleDocument : NSDocument
{
	/* MIBs */
	NSMutableArray *mibs;
	
	/* Properties */
	NSMutableDictionary *properties;
	NSMutableDictionary *vendorModuleDict;
	
	/* UI Elements */
	NSWindowController *mainWindowController;
	
	/* Non-Saved Properties */
	NSMutableArray *baseOids;
	NSMutableDictionary *baseOidDict;
	NSMutableDictionary *oidsByMibDict;
	MBOidSection *curSection;
}

#pragma mark Document Methods
- (id)init;
- (NSString *)windowNibName;
- (void)windowControllerDidLoadNib:(NSWindowController *) aController;
- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError;
- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError;

#pragma mark OID Accessors
- (NSMutableArray *) oids;
- (void) setOids:(NSMutableArray *)array;
- (NSMutableArray *) baseOids;
- (void) setBaseOids:(NSMutableArray *)array;
- (NSMutableDictionary *) baseOidDict;
- (void) setBaseOidDict:(NSMutableDictionary *)dict;
- (void) insertObject:(id)obj inOidsAtIndex:(unsigned int)index;
- (void) updateOIDSectionNames;
- (void) removeObjectFromOidsAtIndex:(unsigned int)index;
- (int) countOfOids;
- (void) removeAllObjectsFromOids;
- (void) insertObject:(id)obj inBaseOidsAtIndex:(unsigned int)index;
- (void) removeObjectFromBaseOidsAtIndex:(unsigned int)index;
- (NSMutableDictionary *) oidsByMibDict;
- (void) setOidsByMibDict:(NSMutableDictionary *)value;
- (NSMutableArray *) oidSections;
- (void) setOidSections:(NSMutableArray *)array;
- (void) insertObject:(id)obj inOidSectionsAtIndex:(unsigned int)index;
- (void) removeObjectFromOidSectionsAtIndex:(unsigned int)index;

#pragma mark MIB Accessors
- (NSMutableArray *) mibs;
- (void) setMibs:(NSMutableArray *)array;
- (void) insertObject:(id)obj inMibsAtIndex:(unsigned int)index;
- (void) removeObjectFromMibsAtIndex:(unsigned int)index;
- (int) countOfMibs;

#pragma mark Container Accessors
- (NSMutableArray *) containers;
- (void) setContainers:(NSMutableArray *)array;
- (void) insertObject:(id)obj inContainersAtIndex:(unsigned int)index;
- (void) removeObjectFromContainersAtIndex:(unsigned int)index;
- (void) moveContainer:(MBContainer *)container toIndex:(unsigned int)index;
- (unsigned int) countOfContainers;
- (MBContainer *) containerNamed:(NSString *)desc;

#pragma mark "Core Deployment Accessors"
- (NSMutableArray *) coreDeployments;
- (void) setCoreDeployments:(NSMutableArray *)array;
- (void) insertObject:(MBCoreDeployment *)obj inCoreDeploymentsAtIndex:(unsigned int)index;
- (void) removeObjectFromCoreDeploymentsAtIndex:(unsigned int)index;

#pragma mark XML Output Methods
- (NSXMLDocument *) compileXml;
- (void) outputXmlToFile:(NSString *)filename;

#pragma mark Properties Accessors
- (NSMutableDictionary *) properties;
- (void) setProperties:(NSMutableDictionary *)dict;

#pragma mark Module Properties (written to XML)
- (NSMutableDictionary *) moduleProperties;
- (void) setModuleProperties:(NSMutableDictionary *)dict;
@property (assign) NSString *vendorModule;
@property (assign) int vendorModuleTag;

#pragma mark "Generate Accessors"
- (NSWindowController *) mainWindowController;

@end
