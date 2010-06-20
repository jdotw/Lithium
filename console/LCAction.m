//
//  LCAction.m
//  Lithium Console
//
//  Created by James Wilson on 1/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCAction.h"

#import "LCCustomer.h"
#import "LCIncident.h"

@implementation LCAction

- (NSString *) xmlRootElement
{ return @"action"; }	

#pragma mark "Constructors"

- (LCAction *) initWithCustomer:(LCCustomer *)initCustomer;
{
	[super initWithHostEntity:initCustomer 
					 taskType:@"action"];
	
	return self;
}

- (id) init
{
	self = [super init];
	if (self != nil) 
	{
		/* Set defaults */
		self.enabled = YES;
		self.monday = YES;
		self.tuesday = YES;
		self.wednesday = YES;
		self.thursday = YES;
		self.friday = YES;
		self.saturday = YES;
		self.sunday = YES;
		self.startHour = 7;
		self.endHour = 19;
		self.delay = 0;
		self.reRunDelay = 30;
		self.activationMode = 1;
		self.taskType = @"action";

		/* Setup XML Translation */
		if (!self.xmlTranslation) { self.xmlTranslation = [NSMutableDictionary dictionary]; }
		[self.xmlTranslation setObject:@"activationMode" forKey:@"activation"];
		[self.xmlTranslation setObject:@"delay" forKey:@"delay"];
		[self.xmlTranslation setObject:@"willReRun" forKey:@"rerun"];
		[self.xmlTranslation setObject:@"reRunDelay" forKey:@"rerun_delay"];
		[self.xmlTranslation setObject:@"timeFiltered" forKey:@"time_filter"];
		[self.xmlTranslation setObject:@"dayMask" forKey:@"day_mask"];
		[self.xmlTranslation setObject:@"startHour" forKey:@"start_hour"];
		[self.xmlTranslation setObject:@"endHour" forKey:@"end_hour"];
		[self.xmlTranslation setObject:@"activationMode" forKey:@"activation"];
		[self.xmlTranslation setObject:@"reRunCount" forKey:@"run_count"];
		[self.xmlTranslation setObject:@"runState" forKey:@"runstate"];
		[self.xmlTranslation setObject:@"runStartTimestamp" forKey:@"start_sec"];
		[self.xmlTranslation setObject:@"runEndTimestamp" forKey:@"end_sec"];
	}
	return self;
}
	

- (id) mutableCopyWithZone:(NSZone *)zone
{
	LCAction *copy = [super mutableCopyWithZone:zone];
	
	copy.incident = self.incident;
	
	return copy;
}

- (void) dealloc
{
	[executionString release];
	[rerunString release];
	[behaviourString release];
	[timeToExecutionString release];
	[actionIcon release];
	
	[entityList release];
	[historyList release];
	
	if (executeRequest)
	{
		[executeRequest cancel];
		[executeRequest release];
	}
	[executeStatusString release];
	[executeScriptOutput release];
	[executeStatusIcon release];
	
	if (xmlReq)
	{	
		[xmlReq cancel];
		[xmlReq release];
	}
	[super dealloc];
}

#pragma mark "XML Methods"

- (NSXMLDocument *) xmlDocument
{
	NSXMLDocument *doc = [super xmlDocument];
	NSXMLElement *rootnode = [doc rootElement];

	/* Add entities to XML */
	for (LCEntity *entity in [entityList objects])
	{ 
		[rootnode addChild:[[entity entityDescriptor] xmlNode]]; 
	}
	
	return doc;
}

@synthesize updateDelegate;

#pragma mark "Execute Script"

- (void) execute
{
	/* Executes an incidents action */
	if (!incident) return;
	if (executeRequest) return;
	
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"action"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"taskID" stringValue:[NSString stringWithFormat:@"%i", self.taskID]]];
	[rootnode addChild:[NSXMLNode elementWithName:@"incid" stringValue:[NSString stringWithFormat:@"%i", [incident incidentID]]]];
	
	/* Set status */
	self.executeStatusIcon = nil;
	self.executeStatusString = nil;
	self.executeScriptOutput = nil;
	
	/* Create and perform request */
	executeRequest = [[LCXMLRequest requestWithCriteria:hostEntity.customer
											   resource:[hostEntity.customer resourceAddress]
												 entity:[hostEntity.customer entityAddress]
												xmlname:@"action_execute"
												 refsec:0
												 xmlout:xmldoc] retain];
	[executeRequest setDelegate:self];
	[executeRequest setXMLDelegate:self];
	[executeRequest setPriority:XMLREQ_PRIO_HIGH];	
	self.xmlOperationInProgress = YES;
	[executeRequest performAsyncRequest];
	[[[(LCEntity *)[incident entity] customer] activeIncidentsList] refreshWithPriority:XMLREQ_PRIO_HIGH];
}

@synthesize executeDelegate;
@synthesize executeScriptOutput;
@synthesize executeStatusString;
@synthesize executeStatusIcon;

#pragma mark "Day Accessors"

- (BOOL) monday 
{ 
	if (dayMask & ACTION_DAY_MON) return YES;
	else return NO;
}
- (void) setMonday:(BOOL)value 
{
	if (value) dayMask = dayMask | ACTION_DAY_MON;
	else dayMask = dayMask ^ ACTION_DAY_MON;
}

- (BOOL) tuesday
{
	if (dayMask & ACTION_DAY_TUE) return YES;
	else return NO;
}
- (void) setTuesday:(BOOL)value 
{ 
	if (value) dayMask = dayMask | ACTION_DAY_TUE;
	else dayMask = dayMask ^ ACTION_DAY_TUE;
}

- (BOOL) wednesday
{
	if (dayMask & ACTION_DAY_WED) return YES;
	else return NO;
}	
- (void) setWednesday:(BOOL)value
{ 
	if (value) dayMask = dayMask | ACTION_DAY_WED;
	else dayMask = dayMask ^ ACTION_DAY_WED;
}

- (BOOL) thursday 
{
	if (dayMask & ACTION_DAY_THU) return YES;
	else return NO;
}	
- (void) setThursday:(BOOL)value 
{ 
	if (value) dayMask = dayMask | ACTION_DAY_THU;
	else dayMask = dayMask ^ ACTION_DAY_THU;
}

- (BOOL) friday
{
	if (dayMask & ACTION_DAY_FRI) return YES;
	else return NO;
}	
- (void) setFriday:(BOOL)value
{ 
	if (value) dayMask = dayMask | ACTION_DAY_FRI;
	else dayMask = dayMask ^ ACTION_DAY_FRI;
}

- (BOOL) saturday 
{
	if (dayMask & ACTION_DAY_SAT) return YES;
	else return NO;
}	
- (void) setSaturday:(BOOL)value 
{ 
	if (value) dayMask = dayMask | ACTION_DAY_SAT;
	else dayMask = dayMask ^ ACTION_DAY_SAT;
}

- (BOOL) sunday 
{
	if (dayMask & ACTION_DAY_SUN) return YES;
	else return NO;
}	
- (void) setSunday:(BOOL)value
{ 
	if (value) dayMask = dayMask | ACTION_DAY_SUN;
	else dayMask = dayMask ^ ACTION_DAY_SUN;
}

#pragma mark "XML Parser Delegate Methods"

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Request-specific handling */
	if (sender == executeRequest) 
	{
		/* Check for executeDelegate */
		if (executeDelegate && sender == executeRequest) 
		{
			if ([executeDelegate respondsToSelector:NSSelectorFromString(@"actionExecuted:")])
			{ [executeDelegate performSelector:NSSelectorFromString(@"actionExecuted:") withObject:self]; }
		}	
		
		/* Execute Finished */
		if (!self.executeStatusString || !self.executeStatusIcon)
		{ 
			self.executeStatusString = @"Failed to request execution of Action Script.";
			self.executeStatusIcon = [NSImage imageNamed:@"stop_48.tif"];
		}

		executeRequest = nil;
	}
	
	[super XMLRequestFinished:sender];
}

#pragma mark "Action Property Methods"

- (void) setEnabled:(BOOL)value
{
	enabled = value;
	[self updateBehaviourString];
	
	if (self.taskID && self.hostEntity && !self.xmlUpdatingValues)
	{ [self performXmlUpdate]; }
}

@synthesize activationMode;
- (void) setActivationMode:(int)value;
{
	activationMode = value;
	[self updateBehaviourString];
	[self updateExecutionString];
	[self updateTimeToExecutionString];
}

@synthesize delay;
- (void) setDelay:(int)value
{
	delay = value;
	[self updateBehaviourString];
	[self updateExecutionString];
	[self updateTimeToExecutionString];
}

@synthesize willReRun;
- (void) setWillReRun:(BOOL)value
{
	willReRun = value;
	[self updateBehaviourString];
	[self updateRerunString];
}

@synthesize reRunDelay;
- (void) setReRunDelay:(int)value
{
	reRunDelay = value;
	[self updateBehaviourString];
	[self updateRerunString];
	
}

@synthesize timeFiltered;
@synthesize dayMask;
- (void) setDayMask:(int)value
{
	[self willChangeValueForKey:@"monday"];
	[self willChangeValueForKey:@"tuesday"];
	[self willChangeValueForKey:@"wednesday"];
	[self willChangeValueForKey:@"thursday"];
	[self willChangeValueForKey:@"friday"];
	[self willChangeValueForKey:@"saturday"];
	[self willChangeValueForKey:@"sunday"];
	dayMask = value;
	[self didChangeValueForKey:@"monday"];
	[self didChangeValueForKey:@"tuesday"];
	[self didChangeValueForKey:@"wednesday"];
	[self didChangeValueForKey:@"thursday"];
	[self didChangeValueForKey:@"friday"];
	[self didChangeValueForKey:@"saturday"];
	[self didChangeValueForKey:@"sunday"];	
}

@synthesize startHour;
@synthesize endHour;

@synthesize reRunCount;
- (void) setReRunCount:(int)value
{
	reRunCount = value;
	[self updateTimeToExecutionString];
}

@synthesize runState;
- (void) setRunState:(int)value
{
	runState = value;
	if (runState == 1)
	{ self.actionIcon = [NSImage imageNamed:@"tools_16.tif"]; }
	else
	{ self.actionIcon = [NSImage imageNamed:@"tools_grey_16.tif"]; }
}

@synthesize runStartTimestamp;
- (void) setRunStartTimestamp:(unsigned long)value
{
	runStartTimestamp = value;
	[self updateTimeToExecutionString];
}

@synthesize runEndTimestamp;

- (void) setScriptName:(NSString *)value
{
	[super setScriptName:value];
	[self updateBehaviourString];
}

#pragma mark "Dyanmic Property Methods"

@synthesize actionIcon;

@synthesize executionString;
- (void) updateExecutionString
{
	if (self.activationMode == 0)
	{
		self.executionString = @"Manual";
	}
	else
	{
		if (self.delay < 1)
		{
			self.executionString = @"Automatically/Immediately";
		}
		else
		{
			self.executionString = [NSString stringWithFormat:@"Automatically after %i seconds", self.delay];
		}
	}
}

@synthesize rerunString;

- (void) updateRerunString
{
	if (self.willReRun == 0)
	{
		self.rerunString = @"Run Once";
	}
	else
	{
		self.rerunString = [NSString stringWithFormat:@"Repeat every %i minutes", self.reRunDelay];
	}
}

@synthesize behaviourString;

- (void) updateBehaviourString
{
	if (self.enabled)
	{
		/* Enabled */
		if (self.activationMode == 1)
		{
			/* Auto */
			if (self.delay == 0)
			{
				/* Without Delay */
				self.behaviourString = [NSString stringWithFormat:@"Executes script %@ automatically with no delay.", self.scriptName];
			}
			else
			{
				/* With delay */
				self.behaviourString = [NSString stringWithFormat:@"Executes script %@ automatically after %i seconds delay.",
					self.scriptName, self.delay];
			}
		}
		else
		{
			/* Manual */
			self.behaviourString = [NSString stringWithFormat:@"Executes script %@ on user command.", self.scriptName]; 
		}
		
		/* Check for re-run */
		if (self.willReRun)
		{
			self.behaviourString = [NSString stringWithFormat:@"%@\n\nScript will be re-run every %i minutes after initial execution until the incident is cleared",
				self.behaviourString, self.reRunDelay];			
		}	
	}
	else
	{
		/* Disabled */
		self.behaviourString = @"Action is currently disabled. No script will be run.";
	}
}

@synthesize timeToExecutionString;
- (void) updateTimeToExecutionString
{
	if (self.runStartTimestamp == 0 || self.delay == 0 || self.activationMode == 0 || self.reRunCount > 0)
	{ self.timeToExecutionString = nil; }
	
	NSTimeInterval incidentActive = [[NSDate dateWithTimeIntervalSinceNow:0] timeIntervalSinceDate:[incident startDate]];
	NSTimeInterval wait = ((float) self.delay) - incidentActive;
	
	if (wait >= 0) self.executionString = [NSString stringWithFormat:@"%f", wait];
	else self.executionString = nil;
}

#pragma mark "Other Property Methods"

@synthesize incident;

@synthesize entityList;

@synthesize historyList;
- (LCActionHistoryList *) historyList
{
	if (![historyList hasBeenRefreshed])
	{ [historyList highPriorityRefresh]; }
	return historyList;
}

- (void) setHostEntity:(LCEntity *)value
{
	[super setHostEntity:value];

	/* Create lists */
	self.scriptList = [[[LCActionScriptList alloc] initWithCustomer:hostEntity.customer
															  task:self] autorelease];
	self.entityList = [[[LCActionEntityList alloc] initWithAction:self] autorelease];
	self.historyList = [[[LCActionHistoryList alloc] initWithAction:self] autorelease];	
}

@end
