//
//  LCTriggersetAppRule.m
//  Lithium Console
//
//  Created by James Wilson on 6/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCTriggersetAppRule.h"
#import "LCTriggerset.h"
#import "LCCustomer.h"

@implementation LCTriggersetAppRule

#pragma mark "Constructors"

+ (LCTriggersetAppRule *) ruleWithObject:(LCEntity *)initObject 
							  triggerset:(id)initTriggerset
							criteriaSite:(LCEntity *)criteriaSite
						  criteriaDevice:(LCEntity *)criteriaDevice
						  criteriaObject:(LCEntity *)criteriaObject
					   criteriaApplyFlag:(BOOL)criteriaApplyFlag
{
	return [[[LCTriggersetAppRule alloc] initWithObject:initObject 
											 triggerset:initTriggerset 
										   criteriaSite:criteriaSite 
										 criteriaDevice:criteriaDevice 
										 criteriaObject:criteriaObject 
									  criteriaApplyFlag:criteriaApplyFlag] autorelease];
}

+ (LCTriggersetAppRule *) ruleWithObject:(LCEntity *)initObject triggerset:(id)initTriggerset
{
	return [[[LCTriggersetAppRule alloc] initWithObject:initObject triggerset:initTriggerset] autorelease];
}

- (LCTriggersetAppRule *) initWithObject:(LCEntity *)initObject 
							  triggerset:(id)initTriggerset
							criteriaSite:(LCEntity *)criteriaSite
						  criteriaDevice:(LCEntity *)criteriaDevice
						  criteriaObject:(LCEntity *)criteriaObject
					   criteriaApplyFlag:(BOOL)criteriaApplyFlag
{
	[self init];
	
	/* Set variables*/
	object = [initObject retain];
	triggerset = [initTriggerset retain];
	
	/* Set criteria */
	if (criteriaSite)
	{
		[self setSiteName:[criteriaSite name]];
		[self setSiteDesc:[criteriaSite desc]];
	}
	if (criteriaDevice)
	{
		[self setDeviceName:[criteriaDevice name]];
		[self setDeviceDesc:[criteriaDevice desc]];
	}
	if (criteriaObject)
	{
		[self setObjectName:[criteriaObject name]];
		[self setObjectDesc:[criteriaObject desc]];
	}
	[self setApply:criteriaApplyFlag];

	return self;
}


- (LCTriggersetAppRule *) initWithObject:(LCEntity *)initObject triggerset:(id)initTriggerset
{
	[self init];
	object = [initObject retain];
	triggerset = [initTriggerset retain];
	return self;
}

- (LCTriggersetAppRule *) init
{
	[super init];
	[self setProperties:[NSMutableDictionary dictionary]];
	return self;
}

- (void) dealloc
{
	[object release];
	[triggerset release];
	[self setProperties:nil];
	[super dealloc];
}

#pragma mark "Update"

- (void) update
{
	if (operationInProgress) return;
	
	/* Create request XML document */
	NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"rule"];
	NSXMLDocument *xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];
	[xmlDoc setVersion:@"1.0"];
	[xmlDoc setCharacterEncoding:@"UTF-8"];
	[rootNode addChild:[NSXMLNode elementWithName:@"tset_name" stringValue:[triggerset name]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"met_name" stringValue:[triggerset metricName]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"id" stringValue:[self ID]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"site_name" stringValue:[self siteName]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"site_desc" stringValue:[self siteDesc]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"dev_name" stringValue:[self deviceName]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"dev_desc" stringValue:[self deviceDesc]]];
	[rootNode addChild:[NSXMLNode elementWithName:@"obj_name" stringValue:[self objectName]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"obj_desc" stringValue:[self objectDesc]]];
	[rootNode addChild:[NSXMLNode elementWithName:@"applyflag" stringValue:[self applyInteger]]]; 
	if (exclusive) [rootNode addChild:[NSXMLNode elementWithName:@"exclusive" stringValue:@"1"]];
	
	/* Create XML Request */
	LCXMLRequest *updateXMLRequest = [[LCXMLRequest requestWithCriteria:[object customer]
												  resource:[[object device] resourceAddress] 
													entity:[object entityAddress] 
												   xmlname:@"triggerset_apprule_update"
													refsec:0 
													xmlout:xmlDoc] retain];
	if (!blindUpdate)
	{
		[updateXMLRequest setDelegate:self];
		[updateXMLRequest setXMLDelegate:self];
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
	if (operationInProgress) return;

	/* Create request XML document */
	NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"rule"];
	NSXMLDocument *xmlDoc = [NSXMLDocument documentWithRootElement:rootNode];
	[xmlDoc setVersion:@"1.0"];
	[xmlDoc setCharacterEncoding:@"UTF-8"];
	[rootNode addChild:[NSXMLNode elementWithName:@"tset_name" stringValue:[triggerset name]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"met_name" stringValue:[triggerset metricName]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"id" stringValue:[self ID]]]; 
	
	
	/* Create XML Request */
	LCXMLRequest *deleteXMLRequest = [[LCXMLRequest requestWithCriteria:[object customer]
															   resource:[[object device] resourceAddress] 
																 entity:[object entityAddress] 
																xmlname:@"triggerset_apprule_delete"
																 refsec:0 
																 xmlout:xmlDoc] retain];
	[deleteXMLRequest setDelegate:self];
	[deleteXMLRequest setXMLDelegate:self];
	[deleteXMLRequest setPriority:XMLREQ_PRIO_HIGH];
	[deleteXMLRequest performAsyncRequest];
	
	[self setOperationInProgress:YES];
	deleted = YES;
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
	
	/* Refresh device */
	[[object device] highPriorityRefresh];
	[[(LCCustomer *) [object customer] activeIncidentsList] highPriorityRefresh];
	
	/* Call delegate */
	if (delegate) 
	{
		SEL finishedsel = NSSelectorFromString(@"appRuleOperationFinished:");
		if (finishedsel && [delegate respondsToSelector:finishedsel])
		{ [delegate performSelector:finishedsel withObject:self]; }
	}
	
	/* Set refresh flag */
	[self setOperationInProgress:NO];
	
	/* CAUTION... object may not exist anymore at this point */
}

#pragma mark "Direct-Edit Methods"

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
		[self setSiteName:[[object site] name]];
		[self setSiteDesc:[[object site] desc]];
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
		[self setDeviceName:[[object device] name]];
		[self setDeviceDesc:[[object device] desc]];
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
		[self setObjectName:[object name]];
		[self setObjectDesc:[object desc]];
	}
	[self updateBlind];
}

- (int) effectIndex
{ 
	return [[properties objectForKey:@"applyflag"] intValue];
}

- (void) setEffectIndex:(int)index
{
	[properties setObject:[NSString stringWithFormat:@"%i", index] forKey:@"applyflag"];
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

- (id) triggerset
{ return triggerset; }

- (LCEntity *) object
{ return object; }

- (NSString *) ID
{ return [properties objectForKey:@"id"]; }

- (NSString *) siteName
{ return [properties objectForKey:@"site_name"]; }
- (void) setSiteName:(NSString *)name
{
	if (name) [properties setObject:name forKey:@"site_name"];
	else [properties removeObjectForKey:@"site_name"]; 
}
- (NSString *) siteDesc
{ return [properties objectForKey:@"site_desc"]; }
- (void) setSiteDesc:(NSString *)desc
{
	if (desc) [properties setObject:desc forKey:@"site_desc"];
	else [properties removeObjectForKey:@"site_desc"]; 
}

- (NSString *) deviceName
{ return [properties objectForKey:@"dev_name"]; }
- (void) setDeviceName:(NSString *)name
{
	if (name) [properties setObject:name forKey:@"dev_name"];
	else [properties removeObjectForKey:@"dev_name"]; 
}
- (NSString *) deviceDesc
{ return [properties objectForKey:@"dev_desc"]; }
- (void) setDeviceDesc:(NSString *)desc
{
	if (desc) [properties setObject:desc forKey:@"dev_desc"];
	else [properties removeObjectForKey:@"dev_desc"]; 
}

- (NSString *) objectName
{ return [properties objectForKey:@"obj_name"]; }
- (void) setObjectName:(NSString *)name
{
	if (name) [properties setObject:name forKey:@"obj_name"];
	else [properties removeObjectForKey:@"obj_name"]; 
}
- (NSString *) objectDesc
{ return [properties objectForKey:@"obj_desc"]; }
- (void) setObjectDesc:(NSString *)desc
{
	if (desc) [properties setObject:desc forKey:@"obj_desc"];
	else [properties removeObjectForKey:@"obj_desc"]; 
}

- (BOOL) apply
{ return (BOOL) [[properties objectForKey:@"applyflag"] intValue]; }
- (void) setApply:(BOOL)flag
{
	if (flag)
	{ [properties setObject:@"1" forKey:@"applyflag"]; }
	else
	{ [properties setObject:@"0" forKey:@"applyflag"]; }
}

- (NSString *) applyInteger
{ return [properties objectForKey:@"applyflag"]; }

- (NSString *) effect
{
	if ([self apply])
	{ return @"Apply trigger set"; }
	else
	{ return @"Do NOT apply trigger set"; }
}

- (BOOL) operationInProgress
{ return operationInProgress; } 
- (void) setOperationInProgress:(BOOL)flag
{ 
	operationInProgress = flag; 
	if (operationInProgress)
	{ [self retain]; }
	else
	{ [self release]; }
}

- (void) setDelegate:(id)newDelegate
{ delegate = newDelegate; }

- (BOOL) exclusive
{ return exclusive; }

- (void) setExclusive:(BOOL)flag
{ exclusive = flag; }

@synthesize object;
@synthesize triggerset;
@synthesize curXMLElement;
@synthesize curXMLString;
@synthesize delegate;
@synthesize blindUpdate;
@synthesize deleted;
@end
