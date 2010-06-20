//
//  LCIncident.m
//  Lithium Console
//
//  Created by James Wilson on 28/06/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCIncident.h"
#import "LCIncidentViewController.h"
#import "LCXMLRequest.h"
#import "LCCustomer.h"
#import "LCAction.h"

@implementation LCIncident

#pragma mark "Initialisation"

- (NSString *) xmlRootElement
{ return @"incident"; }	

- (LCIncident *) init
{
	[super init];

	actions = [[NSMutableArray array] retain];
	actionDictionary = [[NSMutableDictionary dictionary] retain];
	
	if (!self.xmlTranslation)
	{
		self.xmlTranslation = [NSMutableDictionary dictionary];
		[self.xmlTranslation setObject:@"incidentID" forKey:@"id"];
		[self.xmlTranslation setObject:@"state" forKey:@"state_int"];
		[self.xmlTranslation setObject:@"type" forKey:@"type_int"];
		[self.xmlTranslation setObject:@"caseid" forKey:@"caseID"];
		[self.xmlTranslation setObject:@"startTimestamp" forKey:@"start_sec"];
		[self.xmlTranslation setObject:@"endTimestamp" forKey:@"end_sec"];
		[self.xmlTranslation setObject:@"caseID" forKey:@"caseid"];
		[self.xmlTranslation setObject:@"raisedValueString" forKey:@"raised_valstr"];
		[self.xmlTranslation setObject:@"clearedValueString" forKey:@"cleared_valstr"];
	}
	
	return self;
}

- (void) dealloc 
{
	[startDate release];
	[startDateShortString release];
	[endDate release];
	[endDateShortString release];
	[raisedValueString release];
	[clearedValueString release];
	[entity release];
	[entityDescriptor release];
	[actions release];
	[actionDictionary release];
	
	[super dealloc];
}

#pragma mark "Clearing Method"

- (void) clearIncident
{
	/* "Clears" the incident... */
	LCXMLRequest *clearXMLReq = [[LCXMLRequest requestWithCriteria:[[self entity] customer]
										   resource:[[[self entity] device] resourceAddress]
											 entity:[[self entity] entityAddress]
											xmlname:@"incident_clear"
											 refsec:0
											 xmlout:nil] retain];
	[clearXMLReq setPriority:XMLREQ_PRIO_HIGH];
	[clearXMLReq performAsyncRequest];
	
	/* Force refresh of incident list */
	[[(LCCustomer *) [[self entity] customer] activeIncidentsList] refreshWithPriority:XMLREQ_PRIO_HIGH];
}

#pragma mark "Cancel Pending Actions"

- (void) cancelPendingActionsForIncidents:(NSArray *)incidents
{
	/* Cancels all pending actions for all incidents in the given array */
	
	/* Create outbound XML */
	NSXMLDocument *xmldoc;
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"incident"];
	xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	for (LCIncident *inc in incidents)
	{ [rootnode addChild:[NSXMLNode elementWithName:@"incid" stringValue:[NSString stringWithFormat:@"%lu", inc.incidentID]]];	}
	
	/* Send XML Request */
	LCXMLRequest *cancelXMLReq = [[LCXMLRequest requestWithCriteria:[[self entity] customer]
														  resource:[[[self entity] customer] resourceAddress]
															entity:[[self entity] entityAddress]
														   xmlname:@"action_cancelpending"
															refsec:0
															xmlout:xmldoc] retain];
	[cancelXMLReq setPriority:XMLREQ_PRIO_HIGH];
	[cancelXMLReq performAsyncRequest];
	
	/* Force refresh of incident list */
	[[[[self entity] customer] activeIncidentsList] refreshWithPriority:XMLREQ_PRIO_HIGH];	
}

#pragma mark Actions

@synthesize actions;
@synthesize actionDictionary;
@synthesize hasActions;
@synthesize hasPendingActions;

- (void) insertObject:(id)action inActionsAtIndex:(unsigned int)index
{ 
//	[self willChangeValueForKey:@"actionCount"];
//	[self willChangeValueForKey:@"actionIcon"];
	[actions insertObject:action atIndex:index];
	[actionDictionary setObject:action forKey:[NSNumber numberWithUnsignedLong:[(LCAction *)action taskID]]];
//	[self didChangeValueForKey:@"actionIcon"];
//	[self didChangeValueForKey:@"actionCount"];
}
- (void) removeObjectFromActionsAtIndex:(unsigned int)index
{
//	[self willChangeValueForKey:@"actionIcon"];
//	[self willChangeValueForKey:@"actionCount"];
	[actionDictionary removeObjectForKey:[NSNumber numberWithUnsignedLong:[(LCAction *)[actions objectAtIndex:index] taskID]]];
	[actions removeObjectAtIndex:index];
//	[self didChangeValueForKey:@"actionIcon"];
//	[self didChangeValueForKey:@"actionCount"];
}

- (NSNumber *) actionCount
{ return [NSNumber numberWithInt:[actions count]]; }

- (NSImage *) actionIcon
{
	if ([actions count] > 0)
	{
		if ([self hasPendingActions]) { return [NSImage imageNamed:@"tools_16.tif"]; }
		else { return [NSImage imageNamed:@"tools_grey_16.tif"]; }
	}
	else return nil;
}

#pragma mark "Property Methods"

@synthesize incidentID;
@synthesize caseID;

- (void) setCaseID:(unsigned long)value
{
	caseID = value;
	if (caseID > 0) self.hasCase = YES;
	else self.hasCase = NO;
}

@synthesize state;

- (void) setState:(int)value
{
	state = value;
	if (state == 1) self.isActive = YES;
	else self.isActive = NO;
}

@synthesize type;
@synthesize isActive;
@synthesize isLive;
@synthesize hasCase;

@synthesize startTimestamp;

- (void) setStartTimestamp:(unsigned long)value
{
	startTimestamp = value;
	if (startTimestamp > 0)	self.startDate = [NSDate dateWithTimeIntervalSince1970:(NSTimeInterval)self.startTimestamp];
	else self.startDate = nil;
}

@synthesize startDate;
- (void) setStartDate:(NSDate *)value
{
	[startDate release];
	startDate = [value copy];
	self.startDateShortString = [startDate descriptionWithCalendarFormat:@"%Y-%m-%d %H:%M:%S %z" timeZone:nil locale:nil];	
	[self updateOpenTimeInterval];
}
@synthesize startDateShortString;

@synthesize endTimestamp;

- (void) setEndTimestamp:(unsigned long)value
{
	endTimestamp = value;
	if (endTimestamp > 0) self.endDate = [NSDate dateWithTimeIntervalSince1970:(NSTimeInterval)self.endTimestamp];
	else self.endDate = nil;
}

@synthesize endDate;
- (void) setEndDate:(NSDate *)value
{
	[endDate release];
	endDate = [value copy];
	self.endDateShortString = [endDate descriptionWithCalendarFormat:@"%Y-%m-%d %H:%M:%S %z" timeZone:nil locale:nil];
	[self updateOpenTimeInterval];
}
@synthesize endDateShortString;

@synthesize openTimeInterval;

- (void) updateOpenTimeInterval
{
	if (endDate) self.openTimeInterval = [[self endDate] timeIntervalSinceDate:[self startDate]];
	else self.openTimeInterval = [[NSDate dateWithTimeIntervalSinceNow:0] timeIntervalSinceDate:[self startDate]];
}

@synthesize raisedValueString;
@synthesize clearedValueString;

@synthesize entityDescriptor;
- (void) setEntityDescriptor:(LCEntityDescriptor *)value
{
	[entityDescriptor release];
	entityDescriptor = [value retain];
	self.entity = [entityDescriptor locateEntity:isLive];
}

@synthesize entity;

@synthesize relevanceScore;
@synthesize relatedSelected;
@synthesize relatedSelectionDisabled;

#pragma mark "General Accessors"

- (NSArray *) incidents
{
	/* Stub method to allow the device->incident tree */
	return nil;
}

- (id) viewController
{ return [LCIncidentViewController controllerForIncident:self]; }

- (NSImage *) smallIcon
{ return [NSImage imageNamed:@"event_16.tif"]; }

- (NSString *) customerName
{ return [[[self entity] customer] name]; }

- (NSString *) displayString
{
	return [NSString stringWithFormat:@"%@ %@ %@", 
			[self.entity.object desc],
			[self.entity.metric desc],
			[self.entity.trigger desc]];
}

- (NSString *) typeString
{
	return [NSString stringWithFormat:@"Incident %i", self.incidentID];
}

@end
