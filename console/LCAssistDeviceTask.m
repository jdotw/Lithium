//
//  LCAssistDeviceTask.m
//  Lithium Console
//
//  Created by James Wilson on 26/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCAssistDeviceTask.h"

#import "LCDeviceEditController.h"

@implementation LCAssistDeviceTask

#pragma mark "Constructors"

+ (LCAssistDeviceTask *) taskForVendor:(NSString *)initVendor 
							deviceType:(NSString *)initType
						requiredFields:(NSString *)initRequired
						withController:(LCAssistController *)initController
{
	return [[[LCAssistDeviceTask alloc] initForVendor:initVendor
										   deviceType:initType
									   requiredFields:initRequired
									   withController:initController] autorelease];
}

- (LCAssistDeviceTask *) initForVendor:(NSString *)initVendor
							deviceType:(NSString *)initType
						requiredFields:(NSString *)initRequired
						withController:(LCAssistController *)initController
{
	[super init];

	self.vendor = initVendor;
	self.deviceType = initType;
	self.requiredFieldsIdentifier = initRequired;
	self.controller = initController;
	self.thumbnail = [NSImage imageNamed:@"tn_generic.png"];
	
	return self;
}

#pragma mark "UI Actions"

- (IBAction) addDeviceClicked:(id)sender
{
	if (![controller selectedSite]) return;
	
	LCDeviceEditController *editController;
	editController = [[LCDeviceEditController alloc] initForNewDeviceAtSite:(LCSite *)[controller selectedSite] usingModule:[self vendor]];
	
	[NSApp beginSheet:[editController window]
	   modalForWindow:[controller window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

#pragma mark "Accessors"

@synthesize controller;
@synthesize enabled;
@synthesize vendor;
@synthesize deviceType;
@synthesize blurb;
@synthesize requiredFieldsIdentifier;
@synthesize thumbnail;

- (NSPredicate *) deviceFilter
{ return [NSPredicate predicateWithFormat:@"vendor = %@", [self vendor]]; }

@end
