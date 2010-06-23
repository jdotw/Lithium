//
//  MyDocument.m
//  ModuleBuilder
//
//  Created by James Wilson on 9/02/08.
//  Copyright LithiumCorp Pty Ltd 2008 . All rights reserved.
//

#import "ModuleDocument.h"
#import "MBDocumentWindowController.h"
#import "MBMib.h"
#import "MBContainer.h"

@implementation ModuleDocument

#pragma mark Document Methods

- (id)init
{
    [super init];

	/* Create cache path */
	int pid = getpid ();
	[[NSFileManager defaultManager] createDirectoryAtPath:[@"~/Library/Caches/com.lithiumcorp.modulebuilder" stringByExpandingTildeInPath]
											   attributes:nil];
	NSString *cachePath = [[NSString stringWithFormat:@"~/Library/Caches/com.lithiumcorp.modulebuilder/%i", pid] stringByExpandingTildeInPath];
	[[NSFileManager defaultManager] createDirectoryAtPath:cachePath
											   attributes:nil];
	

	/* Create variables */
	[self setProperties:[NSMutableDictionary dictionary]];
	[self setModuleProperties:[NSMutableDictionary dictionary]];
	[self setOids:[NSMutableArray array]];
	[self setBaseOids:[NSMutableArray array]];
	[self setBaseOidDict:[NSMutableDictionary dictionary]];
	[self setMibs:[NSMutableArray array]];
	[self setContainers:[NSMutableArray array]];
	[self setOidSections:[NSMutableArray array]];
	
	/* Set defaults */
	[[self moduleProperties] setObject:@"snmpbasic" forKey:@"vendor"];
	
    return self;
}

- (void) dealloc
{
	[properties release];
	[super dealloc];
}

- (NSString *)windowNibName
{
    return nil;
}

- (void) makeWindowControllers
{
	mainWindowController = [[MBDocumentWindowController alloc] init];
	[self addWindowController:mainWindowController];
}

- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{
    [super windowControllerDidLoadNib:aController];
}

- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError
{
	return [NSKeyedArchiver archivedDataWithRootObject:properties];
}

- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{
	/* Load data */
	[self setProperties:[NSKeyedUnarchiver unarchiveObjectWithData:data]];
	[self setBaseOids:[NSMutableArray array]];
	[self setBaseOidDict:[NSMutableDictionary dictionary]];
	
	/* DEBUG FIX Beta File Fixes */
	if ([[self moduleProperties] objectForKey:@"containers"])
	{ 
		[properties setObject:[[self moduleProperties] objectForKey:@"containers"] forKey:@"containers"]; 
		[[self moduleProperties] removeObjectForKey:@"containers"];
	}

	/* Update document accessors */
	for (MBContainer *container in [self containers])
	{ 
		[container setDocument:self];
		[container bindDependents];
		for (MBMetric *metric in [container children])
		{ [metric bindDependents]; }
	}
	
	/* Write MIBs to Cache */
	for (MBMib *mib in [self mibs])
	{
		int pid = getpid ();
		NSString *cachePath = [[NSString stringWithFormat:@"~/Library/Caches/com.lithiumcorp.modulebuilder/%i", pid] stringByExpandingTildeInPath];
		NSString *filePath = [NSString stringWithFormat:@"%@/%@", cachePath, [mib filename]];
		[[mib mibData] writeToFile:filePath atomically:YES];		
	}
	
    return YES;
}

#pragma mark OID Accessors

- (NSMutableArray *) oids
{ return [properties objectForKey:@"oids"]; }

- (void) setOids:(NSMutableArray *)array
{
	[properties setObject:array forKey:@"oids"];
}

- (void) insertObject:(id)obj inOidsAtIndex:(unsigned int)index
{
	MBOid *oid = obj;

	/* Check for hierarchy change */
	BOOL newSection = NO;
	MBOid *lastOid = [[self oids] lastObject];
	if (lastOid)
	{
		NSArray *lastOidParts = [[lastOid baseOid] componentsSeparatedByString:@"."];
		NSArray *curOidParts = [[oid baseOid] componentsSeparatedByString:@"."];
		if ([lastOidParts count] == [curOidParts count])
		{
			/* Check for a new section based on parsing the OID tree */
			int i;
			for (i = [curOidParts count] - 2; i >= 0; i--)
			{
				NSString *lastOidCriticalPoint = [lastOidParts objectAtIndex:i];
				NSString *curOidCriticalPoint = [curOidParts objectAtIndex:i];
				if (![lastOidCriticalPoint isEqualToString:curOidCriticalPoint])
				{
					newSection = YES;
					break;
				}
			}
		}
		else
		{
			/* Length doesnt match -- must be a new section */
			newSection = YES;
		}
	}
	else
	{ newSection = YES; }
		
	if (newSection)
	{
		/* This OID is the start of a new section */
		oid.startOfSection = YES;
		
		/* Create Section */
		curSection = [[MBOidSection alloc] init];
		curSection.name = [oid baseName];
		[self insertObject:curSection inOidSectionsAtIndex:[[self oidSections] count]];
	}

	[self willChangeValueForKey:@"countOfOids"];
	[[self oids] insertObject:obj atIndex:index];
	[self didChangeValueForKey:@"countOfOids"];
	
	/* Check for top-level OID */
	MBOid *existingBase = [[self baseOidDict] objectForKey:[oid baseOid]];
	if (existingBase)
	{ 
		[existingBase insertObject:oid inChildrenAtIndex:[existingBase.children count]]; 
	}	
	else
	{ 
		MBOid *topLevelOid = [oid copy];
		if ([[[topLevelOid baseName] componentsSeparatedByString:@"::"] count] > 1)
		{ topLevelOid.displayName = [[[topLevelOid baseName] componentsSeparatedByString:@"::"] objectAtIndex:1]; }
		else
		{ topLevelOid.displayName = [topLevelOid baseName]; }
		[topLevelOid insertObject:oid inChildrenAtIndex:0];
		[self insertObject:topLevelOid inBaseOidsAtIndex:[[self baseOids] count]]; 
		[curSection insertObject:topLevelOid inChildrenAtIndex:[[curSection children] count]];
	}
	
	/* Check or per-MIB-OID */
	NSMutableArray *mibOids = [oidsByMibDict objectForKey:[oid mibName]];
	if (mibOids)
	{
		[mibOids addObject:oid];
	}
	else
	{
		mibOids = [NSMutableArray array];
		[oidsByMibDict setObject:mibOids forKey:[oid mibName]];
	}
	
}

- (void) updateOIDSectionNames
{
	for (MBOidSection *section in [self oidSections])
	{
		/* Parse section for common prefix */
		NSMutableString *commonPrefix = nil;
		NSMutableString *mibPrefix = nil;
		for (MBOid *prevOid in section.children)
		{
			NSArray *nameParts = [[prevOid baseName] componentsSeparatedByString:@"::"];
			if ([nameParts count] < 2) continue;
			mibPrefix = [nameParts objectAtIndex:0];
			if (commonPrefix)
			{
				const char *common_str = [commonPrefix cStringUsingEncoding:NSUTF8StringEncoding];
				const char *cur_str = [[nameParts objectAtIndex:1] cStringUsingEncoding:NSUTF8StringEncoding];
				commonPrefix = [NSMutableString string];
				int i;
				for (i=0; i < strlen(cur_str); i++)
				{
					if (common_str[i] != cur_str[i])
					{ break; }
					else
					{ [commonPrefix appendFormat:@"%c", cur_str[i]]; }
				}
			}
			else
			{
				commonPrefix = [NSString stringWithString:[nameParts objectAtIndex:1]];
			}
		}
		
		/* Set common prefix */
		if (commonPrefix)
		{
			section.name = [NSString stringWithFormat:@"%@::%@", mibPrefix, commonPrefix];
			for (MBOid *prevOid in section.children)
			{ prevOid.commonPrefix = commonPrefix; }
		}
	}
}

- (void) removeObjectFromOidsAtIndex:(unsigned int)index
{
	[self willChangeValueForKey:@"countOfOids"];
	[[self oids] removeObjectAtIndex:index];
	[self didChangeValueForKey:@"countOfOids"];
}

- (void) removeAllObjectsFromOids
{
	while ([[self oids] count] > 0)
	{ [self removeObjectFromOidsAtIndex:0]; }
	while ([[self baseOids] count] > 0)
	{ [self removeObjectFromBaseOidsAtIndex:0]; }
	while ([[self oidSections] count] > 0)
	{ [self removeObjectFromOidSectionsAtIndex:0]; }
}

- (int) countOfOids
{ 
	return [[self oids] count];
}

- (NSMutableArray *) baseOids
{ return baseOids; }

- (void) setBaseOids:(NSMutableArray *)array
{
	[baseOids release];
	baseOids = [array retain];
}

- (NSMutableDictionary *) baseOidDict
{ return baseOidDict; }

- (void) setBaseOidDict:(NSMutableDictionary *)dict
{
	[baseOidDict release];
	baseOidDict = [dict retain];
}

- (void) insertObject:(id)obj inBaseOidsAtIndex:(unsigned int)index
{
	MBOid *oid = obj;
	[[self baseOids] insertObject:obj atIndex:index];
	[[self baseOidDict] setObject:obj forKey:[oid baseOid]];
}

- (void) removeObjectFromBaseOidsAtIndex:(unsigned int)index
{
	[[self baseOidDict] removeObjectForKey:[[[self baseOids] objectAtIndex:index] baseOid]];
	[[self baseOids] removeObjectAtIndex:index];
}

- (NSMutableDictionary *) oidsByMibDict
{ return oidsByMibDict; }

- (void) setOidsByMibDict:(NSMutableDictionary *)value
{
	[oidsByMibDict release];
	oidsByMibDict = [value retain];
}

- (NSMutableArray *) oidSections
{
	return [properties objectForKey:@"oidSections"];
}

- (void) setOidSections:(NSMutableArray *)array
{
	[properties setObject:array forKey:@"oidSections"];
}

- (void) insertObject:(id)obj inOidSectionsAtIndex:(unsigned int)index
{
	[[self oidSections] insertObject:obj atIndex:index];
}

- (void) removeObjectFromOidSectionsAtIndex:(unsigned int)index
{
	[[self oidSections] removeObjectAtIndex:index];	
}

#pragma mark MIB Accessors

- (NSMutableArray *) mibs
{ return [properties objectForKey:@"mibs"]; }

- (void) setMibs:(NSMutableArray *)array
{
	[properties setObject:array forKey:@"mibs"];
}

- (void) insertObject:(id)obj inMibsAtIndex:(unsigned int)index
{
	[[[self undoManager] prepareWithInvocationTarget:self] removeObjectFromMibsAtIndex:index];
	[[self mibs] insertObject:obj atIndex:index];
	if (![[self undoManager] isUndoing]) [[self undoManager] setActionName:@"Add MIB"];	
	
	/* Place in cache dir */
	MBMib *mib = obj;
	int pid = getpid ();
	NSString *cachePath = [[NSString stringWithFormat:@"~/Library/Caches/com.lithiumcorp.modulebuilder/%i", pid] stringByExpandingTildeInPath];
	NSString *filePath = [NSString stringWithFormat:@"%@/%@", cachePath, [mib filename]];
	[[mib mibData] writeToFile:filePath atomically:YES];
}

- (void) removeObjectFromMibsAtIndex:(unsigned int)index
{
	/* Remove from cache dir */
	MBMib *mib = [[self mibs] objectAtIndex:index];
	int pid = getpid ();
	NSString *cachePath = [[NSString stringWithFormat:@"~/Library/Caches/com.lithiumcorp.modulebuilder/%i", pid] stringByExpandingTildeInPath];
	NSString *filePath = [NSString stringWithFormat:@"%@/%@", cachePath, [mib filename]];
	[[NSFileManager defaultManager] removeFileAtPath:filePath handler:nil];

	[[[self undoManager] prepareWithInvocationTarget:self] insertObject:mib inMibsAtIndex:index];
	[[self mibs] removeObjectAtIndex:index];
	if (![[self undoManager] isUndoing]) [[self undoManager] setActionName:@"Remove MIB"];
}

- (int) countOfMibs
{ 
	return [[self mibs] count]; 
}

#pragma mark Container Accessors

- (NSMutableArray *) containers
{ return [properties objectForKey:@"containers"]; }

- (void) setContainers:(NSMutableArray *)array
{
	[properties setObject:array forKey:@"containers"];
}

- (MBContainer *) containerNamed:(NSString *)desc
{
	for (MBContainer *container in self.containers)
	{
		if ([[container desc] isEqualToString:desc]) return container;
	}
	return nil;
}

- (void) insertObject:(id)obj inContainersAtIndex:(unsigned int)index
{
	MBContainer *container = obj;
	[container setDocument:self];
	[[[self undoManager] prepareWithInvocationTarget:self] removeObjectFromContainersAtIndex:index];
	[[self containers] insertObject:obj atIndex:index];
	if (![[self undoManager] isUndoing]) [[self undoManager] setActionName:@"Add Container"];
}

- (void) removeObjectFromContainersAtIndex:(unsigned int)index
{
	id obj = [[self containers] objectAtIndex:index];
	[[[self undoManager] prepareWithInvocationTarget:self] insertObject:obj inContainersAtIndex:index];
	[[self containers] removeObjectAtIndex:index];
	if (![[self undoManager] isUndoing]) [[self undoManager] setActionName:@"Remove Container"];
}

- (void) moveContainer:(MBContainer *)container toIndex:(unsigned int)index
{
	unsigned int currentIndex = [[self containers] indexOfObject:container];
	if (currentIndex > index) currentIndex++;
	[[[self undoManager] prepareWithInvocationTarget:self] moveContainer:container toIndex:currentIndex];
	[self willChangeValueForKey:@"containers"];
	[[self containers] insertObject:container atIndex:index];
	[[self containers] removeObjectAtIndex:currentIndex];
	[self didChangeValueForKey:@"containers"];
	if (![[self undoManager] isUndoing]) [[self undoManager] setActionName:@"Move Container"];
}

- (unsigned int) countOfContainers
{ 
	return [[self containers] count]; 
}

#pragma mark XML Output Methods

- (NSXMLDocument *) compileXml
{
	/* Create an XML output to be uploaded to Core */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"modulebuilder"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];	
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	
	/* Add root-level properties */
	NSEnumerator *objectEnum = [[[self moduleProperties] allKeys] objectEnumerator];
	NSString *key;
	while (key=[objectEnum nextObject])
	{
		id value = [[self moduleProperties] objectForKey:key];
		[rootnode addChild:[NSXMLNode elementWithName:key stringValue:value]];
	}
	
	/* Add containers */
	objectEnum = [[self containers] objectEnumerator];
	MBContainer *container;
	while (container=[objectEnum nextObject])
	{
		NSXMLElement *node = (NSXMLElement *) [container xmlNode];
		[rootnode addChild:node];
	}
	
	return xmldoc;
}

- (void) outputXmlToFile:(NSString *)filename
{
	/* Output to file */
	NSXMLDocument *xmldoc = [self compileXml];
	[[xmldoc XMLData] writeToFile:filename atomically:YES];
}

#pragma mark "Core Deployment Accessors"
- (NSMutableArray *) coreDeployments
{ 
	return [properties objectForKey:@"coreDeployments"]; 
}

- (void) setCoreDeployments:(NSMutableArray *)array
{
	[properties setObject:array forKey:@"coreDeployments"];
}

- (void) insertObject:(MBCoreDeployment *)obj inCoreDeploymentsAtIndex:(unsigned int)index
{
	[[self coreDeployments] insertObject:obj atIndex:index];
	[obj setNewlyAdded:YES];
}

- (void) removeObjectFromCoreDeploymentsAtIndex:(unsigned int)index
{
	[[self coreDeployments] removeObjectAtIndex:index];
}

#pragma mark Properties Accessors (saved to Document file)

- (NSMutableDictionary *) properties
{ return properties; }

- (void) setProperties:(NSMutableDictionary *)dict
{
	[properties release];
	properties = [dict retain];
}

#pragma mark Module Properties (written to XML)

- (NSMutableDictionary *) moduleProperties
{ return [properties objectForKey:@"moduleProperties"]; }

- (void) setModuleProperties:(NSMutableDictionary *)dict
{ [properties setObject:dict forKey:@"moduleProperties"]; }

- (NSString *) vendorModule
{ return [[self moduleProperties] objectForKey:@"vendor"]; }

- (void) setVendorModule:(NSString *)value
{ 
	if (value) [[self moduleProperties] setObject:value forKey:@"vendor"]; 
	else [[self moduleProperties] removeObjectForKey:@"vendor"]; 
}

- (int) vendorModuleTag
{ 
	if (![self vendorModule]) return 0;
	else if ([[self vendorModule] isEqualToString:@"snmpbasic"]) return 1;
	else if ([[self vendorModule] isEqualToString:@"snmpadv"]) return 2;
	else if ([[self vendorModule] isEqualToString:@"icmp"]) return 3;
	else if ([[self vendorModule] isEqualToString:@"3com"]) return 10;
	else if ([[self vendorModule] isEqualToString:@"airport"]) return 11;
	else if ([[self vendorModule] isEqualToString:@"alliedtelesyn"]) return 12;
	else if ([[self vendorModule] isEqualToString:@"apcups"]) return 13;
	else if ([[self vendorModule] isEqualToString:@"cisco"]) return 14;
	else if ([[self vendorModule] isEqualToString:@"qlogic"]) return 15;
	else if ([[self vendorModule] isEqualToString:@"foundry"]) return 16;
	else if ([[self vendorModule] isEqualToString:@"filer"]) return 17;
	else if ([[self vendorModule] isEqualToString:@"hp"]) return 18;
	else if ([[self vendorModule] isEqualToString:@"infortrend"]) return 19;
	else if ([[self vendorModule] isEqualToString:@"liebert"]) return 20;
	else if ([[self vendorModule] isEqualToString:@"mgeups"]) return 21;
	else if ([[self vendorModule] isEqualToString:@"netbotz"]) return 22;
	else if ([[self vendorModule] isEqualToString:@"netsnmp"]) return 23;
	else if ([[self vendorModule] isEqualToString:@"printer"]) return 24;
	else if ([[self vendorModule] isEqualToString:@"windows"]) return 25;
	else return 0;
}

- (void) setVendorModuleTag:(int)value
{
	[[[self undoManager] prepareWithInvocationTarget:self] setVendorModuleTag:[self vendorModuleTag]];
	switch (value)
	{
		case 0:
			/* None */
			self.vendorModule = nil;
			break;
		case 1:
			self.vendorModule = @"snmpbasic";
			break;
		case 2:			
			self.vendorModule = @"snmpadv"; 
			break;
		case 3:
			self.vendorModule = @"icmp";
			break;
		case 10:
			self.vendorModule = @"3com";
			break;
		case 11:
			self.vendorModule = @"airport";
			break;
		case 12:
			self.vendorModule = @"alliedtelesyn";
			break;
		case 13:
			self.vendorModule = @"apcups";
			break;
		case 14:
			self.vendorModule = @"cisco";
			break;
		case 15:
			self.vendorModule = @"qlogic";
			break;
		case 16:
			self.vendorModule = @"foundry";
			break;
		case 17:
			self.vendorModule = @"filer";
			break;
		case 18:
			self.vendorModule = @"hp";
			break;
		case 19:
			self.vendorModule = @"infortrend";
			break;
		case 20:
			self.vendorModule = @"liebert";
			break;
		case 21:
			self.vendorModule = @"mgeups";
			break;
		case 22:
			self.vendorModule = @"netbotz";
			break;
		case 23:
			self.vendorModule = @"netsnmp";
			break;
		case 24:
			self.vendorModule = @"printer";
			break;
		case 25:
			self.vendorModule = @"windows";
			break;
		default:
			self.vendorModule = nil;
	}
	[[self undoManager] setActionName:@"Set Extended Module"];
}	

#pragma mark "Generate Accessors"

- (NSWindowController *) mainWindowController 
{ return mainWindowController; }

@end
