//
//  LCXMLRequestQueue.h
//  Lithium Console
//
//  Created by James Wilson on 22/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCXMLRequest.h"

@interface LCXMLRequestQueue : NSObject 
{
	/* Queues */
	NSMutableArray *immediateQueue;
	NSMutableArray *parrallelQueue;
	NSMutableArray *serialQueue;
	
	/* Inprogress */
	int immediateInProgress;
	int parrellelInProgress;
	int serialInProgress;
}

#pragma mark "Initialisation"
+ (LCXMLRequestQueue *) queue;
- (LCXMLRequestQueue *) init;
- (void) dealloc;

#pragma mark "Class Methods"
+ (void) masterInit;
+ (LCXMLRequestQueue *) masterQueue;

#pragma mark "Queue Manipulation"
- (void) enqueueRequest:(LCXMLRequest *)request;
- (void) dequeueRequest:(LCXMLRequest *)request;
- (void) runQueues;
- (void) queuedRequestFinished:(LCXMLRequest *)sender;

#pragma mark "Accessor Methods"
- (NSMutableArray *) immediateQueue;
- (void) insertObject:(id)obj inImmediateQueueAtIndex:(unsigned int)index;
- (void) removeObjectFromImmediateQueueAtIndex:(unsigned int)index;

- (NSMutableArray *) parrallelQueue;
- (void) insertObject:(id)obj inParrallelQueueAtIndex:(unsigned int)index;
- (void) removeObjectFromParrallelQueueAtIndex:(unsigned int)index;

- (NSMutableArray *) serialQueue;
- (void) insertObject:(id)obj inSerialQueueAtIndex:(unsigned int)index;
- (void) removeObjectFromSerialQueueAtIndex:(unsigned int)index;

- (int) maxParrallelRequests;

@property (nonatomic,retain,getter=immediateQueue) NSMutableArray *immediateQueue;
@property (nonatomic,retain,getter=parrallelQueue) NSMutableArray *parrallelQueue;
@property (nonatomic,retain,getter=serialQueue) NSMutableArray *serialQueue;
@property int immediateInProgress;
@property int parrellelInProgress;
@property int serialInProgress;
@end
