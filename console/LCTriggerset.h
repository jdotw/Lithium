//
//  LCTriggerset.h
//  Lithium Console
//
//  Created by James Wilson on 6/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCEntity.h"
#import "LCTriggersetAppRuleList.h"

@interface LCTriggerset : NSObject 
{
	LCEntity *obj;
	NSMutableDictionary *properties;
	NSMutableArray *triggers;
	NSMutableDictionary *triggerDictionary;
	LCTriggersetAppRuleList *appRules;
	
	NSArray *siteChoices;
	NSArray *devChoices;
	NSArray *objChoices;
	NSArray *appRuleEffects;
	NSArray *valRuleEffects;
}

#pragma mark "Initialisation"
+ (LCTriggerset *) triggersetWithObject:(LCEntity *)initObject;
- (LCTriggerset *) initWithObject:(LCEntity *)initObject;
- (void) dealloc;

#pragma mark "Accessors"
- (NSMutableDictionary *) properties;
- (NSMutableArray *) triggers;
- (void) insertObject:(id)trigger inTriggersAtIndex:(unsigned int)index;
- (void) removeObjectFromTriggersAtIndex:(unsigned int)index;
- (NSMutableDictionary *) triggerDictionary;
- (LCTriggersetAppRuleList *) appRules;
- (NSString *) name;
- (void) setName:(NSString *)name;
- (NSString *) desc;
- (NSString *) metricName;
- (NSString *) metricDesc;
- (LCEntity *) metric;
- (BOOL) applied;
- (NSArray *) siteChoices;
- (NSArray *) devChoices;
- (NSArray *) objChoices;
- (NSArray *) appRuleEffects;
- (NSArray *) valRuleEffects;

@property (nonatomic,retain) LCEntity *obj;
@property (nonatomic,retain,getter=properties) NSMutableDictionary *properties;
@property (nonatomic,retain,getter=triggers) NSMutableArray *triggers;
@property (nonatomic,retain,getter=triggerDictionary) NSMutableDictionary *triggerDictionary;
@property (nonatomic,retain,getter=siteChoices) NSArray *siteChoices;
@property (nonatomic,retain,getter=devChoices) NSArray *devChoices;
@property (nonatomic,retain,getter=objChoices) NSArray *objChoices;
@property (nonatomic,retain,getter=appRuleEffects) NSArray *appRuleEffects;
@property (nonatomic,retain,getter=valRuleEffects) NSArray *valRuleEffects;
@property (nonatomic,readonly) BOOL triggerBumpEligable;

@end
