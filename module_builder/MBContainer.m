//
//  MBContainer.m
//  ModuleBuilder
//
//  Created by James Wilson on 12/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "MBContainer.h"
#import "MBMetric.h"
#import "MBContainerViewController.h"
#import "MBTrigger.h"

@implementation MBContainer

#pragma mark Constructors

+ (MBContainer *) containerWithIndexOid:(MBOid *)initOid
{
	return [[[MBContainer alloc] initWithIndexOid:initOid] autorelease];
}

+ (MBContainer *) container
{ 
	return [[[MBContainer alloc] init] autorelease];
}

- (MBContainer *) initWithIndexOid:(MBOid *)initOid
{
	[self init];
	[self setMode:[NSNumber numberWithInt:1]];
	[self setOid:initOid];
	return self;
}

- (MBEntity *) initWithCoder:(NSCoder *)decoder
{
	[super initWithCoder:decoder];
	[self setWebViewMetrics:[[decoder decodeObjectForKey:@"webViewMetrics"] retain]];
	[self setConsoleViewMetrics:[[decoder decodeObjectForKey:@"consoleViewMetrics"] retain]];
	return self;
}

- (void) encodeWithCoder:(NSCoder *)encoder
{
	[super encodeWithCoder:encoder];
	[encoder encodeObject:webViewMetrics forKey:@"webViewMetrics"];
	[encoder encodeObject:consoleViewMetrics forKey:@"consoleViewMetrics"];
}


- (MBContainer * ) init
{
	[super init];	
	[self setMode:[NSNumber numberWithInt:0]];
	[self setType:[NSNumber numberWithInt:4]];
	return self;
}

#pragma mark "XML Output"

- (NSXMLNode *) xmlNode
{
	/* Create our root node */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"container"];
	
	/* Add root-level properties */
	NSEnumerator *objectEnum = [[properties allKeys] objectEnumerator];
	NSString *key;
	while (key=[objectEnum nextObject])
	{
		id value = [properties objectForKey:key];
		[rootnode addChild:[NSXMLNode elementWithName:key stringValue:value]];
	}
	
	/* Add Console-View properties */
	objectEnum = [[[self consoleViewMetrics] allKeys] objectEnumerator];
	while (key=[objectEnum nextObject])
	{
		NSString *value = [[self consoleViewMetrics] objectForKey:key];
		[rootnode addChild:[NSXMLNode elementWithName:[NSString stringWithFormat:@"cview%@", key]
										  stringValue:value]];
	}
	
	/* Add Web-View properties */
	objectEnum = [[self children] objectEnumerator];
	MBMetric *metric;
	while (metric=[objectEnum nextObject])
	{
		/* Add webview info to container node */
		if ([[self webViewMetrics] objectForKey:[metric desc]])
		{
			[rootnode addChild:[NSXMLNode elementWithName:@"wview" stringValue:[metric desc]]];			
		}
	}
	
	/* Add Trigger Sets */
	objectEnum = [[self children] objectEnumerator];
	while (metric=[objectEnum nextObject])
	{
		/* Check for triggers */
		if ([[metric children] count] < 1) continue;
		
		/* Add trigger set for metric */
		NSXMLElement *tsetNode = (NSXMLElement *) [NSXMLNode elementWithName:@"triggerset"];
		[tsetNode addChild:[NSXMLNode elementWithName:@"met_desc" stringValue:[metric desc]]];
		[rootnode addChild:tsetNode];
		
		/* Add triggers */
		NSEnumerator *trgEnum = [[metric children] objectEnumerator];
		MBTrigger *trg;
		while (trg=[trgEnum nextObject])
		{
			NSXMLElement *trgNode = (NSXMLElement *) [NSXMLNode elementWithName:@"trigger"];
			[trgNode addChild:[NSXMLNode elementWithName:@"xval" stringValue:[trg xValue]]];
			if ([[trg yValue] length] > 0)
			{ [trgNode addChild:[NSXMLNode elementWithName:@"yval" stringValue:[trg yValue]]]; }
			[trgNode addChild:[NSXMLNode elementWithName:@"condition" stringValue:[NSString stringWithFormat:@"%i", [[trg condition] intValue]]]];
			[trgNode addChild:[NSXMLNode elementWithName:@"severity" stringValue:[NSString stringWithFormat:@"%i", [[trg severity] intValue]]]];
			[trgNode addChild:[NSXMLNode elementWithName:@"val_type" stringValue:[NSString stringWithFormat:@"%i", [metric valType]]]];
			[trgNode addChild:[NSXMLNode elementWithName:@"duration" stringValue:[NSString stringWithFormat:@"%i", [[trg duration] intValue]]]];
			if ([trg desc])
			{ [trgNode addChild:[NSXMLNode elementWithName:@"trg_desc" stringValue:[NSString stringWithFormat:@"%@", [trg desc]]]]; }
			[tsetNode addChild:trgNode];
		}
	}		
	
	/* Add Metrics */
	objectEnum = [[self children] objectEnumerator];
	while (metric=[objectEnum nextObject])
	{
		/* Create child node */
		NSXMLElement *node = (NSXMLElement *) [metric xmlNode];
		
		/* Add OID depending on our mode */
		if ([[self mode] intValue] == 0)
		{
			/* Static container, add whole metric OID */
			[node addChild:[NSXMLNode elementWithName:@"oid" stringValue:[[metric oid] oid]]];
		}
		else if ([[self mode] intValue] == 1)
		{
			/* Dynamic container, add base metric OID */
			[node addChild:[NSXMLNode elementWithName:@"oid" stringValue:[[metric oid] baseOid]]];			
		}
		[rootnode addChild:node];
	}
	
	return rootnode;
}

#pragma mark "Console View Accessors"

- (NSMutableDictionary *) consoleViewMetrics
{ return consoleViewMetrics; }

- (void) setConsoleViewMetrics:(NSMutableDictionary *)dict
{
	[[[document undoManager] prepareWithInvocationTarget:self] setConsoleViewMetrics:(id)[self consoleViewMetrics]];
	[consoleViewMetrics release];
	consoleViewMetrics = [dict retain];
	[[document undoManager] setActionName:@"Console Layout Change"];	
}

#pragma mark "Web View Accessors"

- (NSMutableDictionary *) webViewMetrics
{ return webViewMetrics; }

- (void) setWebViewMetrics:(NSMutableDictionary *) dict
{ 
	[[[document undoManager] prepareWithInvocationTarget:self] setWebViewMetrics:(id)[self webViewMetrics]];
	[webViewMetrics release];
	webViewMetrics = [dict retain];
	[[document undoManager] setActionName:@"Web Layout Change"];
}

- (NSNumber *) webViewMode
{ return [properties objectForKey:@"webview_mode"]; }

- (void) setWebViewMode:(NSNumber *)number
{ 
	[[[document undoManager] prepareWithInvocationTarget:self] setWebViewMode:(id)[self webViewMode]];
	[properties setObject:number forKey:@"webview_mode"]; 
	[[document undoManager] setActionName:@"Web Layout Mode Change"];
}

#pragma mark "View Controler"

- (NSViewController *) viewController
{ return [MBContainerViewController viewForContainer:self]; }

#pragma mark "General Accessors"

- (MBOid *) oid
{ return [properties objectForKey:@"oid"]; }

- (void) setOid:(MBOid *)oid
{ 
	if ([[self desc] length] < 1 || [[self desc] isEqualToString:[[self oid] name]])
	{ 
		[self willChangeValueForKey:@"desc"];
		NSArray *parts = [[oid baseName] componentsSeparatedByString:@"::"];
		if ([parts count] > 1)
		{
			if (oid.commonPrefix)
			{
				[properties setObject:oid.commonPrefix forKey:@"desc"]; 
			}
			else
			{
				[properties setObject:[parts objectAtIndex:[parts count]-1] forKey:@"desc"]; 
			}
		}
		else
		{ [properties setObject:[oid baseName] forKey:@"desc"]; }
		[self didChangeValueForKey:@"desc"];
	}
	[[[document undoManager] prepareWithInvocationTarget:self] setOid:(id)[self oid]];
	[properties setObject:oid forKey:@"oid"]; 
	[[document undoManager] setActionName:@"Set Container OID"];
}

- (NSNumber *) mode
{ return [properties objectForKey:@"mode"]; }

- (void) setMode:(NSNumber *)number
{ 
	[(MBContainer *)[[document undoManager] prepareWithInvocationTarget:self] setMode:(NSNumber *)[self mode]];
	[properties setObject:number forKey:@"mode"]; 
	[[document undoManager] setActionName:@"Set Container Mode"];
	if ([[self mode] intValue] == 1)
	{ [self setHasIndexOid:YES]; }
	else
	{ [self setHasIndexOid:NO]; }
	
	for (MBMetric *metric in children)
	{ [metric updateOidDisplayString]; }
}

- (BOOL) hasIndexOid
{ return [[properties objectForKey:@"hasIndexOid"] boolValue]; }

- (void) setHasIndexOid:(BOOL)flag
{ [properties setObject:[NSNumber numberWithBool:flag] forKey:@"hasIndexOid"]; }

- (NSMutableArray *) triggersets
{ return triggersets; }

- (void) setTriggersets:(NSMutableArray *)array
{ 
	[triggersets release];
	triggersets = [array retain];
}

- (void) insertObject:(id)obj inTriggersetsAtIndex:(unsigned int)index
{ 
	[triggersets insertObject:obj atIndex:index];
}

- (void) removeObjectFromTriggersetsAtIndex:(unsigned int)index
{
	[triggersets removeObjectAtIndex:index];
}

- (void) removeObjectFromTriggersets:(id)obj
{
	if ([triggersets indexOfObject:obj] != NSNotFound)
	{ [self removeObjectFromTriggersetsAtIndex:[triggersets indexOfObject:obj]]; }
}

@end
