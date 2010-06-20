//
//  LCAssistDeviceTask.h
//  Lithium Console
//
//  Created by James Wilson on 26/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCAssistController.h"

@interface LCAssistDeviceTask : NSObject 
{
	LCAssistController *controller;
	
	BOOL enabled;
	NSString *vendor;
	NSString *deviceType;
	NSString *blurb;
	NSString *requiredFieldsIdentifier;
	NSImage *thumbnail;
}

+ (LCAssistDeviceTask *) taskForVendor:(NSString *)initVendor 
							deviceType:(NSString *)initType
						requiredFields:(NSString *)initRequired
						withController:(LCAssistController *)initController;
- (LCAssistDeviceTask *) initForVendor:(NSString *)initVendor
							deviceType:(NSString *)initType
						requiredFields:(NSString *)initRequired
						withController:(LCAssistController *)initController;

#pragma mark "UI Actions"
- (IBAction) addDeviceClicked:(id)sender;

#pragma mark "Accessors"
@property (retain) LCAssistController *controller;
@property (assign) BOOL enabled;
@property (copy) NSString *vendor;
@property (copy) NSString *deviceType;
@property (copy) NSString *blurb;
@property (copy) NSString *requiredFieldsIdentifier;
@property (copy) NSImage *thumbnail;
- (NSPredicate *) deviceFilter;

@end
