//
//  LCCaseLogEntryList.h
//  Lithium Console
//
//  Created by James Wilson on 14/08/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCaseLogEntry.h"

@interface LCCaseLogEntryList : NSObject 
{
	/* Related Objects */
	id cas;
	id delegate;
	
	/* Properties */
	NSDate *lastUpdated;
	NSString *lastUpdatedShortString;
	
	/* Log Entries */
	NSMutableArray *logEntries;
	NSMutableDictionary *logEntryDictionary;
	
	/* Refresh variables */
	LCXMLRequest *refreshXMLRequest;
	LCCaseLogEntry *curEntry;
	NSMutableString *curXMLString;
	BOOL refreshInProgress;
}

#pragma mark "Refresh methods"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;

#pragma mark "Properties"
@property (retain) id cas;
@property (assign) id delegate;
@property (readonly) NSMutableArray *logEntries;
- (void) insertObject:(id)logentry inLogEntriesAtIndex:(unsigned int)index;
- (void) removeObjectFromLogEntriesAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *logEntryDictionary;
@property (assign) BOOL refreshInProgress;
@property (copy) NSDate *lastUpdated;
@property (copy) NSString *lastUpdatedShortString;

@end
