//
//  LCTriggersetValRule.m
//  Lithium Console
//
//  Created by James Wilson on 6/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCTriggersetValRule.h"
#import "LCXMLRequest.h"
#import "LCTriggerset.h"
#import "LCCustomer.h"

@implementation LCTriggersetValRule

#pragma mark "Constructors"

+ (LCTriggersetValRule *) ruleWithObject:(LCEntity *)initObject triggerset:(id)initTriggerset trigger:(id)initTrigger
{
	return [[[LCTriggersetValRule alloc] initWithObject:initObject triggerset:initTriggerset trigger:initTrigger] autorelease];
}

+ (LCTriggersetValRule *) ruleWithObject:(LCEntity *)initObject 
							  triggerset:(id)initTriggerset
								 trigger:(id)initTrigger
							criteriaSite:(LCEntity *)cSite
						  criteriaDevice:(LCEntity *)cDevice
						  criteriaObject:(LCEntity *)cObject
						 criteriaTrgType:(int)cTrgType
						  criteriaXValue:(NSString *)cXValue
						  criteriaYValue:(NSString *)cYValue
						criteriaDuration:(NSString *)cDuration
					  criteriaAdminstate:(int)cAdminstate
{
	return [[[LCTriggersetValRule alloc] initWithObject:initObject 
											 triggerset:initTriggerset 
												trigger:initTrigger 
										   criteriaSite:cSite 
										 criteriaDevice:cDevice 
										 criteriaObject:cObject 
										criteriaTrgType:cTrgType 
										 criteriaXValue:cXValue 
										 criteriaYValue:cYValue 
									   criteriaDuration:cDuration 
									 criteriaAdminstate:cAdminstate] autorelease];
}

- (LCTriggersetValRule *) initWithObject:(LCEntity *)initObject triggerset:(id)initTriggerset trigger:(id)initTrigger
{
	[super init];
	[self setProperties:[NSMutableDictionary dictionary]];
	obj = [initObject retain];
	tset = [initTriggerset retain];
	trg = [initTrigger retain];
	return self;
}

- (LCTriggersetValRule *) initWithObject:(LCEntity *)initObject 
							  triggerset:(id)initTriggerset
								 trigger:(id)initTrigger
							criteriaSite:(LCEntity *)cSite
						  criteriaDevice:(LCEntity *)cDevice
						  criteriaObject:(LCEntity *)cObject
						 criteriaTrgType:(int)cTrgType
						  criteriaXValue:(NSString *)cXValue
						  criteriaYValue:(NSString *)cYValue
						criteriaDuration:(NSString *)cDuration
					  criteriaAdminstate:(int)cAdminstate
{
	[super init];
	
	/* Create/set variables */
	[self setProperties:[NSMutableDictionary dictionary]];
	obj = [initObject retain];
	tset = [initTriggerset retain];
	trg = [initTrigger retain];
	
	/* Set trigger */
	[self setTriggerName:[trg name]];
	[self setTriggerDesc:[trg desc]];
	
	/* Site criteria */
	if (cSite)
	{ 
		[self setSiteName:[cSite name]];
		[self setSiteDesc:[cSite desc]];
	}
	
	/* Device criteria */
	if (cDevice)
	{
		[self setDeviceName:[cDevice name]];
		[self setDeviceDesc:[cDevice desc]];
	}
	
	/* Object criteria */
	if (cObject)
	{
		[self setObjectName:[cObject name]];
		[self setObjectDesc:[cObject desc]];
	}
	
	/* Trigger type */
	[self setTriggerType:cTrgType];
	
	/* Values */
	[self setXValue:cXValue];
	if (cYValue) [self setYValue:cYValue];
	
	/* Duration */
	[self setDuration:cDuration];
	
	/* Adminstate */
	[self setAdminState:cAdminstate];
	
	return self;
}

- (void) dealloc
{
	[self setProperties:nil];
	[obj release];
	[tset release];
	[trg release];
	[super dealloc];
}

#pragma mark "Update"

- (void) update
{
	/* Create request XML document */
	NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"rule"];
	NSXMLDocument *xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];
	[xmlDoc setVersion:@"1.0"];
	[xmlDoc setCharacterEncoding:@"UTF-8"];
	[rootNode addChild:[NSXMLNode elementWithName:@"tset_name" stringValue:[(LCTriggerset *)tset name]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"met_name" stringValue:[(LCTriggerset *)tset metricName]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"trg_name" stringValue:[self triggerName]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"trg_desc" stringValue:[self triggerDesc]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"id" stringValue:[self ID]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"site_name" stringValue:[self siteName]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"site_desc" stringValue:[self siteDesc]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"dev_name" stringValue:[self deviceName]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"dev_desc" stringValue:[self deviceDesc]]];
	[rootNode addChild:[NSXMLNode elementWithName:@"obj_name" stringValue:[self objectName]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"obj_desc" stringValue:[self objectDesc]]];
	[rootNode addChild:[NSXMLNode elementWithName:@"xval" stringValue:[self xValue]]];
	[rootNode addChild:[NSXMLNode elementWithName:@"yval" stringValue:[self yValue]]];
	[rootNode addChild:[NSXMLNode elementWithName:@"duration" stringValue:[self duration]]];
	[rootNode addChild:[NSXMLNode elementWithName:@"trg_type_num" stringValue:[self triggerTypeInteger]]];
	[rootNode addChild:[NSXMLNode elementWithName:@"adminstate_num" stringValue:[self adminStateInteger]]];
	if (exclusive) [rootNode addChild:[NSXMLNode elementWithName:@"exclusive" stringValue:@"1"]];

	
	/* Create XML Request */
	LCXMLRequest *updateXMLRequest = [[LCXMLRequest requestWithCriteria:[obj customer]
															   resource:[[obj device] resourceAddress] 
																 entity:[obj entityAddress] 
																xmlname:@"triggerset_valrule_update"
																 refsec:0 
																 xmlout:xmlDoc] retain];
	if (!blindUpdate)
	{
		[updateXMLRequest setDelegate:self];
		[updateXMLRequest setXMLDelegate:self];
		[self retain];
		[self setOperationInProgress:YES];
	}
	[updateXMLRequest setPriority:XMLREQ_PRIO_HIGH];
	[updateXMLRequest performAsyncRequest];
}

- (void) updateBlind
{
	blindUpdate = YES;
	[self update];
	blindUpdate = NO;
}

#pragma mark "Delete"

- (void) delete
{
	/* Create request XML document */
	NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"rule"];
	NSXMLDocument *xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];
	[xmlDoc setVersion:@"1.0"];
	[xmlDoc setCharacterEncoding:@"UTF-8"];
	[rootNode addChild:[NSXMLNode elementWithName:@"tset_name" stringValue:[tset name]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"met_name" stringValue:[tset metricName]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"id" stringValue:[self ID]]]; 
	
	
	/* Create XML Request */
	LCXMLRequest *deleteXMLRequest = [[LCXMLRequest requestWithCriteria:[obj customer]
															   resource:[[obj device] resourceAddress] 
																 entity:[obj entityAddress] 
																xmlname:@"triggerset_valrule_delete"
																 refsec:0 
																 xmlout:xmlDoc] retain];
	[deleteXMLRequest setDelegate:self];
	[deleteXMLRequest setXMLDelegate:self];
	[deleteXMLRequest setPriority:XMLREQ_PRIO_HIGH];
	[deleteXMLRequest performAsyncRequest];

	[self retain];
	[self setOperationInProgress:YES];
}

#pragma mark "XML Processing"

- (void) XMLRequestPreParse:(id)sender
{
	/* Set current element to nil */
	curXMLElement = nil;
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	curXMLElement = [element retain];
	
	/* Free previous curXMLString */
	if (curXMLString)
	{
		[curXMLString release];
		curXMLString = nil;
	}
}

- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	/* Append characters to curXMLString */
	if (curXMLElement)
	{
		if (!curXMLString)
		{ curXMLString = [[NSMutableString stringWithString:string] retain]; }
		else
		{ [curXMLString appendString:string]; }
	}
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Add element string to properties dict */
	if (curXMLElement && curXMLString)
	{ 
		[properties setObject:curXMLString forKey:curXMLElement];
	}
	
	/* Release current element */
	if (curXMLElement)
	{
		[curXMLElement release];
		curXMLElement = nil;
	}
	
	/* Free current curXMLString */
	if (curXMLString)
	{
		[curXMLString release];
		curXMLString = nil;
	}
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{	
	/* Free XML request */
	[sender release];
	
	/* Call delegate */
	if (delegate) 
	{
		SEL finishedsel = NSSelectorFromString(@"valRuleOperationFinished:");
		if (finishedsel && [delegate respondsToSelector:finishedsel])
		{ [delegate performSelector:finishedsel withObject:self]; }
	}

	/* Refresh device */
	[[obj device] highPriorityRefresh];
	[[(LCCustomer *)[obj customer] activeIncidentsList] highPriorityRefresh];
	
	/* Set refresh flag */
	[self setOperationInProgress:NO];
	[self release];
}

#pragma mark "Direct Edit Methods"

- (int) siteIndex
{ 
	if ([self siteName]) return 1;
	else return 0;
}
- (void) setSiteIndex:(int)index
{
	if (index == 0)
	{ 
		[self setSiteName:nil];
		[self setSiteDesc:nil];
	}
	else
	{
		[self setSiteName:[[obj site] name]];
		[self setSiteDesc:[[obj site] desc]];
	}
	[self updateBlind];
}

- (int) deviceIndex
{ 
	if ([self deviceName]) return 1;
	else return 0;
}
- (void) setDeviceIndex:(int)index
{
	if (index == 0)
	{ 
		[self setDeviceName:nil];
		[self setDeviceDesc:nil];
	}
	else
	{
		[self setDeviceName:[[obj device] name]];
		[self setDeviceDesc:[[obj device] desc]];
	}
	[self updateBlind];
}

- (int) objectIndex
{
	if ([self objectName]) return 1;
	else return 0;
}
- (void) setObjectIndex:(int)index
{
	if (index == 0)
	{ 
		[self setObjectName:nil];
		[self setObjectDesc:nil];
	}
	else
	{
		[self setObjectName:[obj name]];
		[self setObjectDesc:[obj desc]];
	}
	[self updateBlind];
}

- (int) effectIndex
{ 
	return [[properties objectForKey:@"adminstate_num"] intValue];
}

- (void) setEffectIndex:(int)index
{
	[properties setObject:[NSString stringWithFormat:@"%i", index] forKey:@"adminstate_num"];
	[self updateBlind];
}	

- (NSString *) xValueDirect
{ return [properties objectForKey:@"xval"]; }

- (void) setXValueDirect:(NSString *)value
{ 
	[properties setObject:value forKey:@"xval"];
	[self updateBlind];
}

- (NSString *) yValueDirect
{ return [properties objectForKey:@"yval"]; }

- (void) setYValueDirect:(NSString *)value
{
	[properties setObject:value forKey:@"yval"];
	[self updateBlind];
}

- (NSString *) durationDirect
{ return [properties objectForKey:@"duration"]; }

- (void) setDurationDirect:(NSString *)value
{
	[properties setObject:value forKey:@"duration"];
	[self updateBlind];
}

#pragma mark "Accessors"

- (NSMutableDictionary *) properties
{ return properties; }
- (void) setProperties:(NSMutableDictionary *)newProperties
{
	[properties release];
	properties = [newProperties retain];
}

- (LCEntity *) object
{ return obj; }

- (id) triggerset
{ return tset; }

- (id) trigger
{ return trg; }

- (NSString *) ID
{ return [properties objectForKey:@"id"]; }

- (NSString *) siteName
{ return [properties objectForKey:@"site_name"]; }
- (void) setSiteName:(NSString *)value
{
	if (value) [properties setObject:value forKey:@"site_name"];
	else [properties removeObjectForKey:@"site_name"];
}
- (NSString *) siteDesc
{ return [properties objectForKey:@"site_desc"]; }
- (void) setSiteDesc:(NSString *)value
{
	if (value) [properties setObject:value forKey:@"site_desc"];
	else [properties removeObjectForKey:@"site_desc"];
}

- (NSString *) deviceName
{ return [properties objectForKey:@"dev_name"]; }
- (void) setDeviceName:(NSString *)value
{
	if (value) [properties setObject:value forKey:@"dev_name"];
	else [properties removeObjectForKey:@"dev_name"];
}
- (NSString *) deviceDesc
{ return [properties objectForKey:@"dev_desc"]; }
- (void) setDeviceDesc:(NSString *)value
{
	if (value) [properties setObject:value forKey:@"dev_desc"];
	else [properties removeObjectForKey:@"dev_desc"];
}

- (NSString *) objectName
{ return [properties objectForKey:@"obj_name"]; }
- (void) setObjectName:(NSString *)value
{
	if (value) [properties setObject:value forKey:@"obj_name"];
	else [properties removeObjectForKey:@"obj_name"];
}

- (NSString *) objectDesc
{ return [properties objectForKey:@"obj_desc"]; }
- (void) setObjectDesc:(NSString *)value
{
	if (value) [properties setObject:value forKey:@"obj_desc"];
	else [properties removeObjectForKey:@"obj_desc"];
}

- (NSString *) triggerName
{ return [properties objectForKey:@"trg_name"]; }
- (void) setTriggerName:(NSString *)value
{ [properties setObject:value forKey:@"trg_name"]; }
- (NSString *) triggerDesc
{ return [properties objectForKey:@"trg_desc"]; }
- (void) setTriggerDesc:(NSString *)value
{ [properties setObject:value forKey:@"trg_desc"]; }

- (NSString *) units
{ return [properties objectForKey:@"units"]; }
- (NSString *) unitsDisplayString
{
	if ([self units]) return [self units]; 
	else return @"";
}

- (NSString *) xValue
{ return [properties objectForKey:@"xval"]; }
- (NSString *) xValueDisplayString
{ 
	if ([self xValue]) return [NSString stringWithFormat:@"%@%@", [self xValue], [self unitsDisplayString]]; 
	else return nil;
}
- (void) setXValue:(NSString *)value
{
	[properties setObject:value forKey:@"xval"];
}

- (NSString *) yValue
{ return [properties objectForKey:@"yval"]; }
- (NSString *) yValueDisplayString
{ 
	if ([self yValue]) return [NSString stringWithFormat:@"%@%@", [self yValue], [self unitsDisplayString]]; 
	else return nil;
}
- (void) setYValue:(NSString *)value
{
	[properties setObject:value forKey:@"yval"];
}

- (NSString *) triggerType
{ return [properties objectForKey:@"trg_type"]; }
- (NSString *) triggerTypeInteger
{ return [properties objectForKey:@"trg_type_num"]; }
- (void) setTriggerType:(int)type
{ [properties setObject:[NSString stringWithFormat:@"%i", type] forKey:@"trg_type_num"]; }

- (NSString *) effect
{ return [properties objectForKey:@"effect"]; }

- (NSString *) duration
{ return [properties objectForKey:@"duration"]; }
- (void) setDuration:(NSString *)value
{ [properties setObject:value forKey:@"duration"]; }

- (NSString *) adminState
{ return [properties objectForKey:@"adminstate"]; }
- (void) setAdminState:(int)value
{ [properties setObject:[NSString stringWithFormat:@"%i", value] forKey:@"adminstate_num"]; }
- (NSString *) adminStateInteger
{ return [properties objectForKey:@"adminstate_num"]; }

- (BOOL) operationInProgress
{ return operationInProgress; } 
- (void) setOperationInProgress:(BOOL)flag
{ operationInProgress = flag; }

- (void) setDelegate:(id)newDelegate
{ delegate = newDelegate; }

- (BOOL) exclusive
{ return exclusive; }
- (void) setExclusive:(BOOL)flag
{ exclusive = flag; }

@synthesize obj;
@synthesize tset;
@synthesize trg;
@synthesize blindUpdate;
@synthesize curXMLElement;
@synthesize curXMLString;
@synthesize delegate;
@end
