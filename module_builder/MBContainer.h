//
//  MBContainer.h
//  ModuleBuilder
//
//  Created by James Wilson on 12/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MBEntity.h"
#import "MBOid.h"
#import "MBOidTextField.h"
#import "MBMetric.h"

@interface MBContainer : MBEntity 
{
	IBOutlet MBOidTextField *oidTextField;
	NSMutableArray *triggersets;
	NSMutableDictionary *consoleViewMetrics;
	NSMutableDictionary *webViewMetrics;
}

#pragma mark Constructors
+ (MBContainer *) containerWithIndexOid:(MBOid *)initOid;
+ (MBContainer *) container;
- (MBContainer *) initWithIndexOid:(MBOid *)initOid;
- (MBContainer * ) init;

#pragma mark "Console View Accessors"
- (NSMutableDictionary *) consoleViewMetrics;
- (void) setConsoleViewMetrics:(NSMutableDictionary *)dict;

#pragma mark "Web View Accessors"
- (NSMutableDictionary *) webViewMetrics;
- (void) setWebViewMetrics:(NSMutableDictionary *)dict;
- (NSNumber *) webViewMode;
- (void) setWebViewMode:(NSNumber *)number;

#pragma mark Accessors
- (MBOid *) oid;
- (void) setOid:(MBOid *)oid;
- (NSNumber *) mode;
- (void) setMode:(NSNumber *)number;
- (BOOL) hasIndexOid;
- (void) setHasIndexOid:(BOOL)flag;
- (NSMutableArray *) triggersets;
- (void) setTriggersets:(NSMutableArray *)array;
- (void) insertObject:(id)obj inTriggersetsAtIndex:(unsigned int)index;
- (void) removeObjectFromTriggersetsAtIndex:(unsigned int)index;
- (void) removeObjectFromTriggersets:(id)obj;


@end
