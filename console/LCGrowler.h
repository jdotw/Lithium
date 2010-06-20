//
//  LCGrowler.h
//  Lithium Console
//
//  Created by James Wilson on 26/03/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCIncident.h"
#import <Growl-WithInstaller/Growl.h>

@interface LCGrowler : NSObject <GrowlApplicationBridgeDelegate>
{

}

#pragma mark Class Methods
+ (LCGrowler *) masterGrowler;
+ (void) setMasterGrowler:(LCGrowler *)instance;

#pragma mark Constructors
- (void) awakeFromNib;

#pragma mark Registration / Delegate
- (NSDictionary *) registrationDictionaryForGrowl;
- (NSString *) applicationNameForGrowl;
- (NSData *) applicationIconDataForGrowl;
- (void) growlNotificationWasClicked:(id)clickContext;

#pragma mark Reporting
- (void) reportIncident:(LCIncident *)incident;
- (void) reportMultipleIncidents:(NSArray *)incidents;
- (void) reportIncidentCleared:(LCIncident *)incident;
- (void) reportMultipleIncidentsCleared:(NSArray *)incidents;

@end
