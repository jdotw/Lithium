//
//  LCPerformController.m
//  LCAdminTools
//
//  Created by James Wilson on 3/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCPerformController.h"

static LCPerformController *masterController;

@implementation LCPerformController

#pragma mark "Initialisation"

- (void) awakeFromNib
{
	/* Set master */
	masterController = self;
	
	/* Create items */
	items = [[NSMutableArray array] retain];
	
	/* Create group items */
	[self willChangeValueForKey:@"items"];
	[self setLithiumGroup:[LCPerformanceGroup performanceGroupNamed:@"Lithium Core"]];
	[self setRrdtoolGroup:[LCPerformanceGroup performanceGroupNamed:@"Metric Data Storage"]];
	[self setPostgresGroup:[LCPerformanceGroup performanceGroupNamed:@"Database"]];
	[self didChangeValueForKey:@"items"];
	
	[NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(refreshProcessList) userInfo:nil repeats:YES];
	[self refreshProcessList];
}

+ (LCPerformController *) masterController
{ return masterController; }

#pragma mark "Refresh process list"

- (void) refreshProcessList
{
	NSTask *task;
    task = [[NSTask alloc] init];
    [task setLaunchPath: @"/bin/ps"];
	
    NSArray *arguments;
    arguments = [NSArray arrayWithObjects: @"auxwww", nil];
    [task setArguments: arguments];
	
    NSPipe *pipe;
    pipe = [NSPipe pipe];
    [task setStandardOutput: pipe];
	
    NSFileHandle *file;
    file = [pipe fileHandleForReading];
	
    [task launch];
	
    NSData *data;
    data = [file readDataToEndOfFile];
	
    NSString *string;
    string = [[NSString alloc] initWithData: data encoding: NSUTF8StringEncoding];

	NSMutableDictionary *processDict = [NSMutableDictionary dictionary];

	[self setVacuumInProgress:NO];
	NSArray *lines = [string componentsSeparatedByString:@"\n"];
	[string autorelease];
	NSEnumerator *lineEnum = [lines objectEnumerator];
	NSString *line;
	while (line=[lineEnum nextObject])
	{
		NSArray *cols = [line componentsSeparatedByString:@" "];
		LCPerformanceProcess *process = [LCPerformanceProcess processWithColumns:cols];
		if (![process processID]) continue;
		[processDict setObject:process forKey:[process processID]];
		const char *line_str = [line cStringUsingEncoding:NSUTF8StringEncoding];
		if (strstr(line_str, "/Library/Lithium/LithiumCore.app/Contents/MacOS/lithium"))
		{
			/* Construct process */
			if ([[[self lithiumGroup] itemDictionary] objectForKey:[process processID]])
			{
				/* Update */
				LCPerformanceProcess *existingProcess = [[[self lithiumGroup] itemDictionary] objectForKey:[process processID]];
				[existingProcess setCpuPercent:[process cpuPercent]];
				[existingProcess setRSize:[process rSize]];
				[existingProcess setVSize:[process vSize]];
			}
			else
			{
				/* New */
				[[self lithiumGroup] insertObject:process inItemsAtIndex:[[[self lithiumGroup] items] count]];				
			}
		}
		else if (strstr(line_str, "lcrrdtool"))
		{
			/* RRDtool Process */
			if ([[[self rrdtoolGroup] itemDictionary] objectForKey:[process processID]])
			{
				/* Update */
				LCPerformanceProcess *existingProcess = [[[self rrdtoolGroup] itemDictionary] objectForKey:[process processID]];
				[existingProcess setCpuPercent:[process cpuPercent]];
				[existingProcess setRSize:[process rSize]];
				[existingProcess setVSize:[process vSize]];
			}
			else
			{
				/* New */
				[[self rrdtoolGroup] insertObject:process inItemsAtIndex:[[[self rrdtoolGroup] items] count]];
			}
		}
		else if (strstr(line_str, "postgres") && strstr(line_str, "lithium"))
		{
			/* Postgres Process */
			if ([[[self postgresGroup] itemDictionary] objectForKey:[process processID]])
			{
				/* Update */
				LCPerformanceProcess *existingProcess = [[[self postgresGroup] itemDictionary] objectForKey:[process processID]];
				[existingProcess setCpuPercent:[process cpuPercent]];
				[existingProcess setRSize:[process rSize]];
				[existingProcess setVSize:[process vSize]];
			}
			else
			{
				[[self postgresGroup] insertObject:process inItemsAtIndex:[[[self postgresGroup] items] count]];
			}
			
			/* Check for VACUUM */
			if (strstr(line_str, "VACUUM"))
			{ [self setVacuumInProgress:YES]; }
		}
	}
	
	/* Check for extinct processes */
	[[self lithiumGroup] checkProcessExistence:processDict];
	[[self postgresGroup] checkProcessExistence:processDict];
	[[self rrdtoolGroup] checkProcessExistence:processDict];
	
	/* Update totals */
	[[self lithiumGroup] updateTotals];
	[[self postgresGroup] updateTotals];
	[[self rrdtoolGroup] updateTotals];
	
	[task release];
}

#pragma mark "Group Accessors"

- (LCPerformanceGroup *) lithiumGroup
{ return lithiumGroup; }

- (void) setLithiumGroup:(LCPerformanceGroup *)item
{ 
	[lithiumGroup release];
	lithiumGroup = [item retain];
	[items addObject:item];
}

- (LCPerformanceGroup *) rrdtoolGroup
{ return rrdtoolGroup; }

- (void) setRrdtoolGroup:(LCPerformanceGroup *)item
{ 
	[rrdtoolGroup release];
	rrdtoolGroup = [item retain];
	[items addObject:item];
}

- (LCPerformanceGroup *) postgresGroup
{ return postgresGroup; }

- (void) setPostgresGroup:(LCPerformanceGroup *)item
{ 
	[postgresGroup release];
	postgresGroup = [item retain];
	[items addObject:item];
}

#pragma mark "Accessors"

- (BOOL) vacuumInProgress
{ return vacuumInProgress; }

- (void) setVacuumInProgress:(BOOL)flag
{ vacuumInProgress = flag; }

@end
