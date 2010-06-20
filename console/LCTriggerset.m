//
//  LCTriggerset.m
//  Lithium Console
//
//  Created by James Wilson on 6/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCTriggerset.h"
#import "LCTriggersetTrigger.h"

#import "LCTriggersetAppRule.h"

@implementation LCTriggerset

#pragma mark "Initialisation"

+ (LCTriggerset *) triggersetWithObject:(LCEntity *)initObject
{
	return [[[LCTriggerset alloc] initWithObject:initObject] autorelease];
}

- (LCTriggerset *) initWithObject:(LCEntity *)initObject
{
	[super init];
	
	/* Set variables */
	obj = [initObject retain];
	properties = [[NSMutableDictionary dictionary] retain];
	triggers = [[NSMutableArray array] retain];
	triggerDictionary = [[NSMutableDictionary dictionary] retain];
	appRules = [[LCTriggersetAppRuleList listWithObject:obj triggerset:self] retain];
	
	/* Drop-Down Choices */
	siteChoices = [[NSArray arrayWithObjects:@"*ALL*", [[obj site] displayString], nil] retain];
	devChoices = [[NSArray arrayWithObjects:@"*ALL*", [[obj device] displayString], nil] retain];
	objChoices = [[NSArray arrayWithObjects:@"*ALL*", [obj displayString], nil] retain];
	appRuleEffects = [[NSArray arrayWithObjects:@"Do NOT Apply Trigger Set", @"Apply Trigger Set", nil] retain];
	valRuleEffects = [[NSArray arrayWithObjects:@"Trigger Enabled", @"Trigger Disabled", nil] retain];
	
	return self;
}

- (void) dealloc
{
	[siteChoices release];
	[devChoices release];
	[objChoices release];
	[appRuleEffects release];
	[valRuleEffects release];
	[obj release];
	[properties release];
	[triggers release];
	[triggerDictionary release];
	[super dealloc];
}

#pragma mark "Applied/Not Applied Methods"

- (BOOL) applied
{ 
	if ([[properties objectForKey:@"applied_flag"] intValue] == 1) return YES; 
	else return NO;
}

- (void) setApplied:(BOOL)flag
{
	if (flag)
	{ [properties setObject:@"1" forKey:@"applied_flag"]; }
	else
	{ [properties setObject:@"0" forKey:@"applied_flag"]; }
	
	LCTriggersetAppRule *rule = [[LCTriggersetAppRule alloc] initWithObject:obj
											triggerset:self
										  criteriaSite:[obj site]
										criteriaDevice:[obj device]
										criteriaObject:obj
									 criteriaApplyFlag:[self applied]];	
	[rule setExclusive:YES];
	[rule update];
}

#pragma mark "Trigger Bump Methods"

- (BOOL) triggerBumpEligable
{
	BOOL hasLessThan = YES;
	BOOL hasGreaterThan = YES;
	
	if (hasLessThan && !hasGreaterThan) return YES;
	else if (!hasLessThan && hasGreaterThan) return YES;
	else return NO;
}

#pragma mark "Accessors"

- (NSMutableDictionary *) properties
{ return properties; }

- (NSMutableArray *) triggers
{ return triggers; }
- (void) insertObject:(id)trigger inTriggersAtIndex:(unsigned int)index
{
	[triggerDictionary setObject:trigger forKey:[trigger name]];
	[triggers insertObject:trigger atIndex:index];
}
- (void) removeObjectFromTriggersAtIndex:(unsigned int)index
{
	if ([triggers objectAtIndex:index])
	{ [triggerDictionary removeObjectForKey:[[triggers objectAtIndex:index] name]]; }
	[triggers removeObjectAtIndex:index];
}
- (NSMutableDictionary *) triggerDictionary
{ return triggerDictionary; }

- (LCTriggersetAppRuleList *) appRules
{ 
	if (![appRules initialRefreshCompleted])
	{ [appRules highPriorityRefresh]; }
	return appRules; 
}

- (NSString *) name
{ return [properties objectForKey:@"name"]; }
- (void) setName:(NSString *)name
{ [properties setObject:name forKey:@"name"]; }

- (NSString *) desc
{ return [properties objectForKey:@"desc"]; }

- (NSString *) metricName
{ return [properties objectForKey:@"metric_name"]; }

- (NSString *) metricDesc
{ return [properties objectForKey:@"metric_desc"]; }

- (LCEntity *) metric
{ return [[obj childrenDictionary] objectForKey:[self metricName]]; }

- (NSArray *) siteChoices
{ return siteChoices; }

- (NSArray *) devChoices
{ return devChoices; }

- (NSArray *) objChoices
{ return objChoices; }

- (NSArray *) appRuleEffects
{ return appRuleEffects; }

- (NSArray *) valRuleEffects
{ return valRuleEffects; }

@synthesize obj;
@synthesize properties;
@synthesize triggers;
@synthesize triggerDictionary;
@synthesize siteChoices;
@synthesize devChoices;
@synthesize objChoices;
@synthesize appRuleEffects;
@synthesize valRuleEffects;
@end
