//
//  LCActivityList.h
//  Lithium Console
//
//  Created by James Wilson on 30/10/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCActivity.h"

@interface LCActivityList : NSObject 
{
	unsigned long nextActivityID;
	
	NSMutableArray *activities;
}

#pragma mark "Initialisation"
+ (LCActivityList *) list;
- (LCActivityList *) init;
+ (LCActivityList *) masterInit;

#pragma mark "Accessor Methods"
+ (LCActivityList *) masterList;
- (NSMutableArray *) activities;
- (void) insertObject:(LCActivity *)activity inActivitiesAtIndex:(unsigned int)index;
- (void) removeObjectFromActivitiesAtIndex:(unsigned int)index;
- (unsigned long) allocateActivityID;

@property unsigned long nextActivityID;
@property (retain,getter=activities) NSMutableArray *activities;
@end
