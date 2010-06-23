//
//  MBMetric.m
//  ModuleBuilder
//
//  Created by James Wilson on 12/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "MBMetric.h"
#import "MBMetricViewController.h"
#import "MBContainer.h"
#import "MBEnum.h"

@implementation MBMetric

#pragma mark Constructors

+ (id) metricWithOid:(MBOid *)oid
{
	return [[[MBMetric alloc] initWithOid:oid] autorelease];
}

+ (id) metric
{ 
	return [[[MBMetric alloc] init] autorelease];
}

- (id) initWithOid:(MBOid *)oid
{
	[self init];

	[self setOid:oid];
	
	/* Set recording default */
	int flag = 0;
	switch ([[self oid] pduType])
	{
		case ASN_GAUGE:
			flag = 1;
			break;
	}
	[properties setObject:[NSNumber numberWithInt:flag] forKey:@"record_method"];
	
	return self;
}

- (id) initWithProperties:(NSMutableDictionary *)initProperties
{
	[self init];
	NSArray *keys = [initProperties allKeys];
	for (NSString *key in keys)
	{ [properties setObject:[initProperties objectForKey:key] forKey:key]; }

	return self;
}

- (id) initWithCoder:(NSCoder *)decoder
{
	[super initWithCoder:decoder];
	if (![self multiplier]) [self setMultiplier:[NSNumber numberWithFloat:1.0]];
	[self updateOidDisplayString];
	return self;
}

- (id) init
{ 
	[super init];
	[self setEnumerators:[NSMutableArray array]];
	[self setType:[NSNumber numberWithInt:6]];
	[self setMultiplier:[NSNumber numberWithFloat:1.0]];
	return self;
}

#pragma mark "XML Output"

- (NSXMLNode *) xmlNode
{
	/* Create our root node */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"metric"];
	
	/* Add root-level properties */
	NSEnumerator *objectEnum = [[properties allKeys] objectEnumerator];
	NSString *key;
	while (key=[objectEnum nextObject])
	{
		/* Skip the OID, Container will add this */
		if ([key isEqualToString:@"oid"]) continue;
		else if ([key isEqualToString:@"multiply_by"]) 
		{
			[rootnode addChild:[NSXMLNode elementWithName:key 
											  stringValue:[NSString stringWithFormat:@"%f", [[properties objectForKey:key] floatValue]]]];
		}
		else
		{
			id value = [properties objectForKey:key];
			[rootnode addChild:[NSXMLNode elementWithName:key stringValue:value]];
		}
	}
	
	/* Add value type */
	int val_type = [self valType];
	[rootnode addChild:[NSXMLNode elementWithName:@"val_type" stringValue:[NSString stringWithFormat:@"%i", val_type]]];
	
	/* Add enumerators */
	for (MBEnum *enumerator in self.enumerators)
	{
		[rootnode addChild:[enumerator xmlNode]];
	}
	
	return rootnode;
}

#pragma mark "View Controller"

- (NSViewController *) viewController
{ 
	return [MBMetricViewController viewForMetric:self]; 
}

- (float) rowHeight
{ 
	return 39.0;
}

#pragma mark "General Accessors"

- (MBOid *) oid
{ return [properties objectForKey:@"oid"]; }

- (void) setOid:(MBOid *)oid
{
	/* Set our description based on the OID */
	if ([[self desc] length] < 1 || [[self desc] isEqualToString:[[self oid] name]])
	{ 
		[self willChangeValueForKey:@"desc"];
		NSArray *parts = [[oid baseName] componentsSeparatedByString:@"::"];
		if ([parts count] > 1)
		{ 
			NSString *desc = [parts objectAtIndex:[parts count]-1];
			if (oid.commonPrefix && [desc hasPrefix:oid.commonPrefix] && ![desc isEqualToString:oid.commonPrefix])
			{
				/* Remove common prefix */
				desc = [desc stringByReplacingCharactersInRange:NSMakeRange(0, [oid.commonPrefix length])
													 withString:@""];
			}
			[properties setObject:desc forKey:@"desc"];
		}
		else
		{ [properties setObject:[oid baseName] forKey:@"desc"]; }
		[self didChangeValueForKey:@"desc"];
	}
	
	/* Copy enumerators from OID */
	for (MBEnum *enumerator in oid.enumerators)
	{
		MBEnum *copy = [MBEnum enumWithLabel:enumerator.label forValue:enumerator.value];
		[self insertObject:copy inEnumeratorsAtIndex:self.enumerators.count];
		copy.document = self.document;
	}
	
	/* Set value */
	[[[document undoManager] prepareWithInvocationTarget:self] setOid:(id)[self oid]];
	[properties setObject:oid forKey:@"oid"];
	[[document undoManager] setActionName:@"Set Metric OID"];
	
	/* Update oidDisplayString */
	[self updateOidDisplayString];
}

- (void) updateOidDisplayString
{
	MBContainer *container = (MBContainer *) [self parent];
	NSString *oidName;
	if ([[container mode] intValue] == 1)
	{
		/* DYnamic */
		oidName = [[self oid] baseName];
	}
	else
	{
		/* Static */
		oidName = [[self oid] name];
		
	}
	
	NSString *valueString;
	if ([self valType] >= 1 && [self valType] <= 3)
	{
		valueString = [NSString stringWithFormat:@"%.2f",
					   [[[self oid] value] floatValue] * [[self multiplier] floatValue]];
	}
	else
	{
		valueString = [[self oid] value];
	}
	
	self.oidDisplayString = [NSString stringWithFormat:@"%@ (%@)", oidName, valueString];
}

@synthesize oidDisplayString;	

- (NSNumber *) recordMethod
{ return [properties objectForKey:@"record_method"]; }

- (void) setRecordMethod:(NSNumber *)value
{
	[[[document undoManager] prepareWithInvocationTarget:self] setRecordMethod:[self recordMethod]];
	[properties setObject:value forKey:@"record_method"];	
	[[document undoManager] setActionName:@"Set Record Method"];
}

- (NSString *) maximumValue
{ return [properties objectForKey:@"max_value"]; }

- (void) setMaximumValue:(NSString *)value
{
	[[[document undoManager] prepareWithInvocationTarget:self] setMaximumValue:(id)[self recordMethod]];
	[properties setObject:value forKey:@"max_value"];
	[[document undoManager] setActionName:@"Set Maximum Value"];
}

- (BOOL) webViewFlag
{ return webViewFlag; }

- (void) setWebViewFlag:(BOOL)flag
{ webViewFlag = flag; }

- (int) valType
{
	int val_type = 1;
	switch ([[self oid] pduType])
	{
		case ASN_INTEGER:
			val_type = METRIC_INTEGER;
			break;
		case ASN_COUNTER:
			val_type = METRIC_COUNT;
			break;
		case ASN_GAUGE:
			val_type = METRIC_GAUGE;
			break;
		case ASN_OCTET_STR:
			val_type = METRIC_STRING;
			break;
		case ASN_OBJECT_ID:
			val_type = METRIC_OID;
			break;
		case ASN_IPADDRESS: 
			val_type = METRIC_IP;
			break;
		case ASN_TIMETICKS:
			val_type = METRIC_INTERVAL;
			break;
		case ASN_COUNTER64:
			val_type = METRIC_COUNT64;
			break;
		default:
			NSLog (@"Warning: Metric %@ has unknown type of %i (%s)", [self desc], [[self oid] pduType], snmp_pdu_type([[self oid] pduType]));
			val_type = 1;			
	}	
	return val_type;
}

- (NSString *) units
{ return [properties objectForKey:@"units"]; }

- (void) setUnits:(NSString *)value
{ 
	[[[document undoManager] prepareWithInvocationTarget:self] setUnits:(id)[self units]];
	if (value) [properties setObject:value forKey:@"units"]; 
	else [properties removeObjectForKey:@"units"];
	[[document undoManager] setActionName:@"Set Units"];
}

- (NSNumber *) multiplier
{ return [properties objectForKey:@"multiply_by"]; }
- (void) setMultiplier:(NSNumber *)value;
{ 
	[[[document undoManager] prepareWithInvocationTarget:self] setUnits:(id)[self units]];
	if (value) [properties setObject:value forKey:@"multiply_by"]; 
	else [properties removeObjectForKey:@"multiply_by"];
	[[document undoManager] setActionName:@"Set Multiplier"];
	[self updateOidDisplayString];
}


- (BOOL) showInSummary
{ return [[properties objectForKey:@"summary_flag"] boolValue]; }

- (void) setShowInSummary:(BOOL)flag
{ 
	[[[document undoManager] prepareWithInvocationTarget:self] setShowInSummary:[self showInSummary]];
	[properties setObject:[NSNumber numberWithBool:flag] forKey:@"summary_flag"]; 
	[[document undoManager] setActionName:@"Set Show in Summary"];
}

- (void) setParent:(MBEntity *)value
{
	[super setParent:value];
	[self updateOidDisplayString];
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

- (void) setDocument:(ModuleDocument *)value
{
	[super setDocument:value];
	for (MBEnum *enumerator in self.enumerators)
	{ enumerator.document = value; }
}

@end

