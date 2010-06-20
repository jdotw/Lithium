//
//  LCActivity.h
//  Lithium Console
//
//  Created by James Wilson on 30/10/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

@interface LCActivity : NSObject 
{
	unsigned long activityID;
	
	id customer;
	id delegate;
	
	BOOL inProgress;
	SEL stopSelector;
	NSMutableDictionary *properties;
}

+ (LCActivity *) activityWithDescription:(NSString *)initDescription
							 forCustomer:(id)initCustomer
								delegate:(id)initDelegate
							stopSelector:(SEL)initStopSelector;
- (LCActivity *) initWithDescription:(NSString *)initDescription
						 forCustomer:(id)initCustomer
							delegate:(id)initDelegate
						stopSelector:(SEL)initStopSelector;

#pragma mark "Activity Manipulation"

- (void) invalidate;

#pragma mark "Accessor Methods"

- (NSMutableDictionary *) properties;

- (NSString *) description;
- (void) setDescription:(NSString *)string;

- (id) customer;
- (void) setCustomer:(id)entity;

- (id) delegate;
- (void) setDelegate:(id)obj;

- (SEL) stopSelector;
- (void) setStopSelector:(SEL)selector;

- (NSString *) status;
- (void) setStatus:(NSString *)string;

- (BOOL) inProgress;
- (void) setInProgress:(BOOL)flag;

- (unsigned long) activityID;
- (void) setActivityID:(unsigned long)idNumber;

- (NSNumber *) priority;
- (void) setPriority:(NSNumber *)priority;

@property (getter=activityID,setter=setActivityID:) unsigned long activityID;
@property (assign,getter=customer,setter=setCustomer:) id customer;
@property (assign,getter=delegate,setter=setDelegate:) id delegate;
@property (getter=inProgress,setter=setInProgress:) BOOL inProgress;
@property (getter=stopSelector,setter=setStopSelector:) SEL stopSelector;
@property (retain,getter=properties) NSMutableDictionary *properties;
@end
