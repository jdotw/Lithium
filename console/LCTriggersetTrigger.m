//
//  LCTriggersetTrigger.m
//  Lithium Console
//
//  Created by James Wilson on 6/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCTriggersetTrigger.h"


@implementation LCTriggersetTrigger

#pragma mark "Constructors"

+ (LCTriggersetTrigger *) triggerWithObject:(LCEntity *)initObject triggerset:(LCTriggerset *)initTset
{
	return [[[LCTriggersetTrigger alloc] initWithObject:initObject triggerset:initTset] autorelease];
}

- (LCTriggersetTrigger *) initWithObject:(LCEntity *)initObject triggerset:(LCTriggerset *)initTset
{
	[super init];
	obj = [initObject retain];
	tset = [initTset retain];
	properties = [[NSMutableDictionary dictionary] retain];
	valRules = [[LCTriggersetValRuleList listWithObject:obj triggerset:tset trigger:self] retain];
	
	return self;
}

- (void) dealloc
{
	[obj release];
	[tset release];
	[properties release];
	[super dealloc];
}

#pragma mark "Direct-Alert Methods"

- (BOOL) enabled
{
	if ([[properties objectForKey:@"adminstate_num"] intValue] == 1)
	{ return NO; }
	else
	{ return YES; }
}

- (void) setEnabled:(BOOL)flag
{
	if (flag)
	{ [properties setObject:@"0" forKey:@"adminstate_num"]; }
	else
	{ [properties setObject:@"1" forKey:@"adminstate_num"]; }
	[self sendExclusiveRuleUpdate];
}

- (NSString *) xValue
{ return [properties objectForKey:@"xval"]; }
- (void) setXValue:(NSString *)value
{
	[properties setObject:value forKey:@"xval"];
	[self sendExclusiveRuleUpdate];
}
- (NSString *) xValueDisplayString
{ 
	if ([self xValue]) return [NSString stringWithFormat:@"%@%@", [self xValue], [self unitsDisplayString]]; 
	else return nil;
}

- (NSString *) yValue
{ return [properties objectForKey:@"yval"]; }
- (void) setYValue:(NSString *)value
{
	[properties setObject:value forKey:@"yval"];
	[self sendExclusiveRuleUpdate];	
}
- (NSString *) yValueDisplayString
{ 
	if ([self yValue]) return [NSString stringWithFormat:@"%@%@", [self yValue], [self unitsDisplayString]]; 
	else return nil;
}

- (int) conditionIndex
{ return [[properties objectForKey:@"trgtype_num"] intValue] - 1; }
- (void) setConditionIndex:(int)index
{ 
	[properties setObject:[NSString stringWithFormat:@"%i", index+1]
				   forKey:@"trgtype_num"];
	[self sendExclusiveRuleUpdate];
}

- (int) effectIndex
{ return [[properties objectForKey:@"effect_num"] intValue] - 1; }
- (void) setEffectIndex:(int)index
{ 
	[properties setObject:[NSString stringWithFormat:@"%i", index+1]
				   forKey:@"effect_num"];
	[self sendExclusiveRuleUpdate];
}

- (NSString *) duration
{ return [properties objectForKey:@"duration"]; }
- (void) setDuration:(NSString *)value
{
	[properties setObject:value forKey:@"duration"];
	[self sendExclusiveRuleUpdate];	
}

- (void) sendExclusiveRuleUpdate
{
	LCTriggersetValRule *valRule = [LCTriggersetValRule ruleWithObject:obj
															triggerset:tset
															   trigger:self
														  criteriaSite:[obj site]
														criteriaDevice:[obj device]
														criteriaObject:obj
													   criteriaTrgType:[[properties objectForKey:@"trgtype_num"] intValue]
														criteriaXValue:[properties objectForKey:@"xval"]
														criteriaYValue:[properties objectForKey:@"yval"]
													  criteriaDuration:[properties objectForKey:@"duration"]
													criteriaAdminstate:[[properties objectForKey:@"adminstate_num"] intValue]];
	[valRule setExclusive:YES];
	[valRule update];	
}

#pragma mark Accessors

- (NSMutableDictionary *) properties
{ return properties; }

- (LCEntity *) object
{ return obj; }

- (LCTriggerset *) triggerset
{ return tset; }

- (LCTriggersetValRuleList *) valRules
{
	if (![valRules initialRefreshCompleted])
	{ [valRules highPriorityRefresh]; }
	return valRules; 
}

- (NSString *) name
{ return [properties objectForKey:@"name"]; }
- (void) setName:(NSString *)string
{ [properties setObject:@"name" forKey:string]; }

- (NSString *) desc
{ return [properties objectForKey:@"desc"]; }

- (NSString *) triggerType
{ return [properties objectForKey:@"trgtype"]; }

- (NSString *) valueType
{ return [properties objectForKey:@"valtype"]; }

- (NSString *) units
{ return [properties objectForKey:@"units"]; }
- (NSString *) unitsDisplayString
{
	if ([self units]) return [self units]; 
	else return @"";
}

- (NSString *) effect
{ return [properties objectForKey:@"effect"]; }

- (NSString *) adminState
{ return [properties objectForKey:@"adminstate"]; }

- (NSString *) adminStateInteger
{ return [properties objectForKey:@"adminstate_num"]; }


@synthesize tset;
@synthesize obj;
@synthesize properties;
@end
