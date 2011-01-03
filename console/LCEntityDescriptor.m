//
//  LCEntityDescriptor.m
//  Lithium Console
//
//  Created by James Wilson on 28/06/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCEntityDescriptor.h"
#import "LCEntity.h"
#import "LCCustomerList.h"


@implementation LCEntityDescriptor

#pragma mark "Initialisation"

+ (LCEntityDescriptor *) descriptorWithProperties:(id)initProperties
{
	return [[[LCEntityDescriptor alloc] initWithProperties:initProperties] autorelease];
}

+ (LCEntityDescriptor *) descriptorForEntity:(LCEntity *)entity
{
	return [[[LCEntityDescriptor alloc] initForEntity:entity] autorelease];
}

+ (LCEntityDescriptor *) descriptorWithXmlNode:(LCXMLNode *)xmlNode
{
	return [[[LCEntityDescriptor alloc] initWithXmlNode:xmlNode] autorelease];
}

+ (LCEntityDescriptor *) descriptorWithEntityAddress:(LCEntityAddress *)entityAddress
{
	return [[[LCEntityDescriptor alloc] initWithEntityAddress:entityAddress] autorelease];
}


- (id) initWithProperties:(id)initProperties
{
	[self init];
	[properties addEntriesFromDictionary:initProperties];
	return self;
}

- (id) initWithXmlNode:(LCXMLNode *)xmlNode
{
	[self init];
	
	NSArray *keys = [xmlNode.properties allKeys];
	for (NSString *key in keys) [self.properties setObject:[xmlNode.properties objectForKey:key] forKey:key];
	
	return self;
}

- (id) initForEntity:(LCEntity *)entity
{
	/* Initialise entity descriptor for the entity */
	int i;
	NSString *prefix = nil;
	LCEntity *hierarchyEntity;

	[self init];

	/* Add properties from other entities in hierarchy */
	for (i=0; i < 7; i++)
	{
		switch (i)
		{
			case 0:
				prefix = @"cust";
				hierarchyEntity = [entity customer];
				break;
			case 1:
				prefix = @"site";
				hierarchyEntity = [entity site];
				break;
			case 2:
				prefix = @"dev";
				hierarchyEntity = [entity device];
				break;
			case 3:
				prefix = @"cnt";
				hierarchyEntity = [entity container];
				break;
			case 4:
				prefix = @"obj";
				hierarchyEntity = [entity object];
				break;
			case 5:
				prefix = @"met";
				hierarchyEntity = [entity metric];
				break;
			case 6:
				prefix = @"trg";
				hierarchyEntity = [entity trigger];
				break;
		}
		
		if (hierarchyEntity)
		{
			[properties setValue:[hierarchyEntity name] forKey:[NSString stringWithFormat:@"%@_name", prefix]];
			[properties setValue:[hierarchyEntity desc] forKey:[NSString stringWithFormat:@"%@_desc", prefix]];
			[properties setValue:[hierarchyEntity adminstateInteger] forKey:[NSString stringWithFormat:@"%@_adminstate_num", prefix]];
			[properties setValue:[hierarchyEntity opstateInteger] forKey:[NSString stringWithFormat:@"%@_opstate_num", prefix]];
			if (i == 1)
			{
				/* Site-specific */
				[properties setValue:[[hierarchyEntity properties] objectForKey:@"suburb"] 
							  forKey:[NSString stringWithFormat:@"site_suburb"]];
			}
		}
	}
	
	/* Add local entity values */
	[properties setValue:[entity typeInteger] forKey:@"type_num"];
	[properties setValue:[entity name] forKey:@"name"];
	[properties setValue:[entity desc] forKey:@"desc"];
	[properties setValue:[entity adminstateInteger] forKey:@"adminstate_num"];
	[properties setValue:[entity opstateInteger] forKey:@"opstate_num"];
	
	return self;
}

- (id) initWithEntityAddress:(LCEntityAddress *)entityAddress
{
	/* Constructs a basic entity descriptor using the enityAddress */
	self = [self init];
	[properties setObject:[entityAddress.type stringValue] forKey:@"type_num"];
	if (entityAddress.cust) [properties setObject:entityAddress.cust forKey:@"cust_name"];
	if (entityAddress.site) [properties setObject:entityAddress.site forKey:@"site_name"];
	if (entityAddress.dev) [properties setObject:entityAddress.dev forKey:@"dev_name"];
	if (entityAddress.cnt) [properties setObject:entityAddress.cnt forKey:@"cnt_name"];
	if (entityAddress.obj) [properties setObject:entityAddress.obj forKey:@"obj_name"];
	if (entityAddress.met) [properties setObject:entityAddress.met forKey:@"met_name"];
	if (entityAddress.trg) [properties setObject:entityAddress.trg forKey:@"trg_name"];
	return self;
}

- (id) initWithCoder:(NSCoder *)decoder
{
	[super init];
	properties = [[decoder decodeObjectForKey:@"properties"] retain];
	return self;
}

- (id) init
{
	[super init];
	properties = [[NSMutableDictionary dictionary] retain];
	return self;
}

- (void) dealloc 
{
	[properties release];
	[super dealloc];
}

#pragma mark "XML Methods"

- (NSXMLNode *) xmlNode
{
	NSXMLElement *entnode;
	
	entnode = (NSXMLElement *) [NSXMLNode elementWithName:@"entity_descriptor"];

	[entnode addChild:[NSXMLNode elementWithName:@"type_num" stringValue:[self type_num]]];
	
	[entnode addChild:[NSXMLNode elementWithName:@"name" stringValue:[self name]]];
	[entnode addChild:[NSXMLNode elementWithName:@"desc" stringValue:[self desc]]];
	[entnode addChild:[NSXMLNode elementWithName:@"adminstate" stringValue:[self adminstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"adminstate_num" stringValue:[self adminstate_num]]];
	[entnode addChild:[NSXMLNode elementWithName:@"opstate" stringValue:[self opstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"opstate_num" stringValue:[self opstate_num]]];
	
	[entnode addChild:[NSXMLNode elementWithName:@"cust_name" stringValue:[self cust_name]]];
	[entnode addChild:[NSXMLNode elementWithName:@"cust_desc" stringValue:[self cust_desc]]];
	[entnode addChild:[NSXMLNode elementWithName:@"cust_adminstate" stringValue:[self cust_adminstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"cust_adminstate_num" stringValue:[self cust_adminstate_num]]];
	[entnode addChild:[NSXMLNode elementWithName:@"cust_opstate" stringValue:[self cust_opstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"cust_opstate_num" stringValue:[self cust_opstate_num]]];
	
	[entnode addChild:[NSXMLNode elementWithName:@"site_name" stringValue:[self site_name]]];
	[entnode addChild:[NSXMLNode elementWithName:@"site_desc" stringValue:[self site_desc]]];
	[entnode addChild:[NSXMLNode elementWithName:@"site_suburb" stringValue:[self site_suburb]]];
	[entnode addChild:[NSXMLNode elementWithName:@"site_adminstate" stringValue:[self site_adminstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"site_adminstate_num" stringValue:[self site_adminstate_num]]];
	[entnode addChild:[NSXMLNode elementWithName:@"site_opstate" stringValue:[self site_opstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"site_opstate_num" stringValue:[self site_opstate_num]]];
	
	[entnode addChild:[NSXMLNode elementWithName:@"dev_name" stringValue:[self dev_name]]];
	[entnode addChild:[NSXMLNode elementWithName:@"dev_desc" stringValue:[self dev_desc]]];
	[entnode addChild:[NSXMLNode elementWithName:@"dev_adminstate" stringValue:[self dev_adminstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"dev_adminstate_num" stringValue:[self dev_adminstate_num]]];
	[entnode addChild:[NSXMLNode elementWithName:@"dev_opstate" stringValue:[self dev_opstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"dev_opstate_num" stringValue:[self dev_opstate_num]]];
	
	[entnode addChild:[NSXMLNode elementWithName:@"cnt_name" stringValue:[self cnt_name]]];
	[entnode addChild:[NSXMLNode elementWithName:@"cnt_desc" stringValue:[self cnt_desc]]];
	[entnode addChild:[NSXMLNode elementWithName:@"cnt_adminstate" stringValue:[self cnt_adminstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"cnt_adminstate_num" stringValue:[self cnt_adminstate_num]]];
	[entnode addChild:[NSXMLNode elementWithName:@"cnt_opstate" stringValue:[self cnt_opstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"cnt_opstate_num" stringValue:[self cnt_opstate_num]]];
	
	[entnode addChild:[NSXMLNode elementWithName:@"obj_name" stringValue:[self obj_name]]];
	[entnode addChild:[NSXMLNode elementWithName:@"obj_desc" stringValue:[self obj_desc]]];
	[entnode addChild:[NSXMLNode elementWithName:@"obj_adminstate" stringValue:[self obj_adminstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"obj_adminstate_num" stringValue:[self obj_adminstate_num]]];
	[entnode addChild:[NSXMLNode elementWithName:@"obj_opstate" stringValue:[self obj_opstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"obj_opstate_num" stringValue:[self obj_opstate_num]]];
	
	[entnode addChild:[NSXMLNode elementWithName:@"met_name" stringValue:[self met_name]]];
	[entnode addChild:[NSXMLNode elementWithName:@"met_desc" stringValue:[self met_desc]]];
	[entnode addChild:[NSXMLNode elementWithName:@"met_adminstate" stringValue:[self met_adminstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"met_adminstate_num" stringValue:[self met_adminstate_num]]];
	[entnode addChild:[NSXMLNode elementWithName:@"met_opstate" stringValue:[self met_opstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"met_opstate_num" stringValue:[self met_opstate_num]]];
	
	[entnode addChild:[NSXMLNode elementWithName:@"trg_name" stringValue:[self trg_name]]];
	[entnode addChild:[NSXMLNode elementWithName:@"trg_desc" stringValue:[self trg_desc]]];
	[entnode addChild:[NSXMLNode elementWithName:@"trg_adminstate" stringValue:[self trg_adminstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"trg_adminstate_num" stringValue:[self trg_adminstate_num]]];
	[entnode addChild:[NSXMLNode elementWithName:@"trg_opstate" stringValue:[self trg_opstate]]];
	[entnode addChild:[NSXMLNode elementWithName:@"trg_opstate_num" stringValue:[self trg_opstate_num]]];
	
	return (NSXMLNode *) entnode;
}

#pragma mark "Address string"

- (NSString *) addressString
{
	switch ([[self type_num] intValue])
	{
		case 1: return [NSString stringWithFormat:@"%@:%@", [self type_num], [self cust_name]];
		case 2: return [NSString stringWithFormat:@"%@:%@:%@", [self type_num], [self cust_name], [self site_name]]; 
		case 3: return [NSString stringWithFormat:@"%@:%@:%@:%@", [self type_num], [self cust_name], [self site_name], [self dev_name]];
		case 4: return [NSString stringWithFormat:@"%@:%@:%@:%@:%@", [self type_num], [self cust_name], [self site_name], [self dev_name], [self cnt_name]];
		case 5: return [NSString stringWithFormat:@"%@:%@:%@:%@:%@:%@", [self type_num], [self cust_name], [self site_name], [self dev_name], [self cnt_name], [self obj_name]];
		case 6: return [NSString stringWithFormat:@"%@:%@:%@:%@:%@:%@:%@", [self type_num], [self cust_name], [self site_name], [self dev_name], [self cnt_name], [self obj_name], [self met_name]];
		case 7: return [NSString stringWithFormat:@"%@:%@:%@:%@:%@:%@:%@:%@", [self type_num], [self cust_name], [self site_name], [self dev_name], [self cnt_name], [self obj_name], [self met_name], [self trg_name]];
	}
	
	return nil;
}

#pragma mark "Entity Location Methods"

- (id) locateEntity:(BOOL)createMissing
{
	/* Attempts to find the local LCEntity instance
	 * for the entity decribed by this entity descriptor.
	 * If any of the entities in the path are not found
	 * they will be created using data from the entity descriptor
	 * if the createMissing flag is set to YES
	 */
	
	int i;
	LCEntity *currentParent = nil;
	LCEntity *entity = nil;
	
	for (i=0; i < [[self type_num] intValue]; i++)
	{
		int type;
		NSString *nameString;
		NSString *descString;
		NSString *prefixString;
		NSDictionary *childDictionary = [currentParent childrenDictionary];

		/* Set childDictionary and nameString */
		switch (i)
		{
			case 0:
				/* Customer match */
				childDictionary = [[LCCustomerList masterList] dict];
				nameString = [self cust_name];
				descString = [self cust_desc] ? : [self cust_name];
				prefixString = @"cust_";
				type = 1;
				break;
			case 1:
				/* Site Match */
				nameString = [self site_name];
				descString = [self site_desc] ? : [self site_name];
				prefixString = @"site_";
				type = 2;
				break;
			case 2:
				/* Device match */
				nameString = [self dev_name];
				descString = [self dev_desc] ? : [self dev_name];
				prefixString = @"dev_";
				type = 3;
				break;
			case 3:
				/* Container match */
				nameString = [self cnt_name];
				descString = [self cnt_desc] ? : [self cnt_name];
				prefixString = @"cnt_";
				type = 4;
				break;
			case 4:
				/* Object match */
				nameString = [self obj_name];
				descString = [self obj_desc] ? : [self obj_name];
				prefixString = @"obj_";
				type = 5;
				break;
			case 5:
				/* Metric match */
				nameString = [self met_name];
				descString = [self met_desc] ? : [self met_name];
				prefixString = @"met_";
				type = 6;
				break;
			case 6:
				/* Trigger match */
				nameString = [self trg_name];
				descString = [self trg_desc] ? : [self trg_name];
				prefixString = @"trg_";
				type = 7;
				break;				
		}
		
		/* Attempt to find existing entity */
		entity = [childDictionary objectForKey:nameString];
		if (!entity)
		{
			/* Entity not found */
			if (createMissing == YES)
			{
				/* Create properties */
				NSArray *keys = [[self properties] allKeys];
				NSEnumerator *enumerator = [keys objectEnumerator];
				NSString *key;
				NSMutableDictionary *entityProperties = [NSMutableDictionary dictionary];
				while (key = [enumerator nextObject])
				{
					if ([key hasPrefix:prefixString])
					{
						NSMutableString *newKey = [NSMutableString stringWithString:key];
						[newKey replaceOccurrencesOfString:prefixString 
												withString:@""
												   options:0 
													 range:NSMakeRange(0, [newKey length])];
						[entityProperties setObject:[[self properties] objectForKey:key]
										   forKey:newKey];
						
					}
				}
				
				/* Create missing entity */
				if (type == 1)
				{ 
					entity = (LCEntity *) [LCCustomer customerWithName:nameString cluster:nil node:nil url:nil]; 
				}
				else
				{
					entity = [[currentParent childClass] entityWithType:type 
																   name:nameString 
																   desc:descString 
												  resourceAddressString:[properties objectForKey:@"resaddr"]];
					
				}
				[entity setPresenceConfirmed:NO];
				[entity setParent:currentParent];
				if (currentParent)
				{
					[currentParent insertObject:entity inChildrenAtIndex:0]; 
				}
			}
			else
			{
				/* Match failed, return nil */
				return nil;
			}
		}
		
		/* Set current parent */
		currentParent = entity;
	}
	
	return entity;
}

#pragma mark "Entity Update Methods"

- (void) updateLocalFromDescriptor
{
	/* Locates the local entity and updates it, 
	 * and its parents with the state info found 
	 * in this descriptor 
	 */
	
	/* Locate entity */
	LCEntity *entity = [self locateEntity:YES];
	if (!entity) return;
	
	/* Set local */
	[entity setOpState:[[self opstate_num] intValue]];
	
	/* Customer */
	if ([entity customer])
	{
		if ([self cust_opstate_num]) [[entity customer] setOpState:[[self cust_opstate_num] intValue]];
	}
	else
	{ return; }
	
	/* Site */
	if ([entity site])
	{
		if ([self site_opstate_num]) [[entity site] setOpState:[[self site_opstate_num] intValue]];
	}
	else
	{ return; }
	
	/* Device */
	if ([entity device])
	{
		if ([self dev_opstate_num]) [[entity device] setOpState:[[self dev_opstate_num] intValue]];
	}
	else
	{ return; }

	/* Container */
	if ([entity container])
	{
		if ([self cnt_opstate_num]) [[entity container] setOpState:[[self cnt_opstate_num] intValue]];
	}	
	else
	{ return; }

	/* Object */
	if ([entity object])
	{
		if ([self obj_opstate_num]) [[entity object] setOpState:[[self obj_opstate_num] intValue]];
	}		
	else
	{ return; }
	
	/* Metric */
	if ([entity metric])
	{
		if ([self met_opstate_num]) [[entity metric] setOpState:[[self met_opstate_num] intValue]];
	}
	else
	{ return; }

	/* Trigger */
	if ([entity trigger])
	{
		if ([self trg_opstate_num]) [[entity trigger] setOpState:[[self trg_opstate_num] intValue]];
	}
	else
	{ return; }
}

#pragma mark "Encoding and Decoding"

- (void)encodeWithCoder:(NSCoder *)encoder
{
	[encoder encodeObject:properties forKey:@"properties"];
}

- (id)awakeAfterUsingCoder:(NSCoder *)decoder
{
	/* Locate the entity and return it */
	return [self locateEntity:YES];
}

#pragma mark "Accessor Methods"

@synthesize properties;

- (void) updateProperties:(NSDictionary *)dictionary
{ [properties addEntriesFromDictionary:dictionary]; }

- (NSString *) type { return [properties objectForKey:@"type"]; }
- (NSString *) type_num { return [properties objectForKey:@"type_num"]; }
- (NSString *) name { return [properties objectForKey:@"name"]; }
- (NSString *) desc { return [properties objectForKey:@"desc"]; }
- (NSString *) adminstate { return [properties objectForKey:@"adminstate"]; }
- (NSString *) adminstate_num { return [properties objectForKey:@"adminstate_num"]; }
- (NSString *) opstate { return [properties objectForKey:@"opstate"]; }
- (NSString *) opstate_num { return [properties objectForKey:@"opstate_num"]; }

- (NSString *) cust_name { return [properties objectForKey:@"cust_name"]; }
- (NSString *) cust_desc { return [properties objectForKey:@"cust_desc"]; }
- (NSString *) cust_adminstate { return [properties objectForKey:@"cust_adminstate"]; }
- (NSString *) cust_adminstate_num { return [properties objectForKey:@"cust_adminstate_num"]; }
- (NSString *) cust_opstate { return [properties objectForKey:@"cust_opstate"]; }
- (NSString *) cust_opstate_num { return [properties objectForKey:@"cust_opstate_num"]; }

- (NSString *) site_name { return [properties objectForKey:@"site_name"]; }
- (NSString *) site_desc { return [properties objectForKey:@"site_desc"]; }
- (NSString *) site_suburb { return [properties objectForKey:@"site_suburb"]; }
- (NSString *) site_adminstate { return [properties objectForKey:@"site_adminstate"]; }
- (NSString *) site_adminstate_num { return [properties objectForKey:@"site_adminstate_num"]; }
- (NSString *) site_opstate { return [properties objectForKey:@"site_opstate"]; }
- (NSString *) site_opstate_num { return [properties objectForKey:@"site_opstate_num"]; }

- (NSString *) dev_name { return [properties objectForKey:@"dev_name"]; }
- (NSString *) dev_desc { return [properties objectForKey:@"dev_desc"]; }
- (NSString *) dev_adminstate { return [properties objectForKey:@"dev_adminstate"]; }
- (NSString *) dev_adminstate_num { return [properties objectForKey:@"dev_adminstate_num"]; }
- (NSString *) dev_opstate { return [properties objectForKey:@"dev_opstate"]; }
- (NSString *) dev_opstate_num { return [properties objectForKey:@"dev_opstate_num"]; }

- (NSString *) cnt_name { return [properties objectForKey:@"cnt_name"]; }
- (NSString *) cnt_desc { return [properties objectForKey:@"cnt_desc"]; }
- (NSString *) cnt_adminstate { return [properties objectForKey:@"cnt_adminstate"]; }
- (NSString *) cnt_adminstate_num { return [properties objectForKey:@"cnt_adminstate_num"]; }
- (NSString *) cnt_opstate { return [properties objectForKey:@"cnt_opstate"]; }
- (NSString *) cnt_opstate_num { return [properties objectForKey:@"cnt_opstate_num"]; }

- (NSString *) obj_name { return [properties objectForKey:@"obj_name"]; }
- (NSString *) obj_desc { return [properties objectForKey:@"obj_desc"]; }
- (NSString *) obj_adminstate { return [properties objectForKey:@"obj_adminstate"]; }
- (NSString *) obj_adminstate_num { return [properties objectForKey:@"obj_adminstate_num"]; }
- (NSString *) obj_opstate { return [properties objectForKey:@"obj_opstate"]; }
- (NSString *) obj_opstate_num { return [properties objectForKey:@"obj_opstate_num"]; }

- (NSString *) met_name { return [properties objectForKey:@"met_name"]; }
- (NSString *) met_desc { return [properties objectForKey:@"met_desc"]; }
- (NSString *) met_adminstate { return [properties objectForKey:@"met_adminstate"]; }
- (NSString *) met_adminstate_num { return [properties objectForKey:@"met_adminstate_num"]; }
- (NSString *) met_opstate { return [properties objectForKey:@"met_opstate"]; }
- (NSString *) met_opstate_num { return [properties objectForKey:@"met_opstate_num"]; }

- (NSString *) trg_name { return [properties objectForKey:@"trg_name"]; }
- (NSString *) trg_desc { return [properties objectForKey:@"trg_desc"]; }
- (NSString *) trg_adminstate { return [properties objectForKey:@"trg_adminstate"]; }
- (NSString *) trg_adminstate_num { return [properties objectForKey:@"trg_adminstate_num"]; }
- (NSString *) trg_opstate { return [properties objectForKey:@"trg_opstate"]; }
- (NSString *) trg_opstate_num { return [properties objectForKey:@"trg_opstate_num"]; }

- (NSString *) longDisplayString
{
	switch ([[self type_num] intValue])
	{
		case 1:
			return [NSString stringWithFormat:@"Customer %@", [self cust_desc]];
		case 2:
			return [NSString stringWithFormat:@"Site %@ (%@)", [self site_desc], [self cust_desc]];
		case 3:
			return [NSString stringWithFormat:@"Device %@ at %@ (%@)", [self dev_desc], [self site_desc], [self cust_desc]];
		case 4: 
			return [NSString stringWithFormat:@"%@ Container on %@ at %@ (%@)", 
				[self cnt_desc], [self dev_name], [self site_desc], [self cust_desc]];
		case 5: 
			return [NSString stringWithFormat:@"%@ Object %@ on %@ at %@ (%@)", [self cnt_desc], [self obj_desc], 
				[self dev_name], [self site_desc], [self cust_desc]];
		case 6: 
			return [NSString stringWithFormat:@"%@ %@ %@ Metric on %@ at %@ (%@)", [self cnt_desc], [self obj_desc], 
				[self met_desc], [self dev_name], [self site_desc], [self cust_desc]];
		case 7: 
			return [NSString stringWithFormat:@"%@ %@ %@ %@ Trigger on %@ at %@ (%@)", [self cnt_desc], [self obj_desc], 
				[self met_desc], [self trg_desc], [self dev_name], [self site_desc], [self cust_desc]];
		default:
			return nil;
	}
}

@end
