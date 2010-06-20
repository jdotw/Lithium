//
//  LCProcessList.m
//  LCAdminTools
//
//  Created by Liam Elliott on 18/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCProcessList.h"
#import <assert.h>
#import <errno.h>
#import <stdbool.h>
#import <stdlib.h>
#import <stdio.h>
#import <sys/sysctl.h>
#import <sys/proc.h>

static LCProcessList *masterProcessList = nil;

@implementation LCProcessList

#pragma mark "Constructors"

+ (LCProcessList *) masterList 
{ return masterProcessList; }

+ (LCProcessList *) processList
{
	return [[[LCProcessList alloc] init] autorelease];
}

-(LCProcessList *)init
{
	[super init];
	[self getProcessList];
	masterProcessList = self;
	return self;
}

-(void)dealloc
{
	[totalLithiumProcesses release];
	[totalRRDtoolProcesses release];
	[totalPostgresProcesses release];
	[super dealloc];
}

#pragma mark "Process List Methods"

-(int) getProcessList
{
	int err;
	kinfo_proc **procList;
	size_t *procCount;
	kinfo_proc *result;
	Boolean done;
	const int name[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL,0};
	
	procList = malloc(sizeof(kinfo_proc *));
	*procList = nil;
	procCount = malloc(sizeof(size_t *));
	
	size_t length;
	
	assert(procList != NULL);
	assert(*procList==NULL);
	assert(procCount != NULL);
	
	*procCount = 0;
	
	result = NULL;
	done = false;
	
	do 
	{
		assert(result == NULL);
		
		length = 0;
		err = sysctl((int *)name, (sizeof(name) / sizeof(*name)) - 1, NULL, &length, NULL,0);
		
		if (err == -1) 
		{
			err = errno;
		}
		
		if (err == 0) 
		{
			result = malloc(length);
			if (result == NULL) 
			{
				err = ENOMEM;
			}
		}
		
		if (err == 0)
		{
			err = sysctl((int *)name, (sizeof(name) / sizeof (*name)) -1, result, &length,NULL,0);
			
			if (err == -1) 
			{
				err = errno;
			}
			
			if (err == 0) 
			{
				done = true;
			}
			else if (err == ENOMEM) 
			{
				assert(result != NULL);
				free(result);
				result = NULL;
				err = 0;
			}
		}
	} 
	while (err == 0 && !done);
	
	if (err != 0 && result != NULL) 
	{
		free(result);
		result = NULL;
	}
	
	*procList = result;
	if (err ==0) 
	{
		*procCount = length / sizeof(kinfo_proc);
	}
	
	assert((err==0) == (*procList != NULL));
	
	int i;
	kinfo_proc *item = *procList;
	int contructCount = 0;
	int rrdtoolCount = 0;
	int postmasterCount = 0;
	int httpdCount = 0;
	for (i= 0; i < *procCount; i++)
	{
		char *procName = (char *)&item[i].kp_proc.p_comm;
		
		if (strcmp(procName, "lithium") == 0)
		{ contructCount++; }
		else if (strcmp(procName, "lcrrdtool") == 0)
		{ rrdtoolCount++; }
		else if (strcmp(procName, "lcpostgres") == 0)
		{ postmasterCount++; }
		else if (strcmp(procName, "clientd") == 0)
		{ httpdCount++; }
	}

	free (result);
	free (procList);
	free (procCount);
	
	[self setTotalLithiumProcesses:[NSNumber numberWithInt:contructCount]];
	[self setTotalRRDtoolProcesses:[NSNumber numberWithInt:rrdtoolCount]];
	[self setTotalPostgresProcesses:[NSNumber numberWithInt:postmasterCount]];
	[self setTotalApacheProcesses:[NSNumber numberWithInt:httpdCount]];
	
	return err;
}

#pragma mark "Count Accessors"

-(NSNumber *) totalLithiumProcesses 
{ return totalLithiumProcesses; }
-(void) setTotalLithiumProcesses:(NSNumber *)count
{
	[self willChangeValueForKey:@"lithiumRunning"];
	[self willChangeValueForKey:@"lithiumIcon"];
	[self willChangeValueForKey:@"globalIcon"];
	[totalLithiumProcesses release];
	totalLithiumProcesses = [count retain];
	[self didChangeValueForKey:@"lithiumRunning"];
	[self didChangeValueForKey:@"lithiumIcon"];
	[self didChangeValueForKey:@"globalIcon"];
}

-(NSNumber *) totalRRDtoolProcesses 
{ return totalRRDtoolProcesses; }
-(void) setTotalRRDtoolProcesses:(NSNumber *)count
{
	[self willChangeValueForKey:@"rrdtoolRunning"];
	[self willChangeValueForKey:@"rrdtoolIcon"];
	[self willChangeValueForKey:@"globalIcon"];
	[totalRRDtoolProcesses release];
	totalRRDtoolProcesses = [count retain];
	[self didChangeValueForKey:@"rrdtoolRunning"];
	[self didChangeValueForKey:@"rrdtoolIcon"];
	[self didChangeValueForKey:@"globalIcon"];
}

-(NSNumber *) totalPostgresProcesses 
{ return totalPostgresProcesses; }
-(void) setTotalPostgresProcesses:(NSNumber *)count
{
	[self willChangeValueForKey:@"postgresRunning"];
	[self willChangeValueForKey:@"postgresIcon"];
	[self willChangeValueForKey:@"globalIcon"];
	[totalPostgresProcesses release];
	totalPostgresProcesses = [count retain];
	[self didChangeValueForKey:@"postgresRunning"];
	[self didChangeValueForKey:@"postgresIcon"];
	[self didChangeValueForKey:@"globalIcon"];
}

-(NSNumber *) totalApacheProcesses 
{ return totalApacheProcesses; }
-(void) setTotalApacheProcesses:(NSNumber *)count
{
	[self willChangeValueForKey:@"apacheRunning"];
	[self willChangeValueForKey:@"apacheIcon"];
	[self willChangeValueForKey:@"globalIcon"];
	[totalApacheProcesses release];
	totalApacheProcesses = [count retain];
	[self didChangeValueForKey:@"apacheRunning"];
	[self didChangeValueForKey:@"apacheIcon"];
	[self didChangeValueForKey:@"globalIcon"];
}

#pragma mark "Flag Accessors"

- (BOOL) lithiumRunning
{ 
	if ([totalLithiumProcesses intValue] > 0)
	{ return YES; }
	else
	{ return NO; }
}

- (BOOL) rrdtoolRunning
{
	if ([totalRRDtoolProcesses intValue] > 0)
	{ return YES; }
	else
	{ return NO; }
}

- (BOOL) postgresRunning
{ 
	if ([totalPostgresProcesses intValue] > 0)
	{ return YES; }
	else
	{ return NO; }
}

- (BOOL) apacheRunning
{ 
	if ([totalApacheProcesses intValue] > 0)
	{ return YES; }
	else
	{ return NO; }
}

#pragma mark "Icon Accessors"

- (NSImage *) lithiumIcon
{
	if ([self lithiumRunning]) 
	{ return [NSImage imageNamed:@"ok_16.tif"]; }
	else
	{ return [NSImage imageNamed:@"stop_16.tif"]; }
}

- (NSImage *) rrdtoolIcon
{ 
	if ([self rrdtoolRunning])
	{ return [NSImage imageNamed:@"ok_16.tif"]; }
	else
	{ return [NSImage imageNamed:@"stop_16.tif"]; }
}

- (NSImage *) postgresIcon
{
	if ([self postgresRunning])
	{ return [NSImage imageNamed:@"ok_16.tif"]; }
	else
	{ return [NSImage imageNamed:@"stop_16.tif"]; }
}

- (NSImage *) apacheIcon
{
	if ([self apacheRunning])
	{ return [NSImage imageNamed:@"ok_16.tif"]; }
	else
	{ return [NSImage imageNamed:@"stop_16.tif"]; }
}

- (NSImage *) globalIcon
{
	if ([self postgresRunning] && [self lithiumRunning] && [self rrdtoolRunning])
	{ return [NSImage imageNamed:@"ok_48.tif"]; }
	else
	{ return [NSImage imageNamed:@"stop_48.tif"]; }
}

@end
