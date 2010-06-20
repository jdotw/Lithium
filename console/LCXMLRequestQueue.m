//
//  LCXMLRequestQueue.m
//  Lithium Console
//
//  Created by James Wilson on 22/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXMLRequestQueue.h"


@implementation LCXMLRequestQueue

static LCXMLRequestQueue *masterQueue = nil;

#pragma mark "Initialisation"

+ (LCXMLRequestQueue *) queue
{
	return [[[LCXMLRequestQueue alloc] init] autorelease];
}

- (LCXMLRequestQueue *) init
{
	[super init];
	
	immediateQueue = [[NSMutableArray array] retain];
	parrallelQueue = [[NSMutableArray array] retain];
	serialQueue = [[NSMutableArray array] retain];
		
	return self;
}

- (void) dealloc
{
	[immediateQueue release];
	[parrallelQueue release];
	[serialQueue release];
	[super dealloc];
}

#pragma mark "Class Methods"

+ (void) masterInit
{
	if (masterQueue) [masterQueue release];
	
	masterQueue = [[LCXMLRequestQueue queue] retain];
}

+ (LCXMLRequestQueue *) masterQueue
{
	return masterQueue;
}

#pragma mark "Queue Manipulation"

- (void) enqueueRequest:(LCXMLRequest *)request
{
	/* Determine Queue and enqueue object */
	switch ([request priority])
	{
		case XMLREQ_PRIO_HIGH:
			[self insertObject:request inImmediateQueueAtIndex:[[self immediateQueue] count]];
			break;
		case XMLREQ_PRIO_NORMAL:
			[self insertObject:request inParrallelQueueAtIndex:[[self parrallelQueue] count]];
			break;
		case XMLREQ_PRIO_LOW:
			[self insertObject:request inSerialQueueAtIndex:[[self serialQueue] count]];
			break;
	}
}

- (void) dequeueRequest:(LCXMLRequest *)request
{
	/* Determine Queue and dequeue object */
	switch ([request priority])
	{
		case XMLREQ_PRIO_HIGH:
			if ([[self immediateQueue] containsObject:request] == NO) return;
			[self removeObjectFromImmediateQueueAtIndex:[[self immediateQueue] indexOfObject:request]];
			break;
		case XMLREQ_PRIO_NORMAL:
			if ([[self parrallelQueue] containsObject:request] == NO) return;
			[self removeObjectFromParrallelQueueAtIndex:[[self parrallelQueue] indexOfObject:request]];
			break;
		case XMLREQ_PRIO_LOW:
			if ([[self serialQueue] containsObject:request] == NO) return;
			[self removeObjectFromSerialQueueAtIndex:[[self serialQueue] indexOfObject:request]];
			break;
	}
}

- (void) runQueues
{
	LCXMLRequest *req;
	
	/* Immediate queue */
	while ([immediateQueue count] > 0 && (req = [immediateQueue objectAtIndex:0]))
	{
		[req performQueuedRequest];
		immediateInProgress++;
		[immediateQueue removeObjectAtIndex:0];
	}
	
	/* Parrallel queue */
	while (parrellelInProgress < [self maxParrallelRequests] && [parrallelQueue count] > 0 && (req = [parrallelQueue objectAtIndex:0]))
	{
		/* Check if any immediate requests are in progress */
		if (immediateInProgress > 0)
		{
			/* An immediate/high-priotity request is in 
			 * progress. In this case, only 1 parrallell/medium-priority
			 * request may run at any one time
			 */
			if (parrellelInProgress > 0) break;
		}
		
		/* Perform request */
		[req performQueuedRequest];
		parrellelInProgress++;
		[parrallelQueue removeObjectAtIndex:0];
		
	}
	
	/* Serial queue */
	while (serialInProgress < 1 && [serialQueue count] > 0 && (req = [serialQueue objectAtIndex:0]))
	{
		[req performQueuedRequest];
		serialInProgress++;
		[serialQueue removeObjectAtIndex:0];
	}
}

- (void) queuedRequestFinished:(LCXMLRequest *)sender
{
	switch ([sender priority])
	{
		case XMLREQ_PRIO_HIGH:
			immediateInProgress--;
			break;
		case XMLREQ_PRIO_NORMAL:
			parrellelInProgress--;
			break;
		case XMLREQ_PRIO_LOW:
			serialInProgress--;
			break;
	}
	
	[self runQueues];
}

#pragma mark "Accessor Methods"

- (NSMutableArray *) immediateQueue
{ return immediateQueue; }
- (void) insertObject:(id)obj inImmediateQueueAtIndex:(unsigned int)index
{
	[immediateQueue insertObject:obj atIndex:index];
	[NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(runQueues) userInfo:nil repeats:NO];
}
- (void) removeObjectFromImmediateQueueAtIndex:(unsigned int)index
{
	[immediateQueue removeObjectAtIndex:index];
	[NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(runQueues) userInfo:nil repeats:NO];
}

- (NSMutableArray *) parrallelQueue
{ return parrallelQueue; }
- (void) insertObject:(id)obj inParrallelQueueAtIndex:(unsigned int)index
{
	[parrallelQueue insertObject:obj atIndex:index];
	[NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(runQueues) userInfo:nil repeats:NO];
}
- (void) removeObjectFromParrallelQueueAtIndex:(unsigned int)index
{
	[parrallelQueue removeObjectAtIndex:index];
	[NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(runQueues) userInfo:nil repeats:NO];
}

- (NSMutableArray *) serialQueue
{ return serialQueue; }
- (void) insertObject:(id)obj inSerialQueueAtIndex:(unsigned int)index
{
	[serialQueue insertObject:obj atIndex:index];
	[NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(runQueues) userInfo:nil repeats:NO];
}
- (void) removeObjectFromSerialQueueAtIndex:(unsigned int)index
{
	[serialQueue removeObjectAtIndex:index];
	[NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(runQueues) userInfo:nil repeats:NO];
}

- (int) maxParrallelRequests
{
	return [[NSUserDefaults standardUserDefaults] integerForKey:@"xmlMaxParrallellReqs"];
}

@synthesize immediateQueue;
@synthesize parrallelQueue;
@synthesize serialQueue;
@synthesize immediateInProgress;
@synthesize parrellelInProgress;
@synthesize serialInProgress;
@end
