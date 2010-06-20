//
//  LCProcessList.h
//  LCAdminTools
//
//  Created by Liam Elliott on 18/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <stdio.h>

typedef struct kinfo_proc kinfo_proc;

@interface LCProcessList : NSObject 
{
	NSNumber *totalLithiumProcesses;
	NSNumber *totalRRDtoolProcesses;
	NSNumber *totalPostgresProcesses;
	NSNumber *totalApacheProcesses;
}

#pragma mark "Constructors"
+ (LCProcessList *) masterList;
+ (LCProcessList *) processList;
-(LCProcessList *) init;
-(void) dealloc;

#pragma mark "Process List Methods"
-(int) getProcessList;

#pragma mark "Count Accessors"
-(NSNumber *) totalLithiumProcesses;
-(void) setTotalLithiumProcesses:(NSNumber *)count;
-(NSNumber *) totalRRDtoolProcesses;
-(void) setTotalRRDtoolProcesses:(NSNumber *)count;
-(NSNumber *) totalPostgresProcesses;
-(void) setTotalPostgresProcesses:(NSNumber *)count;
-(NSNumber *) totalApacheProcesses;
-(void) setTotalApacheProcesses:(NSNumber *)count;

#pragma mark "Flag Accessors"
- (BOOL) lithiumRunning;
- (BOOL) rrdtoolRunning;
- (BOOL) postgresRunning;
- (BOOL) apacheRunning;

#pragma mark "Icon Accessors"
- (NSImage *) lithiumIcon;
- (NSImage *) rrdtoolIcon;
- (NSImage *) postgresIcon;
- (NSImage *) apacheIcon;

@end
