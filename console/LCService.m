//
//  LCService.m
//  Lithium Console
//
//  Created by James Wilson on 7/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCService.h"


@implementation LCService

- (NSString *) xmlRootElement
{ return @"service"; }	

#pragma mark "Constructors"

+ (LCService *) serviceWithObject:(LCObject *)initObject
{
	return [[[LCService alloc] initServiceWithObject:initObject] autorelease];	
}

+ (LCService *) newServiceForDevice:(LCDevice *)initDevice
{
	return [[[LCService alloc] initNewServiceForDevice:initDevice] autorelease];
}

- (id) initServiceWithObject:(LCObject *)initObject
{
	/* Create a service object based on 
	 * the specified service 'object' 
	 * (i.e an existing service)
	 */
	
	[super initWithHostEntity:[initObject device]
					 taskType:@"service"];

	self.object = initObject;
	self.taskID = [[initObject name] intValue];
	
	return self;
}

- (id) initNewServiceForDevice:(LCDevice *)initDevice
{
	/* Create a service object for adding
	 * a new service to the specified device
	 */

	[super initWithHostEntity:initDevice
					 taskType:@"service"];
	
	return self;
}

- (id) mutableCopyWithZone:(NSZone *)zone
{
	LCService *copy = [super mutableCopyWithZone:zone];
	
	copy.object = self.object;
	
	return copy;
}

- (void) dealloc
{
	[object release];
	[super dealloc];
}	

#pragma mark "Properties"
@synthesize object;

- (void) setHostEntity:(LCEntity *)value
{
	[super setHostEntity:value];
	
	/* Create lists */
	self.scriptList = [[[LCServiceScriptList alloc] initWithCustomer:hostEntity.customer
																task:self] autorelease];
}

@end
