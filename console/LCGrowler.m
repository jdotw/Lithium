//
//  LCGrowler.m
//  Lithium Console
//
//  Created by James Wilson on 26/03/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCGrowler.h"
#import "LCIncident.h"
#import "LCIncidentController.h"
#import "LCCustomer.h"
#import "LCCustomerList.h"

@implementation LCGrowler

#pragma mark Class Methods

static LCGrowler* masterGrowler = nil;

+ (LCGrowler *) masterGrowler
{ return masterGrowler; }

+ (void) setMasterGrowler:(LCGrowler *)instance
{ masterGrowler = instance; }

#pragma mark Constructors

- (void) awakeFromNib
{
	/* Awoken from MainMenu.nib */
	[GrowlApplicationBridge setGrowlDelegate:self];
	[LCGrowler setMasterGrowler:self];
}

#pragma mark Registration / Delegate

- (NSDictionary *) registrationDictionaryForGrowl
{
	NSMutableDictionary *regDict = [NSMutableDictionary dictionary];
	NSMutableArray *defaultNoficiations = [NSMutableArray array];
	[defaultNoficiations addObject:@"New Incident"];
	[defaultNoficiations addObject:@"Cleared Incident"];
	[regDict setObject:defaultNoficiations forKey:GROWL_NOTIFICATIONS_ALL];
	[regDict setObject:defaultNoficiations forKey:GROWL_NOTIFICATIONS_DEFAULT];
	return regDict;
}

- (NSString *) applicationNameForGrowl
{
	return @"Lithium Console";
}

- (NSData *) applicationIconDataForGrowl
{
	return [[NSImage imageNamed:@"Console.icns"] TIFFRepresentation];
}

- (void) growlNotificationWasClicked:(id)clickContext
{
	NSString *str = clickContext;
	NSArray *array = [str componentsSeparatedByString:@":"];
	if ([array count] != 2) return;
	
	LCCustomer *customer = [LCCustomerList masterCustByName:[array objectAtIndex:0]];
	LCIncident *incident = [[[customer activeIncidentsList] incidentDictionary] objectForKey:[array objectAtIndex:1]];
	
	if (incident)
	{
		[[LCIncidentController alloc] initForIncident:incident];
		[NSApp activateIgnoringOtherApps:YES];
	}
}

#pragma mark Reporting

- (void) reportIncident:(LCIncident *)incident
{
	if (![incident entity]) return;
	
	NSString *title = [NSString stringWithFormat:@"%@ %@",
		[[[incident entity] device] displayString], [[[incident entity] container] displayString]];
	NSString *desc = [NSString stringWithFormat:@"%@ %@ %@ %@", 
		[[[incident entity] container] displayString],
		[[[incident entity] object] displayString],
		[[[incident entity] metric] displayString],
		[[[incident entity] trigger] displayString]];
	NSData *iconData;
	switch ([[[incident entityDescriptor] opstate_num] intValue])
	{
		case 0:
			iconData = [[NSImage imageNamed:@"ConsoleGreen.icns"] TIFFRepresentation];
			break;
		case 1:
			iconData = [[NSImage imageNamed:@"ConsoleYellow.icns"] TIFFRepresentation];
			break;
		case 2:
			iconData = [[NSImage imageNamed:@"ConsoleYellow.icns"] TIFFRepresentation];
			break;
		case 3:
			iconData = [[NSImage imageNamed:@"ConsoleRed.icns"] TIFFRepresentation];
			break;
		default:
			iconData = nil;
	}
	[GrowlApplicationBridge notifyWithTitle:title
								description:desc
						   notificationName:@"New Incident"
								   iconData:iconData
								   priority:1
								   isSticky:NO
							   clickContext:[NSString stringWithFormat:@"%@:%i", [[[incident entity] customer] name], [incident incidentID]]];
}

- (void) reportMultipleIncidents:(NSArray *)incidents
{
	NSString *title = [NSString stringWithFormat:@"%i New Incidents", [incidents count]];
	LCIncident *inc;
	unsigned int atRiskCount = 0;
	unsigned int impairedCount = 0;
	unsigned int criticalCount = 0;
	for (inc in incidents)
	{
		if (![inc entity]) continue;
		switch ([[[inc entityDescriptor] opstate_num] intValue])
		{
			case 1:
				atRiskCount++;
				break;
			case 2:
				impairedCount++;
				break;
			case 3:
				criticalCount++;
				break;
		}
	}
	NSString *desc = [NSString stringWithFormat:@"%i at-risk, %i impaired, %i critical", atRiskCount, impairedCount, criticalCount];
	NSData *iconData = [[NSImage imageNamed:@"ConsoleRed.icns"] TIFFRepresentation];
	[GrowlApplicationBridge notifyWithTitle:title
								description:desc
						   notificationName:@"New Incident"
								   iconData:iconData
								   priority:1
								   isSticky:NO
							   clickContext:nil];
}


- (void) reportIncidentCleared:(LCIncident *)incident
{
	if (![incident entity]) return;
	NSString *title = [NSString stringWithFormat:@"Resolved: %@ %@",
		[[[incident entity] device] displayString], [[[incident entity] container] displayString]];
	NSString *desc = [NSString stringWithFormat:@"%@ %@ %@ %@", 
		[[[incident entity] container] displayString],
		[[[incident entity] object] displayString],
		[[[incident entity] metric] displayString],
		[[[incident entity] trigger] displayString]];
	[GrowlApplicationBridge notifyWithTitle:title
								description:desc
						   notificationName:@"Cleared Incident"
								   iconData:[[NSImage imageNamed:@"ConsoleGreen.icns"] TIFFRepresentation]
								   priority:1
								   isSticky:NO
							   clickContext:nil];
	
}

- (void) reportMultipleIncidentsCleared:(NSArray *)incidents
{
	NSString *title = [NSString stringWithFormat:@"Resolved: %i Incidents", [incidents count]];
	[GrowlApplicationBridge notifyWithTitle:title
								description:@""
						   notificationName:@"Cleared Incident"
								   iconData:[[NSImage imageNamed:@"ConsoleGreen.icns"] TIFFRepresentation]
								   priority:1
								   isSticky:NO
							   clickContext:nil];
}

@end
