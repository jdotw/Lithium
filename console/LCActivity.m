//
//  LCActivity.m
//  Lithium Console
//
//  Created by James Wilson on 30/10/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCActivity.h"
#import "LCActivityList.h"

@implementation LCActivity

#pragma mark "Initialisation"

+ (LCActivity *) activityWithDescription:(NSString *)initDescription
							 forCustomer:(id)initCustomer
								delegate:(id)initDelegate
							stopSelector:(SEL)initStopSelector
{
	return [[[LCActivity alloc] initWithDescription:initDescription
									   forCustomer:initCustomer
										  delegate:initDelegate
									  stopSelector:initStopSelector] autorelease];
}

- (LCActivity *) initWithDescription:(NSString *)initDescription
						 forCustomer:(id)initCustomer
							delegate:(id)initDelegate
						stopSelector:(SEL)initStopSelector
{
	[super init];

	properties = [[NSMutableDictionary dictionary] retain];
	
	if (initDescription) [self setDescription:initDescription];
	if (initCustomer) [self setCustomer:initCustomer];
	if (initDelegate) [self setDelegate:initDelegate];
	if (initStopSelector) [self setStopSelector:initStopSelector];
	
	[[LCActivityList masterList] insertObject:self inActivitiesAtIndex:0];
	
	return self;
}

- (void) dealloc
{
	[properties release];
	[super dealloc];
}

#pragma mark "Activity Manipulation"

- (void) invalidate
{
	if ([[[LCActivityList masterList] activities] indexOfObject:self] != NSNotFound)
	{
		unsigned int index = [[[LCActivityList masterList] activities] indexOfObject:self];
		[[LCActivityList masterList] removeObjectFromActivitiesAtIndex:index];
	}
}

#pragma mark "Accessor Methods"

- (NSMutableDictionary *) properties
{ return properties; }

- (NSString *) description
{ return [properties objectForKey:@"description"]; }
- (void) setDescription:(NSString *)string
{ [properties setObject:string forKey:@"description"]; }

- (id) customer
{ return customer; }
- (void) setCustomer:(id)entity
{ customer = entity; }

- (id) delegate
{ return delegate; }
- (void) setDelegate:(id)obj
{ delegate = obj; }

- (SEL) stopSelector
{ return stopSelector; }
- (void) setStopSelector:(SEL)selector
{ stopSelector = selector; }

- (NSString *) status
{ return [properties objectForKey:@"status"]; }
- (void) setStatus:(NSString *)string
{ [properties setObject:string forKey:@"status"]; }

- (BOOL) inProgress 
{ return inProgress; }
- (void) setInProgress:(BOOL)flag
{ inProgress = flag; }

- (unsigned long) activityID
{ return activityID; }
- (void) setActivityID:(unsigned long)idNumber
{ activityID = idNumber; }

- (NSNumber *) priority
{ 
	if ([properties objectForKey:@"priority"])
	{ return [properties objectForKey:@"priority"]; }
	
	return [NSNumber numberWithInt:0];
}
- (void) setPriority:(NSNumber *)priority
{ 
	[properties setObject:priority forKey:@"priority"];
}

@synthesize properties;
@end
