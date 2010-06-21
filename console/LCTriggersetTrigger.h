//
//  LCTriggersetTrigger.h
//  Lithium Console
//
//  Created by James Wilson on 6/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCTriggerset.h"
#import "LCEntity.h"
#import "LCTriggersetValRuleList.h"

@interface LCTriggersetTrigger : NSObject 
{
	LCTriggerset *tset;
	LCEntity *obj;
	NSMutableDictionary *properties;
	LCTriggersetValRuleList *valRules;
	
}

#pragma mark "Constructors"
+ (LCTriggersetTrigger *) triggerWithObject:(LCEntity *)initObject triggerset:(LCTriggerset *)initTset;
- (LCTriggersetTrigger *) initWithObject:(LCEntity *)initObject triggerset:(LCTriggerset *)initTset;
- (void) dealloc;

#pragma mark "Update Methods"
- (void) sendExclusiveRuleUpdate;

#pragma mark Accessors
- (NSMutableDictionary *) properties;
- (LCEntity *) object;
- (LCTriggerset *) triggerset;
- (LCTriggersetValRuleList *) valRules;
- (NSString *) name;
- (void) setName:(NSString *)string;
- (NSString *) desc;
- (NSString *) triggerType;
- (NSString *) valueType;
- (NSString *) units;
- (NSString *) unitsDisplayString;
- (NSString *) xValue;
- (NSString *) xValueDisplayString;
- (NSString *) yValue;
- (NSString *) yValueDisplayString;
- (NSString *) effect;
- (NSString *) duration;
- (NSString *) adminState;
- (NSString *) adminStateInteger;

@property (nonatomic,retain,getter=triggerset) LCTriggerset *tset;
@property (nonatomic,retain,getter=object) LCEntity *obj;
@property (nonatomic,retain,getter=properties) NSMutableDictionary *properties;
@end
