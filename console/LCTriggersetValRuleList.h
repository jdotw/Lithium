//
//  LCTriggersetValRuleList.h
//  Lithium Console
//
//  Created by James Wilson on 6/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXMLRequest.h"
#import "LCTriggersetValRule.h"

@interface LCTriggersetValRuleList : NSObject 
{
	/* Variables */
	LCEntity *obj;
	id tset;
	id trg;
	NSMutableArray *rules;
	NSMutableDictionary *ruleDictionary;
	
	/* Refresh */
	LCXMLRequest *refreshXMLRequest;
	LCTriggersetValRule *curRule;
	NSString *curXMLElement;
	NSMutableString *curXMLString;
	BOOL refreshInProgress;	
	BOOL initialRefreshCompleted;
	NSMutableDictionary *xmlRuleDict;
	
	/* Delegate */
	id delegate;
}

#pragma mark "Constructors"
+ (LCTriggersetValRuleList *) listWithObject:(LCEntity *)initObject triggerset:(id)initTriggerset trigger:(id)initTrigger;
- (LCTriggersetValRuleList *) initWithObject:(LCEntity *)initObject triggerset:(id)initTriggerset trigger:(id)initTrigger;
- (void) dealloc;

#pragma mark "Refresh Methods"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;
- (void) XMLRequestPreParse:(id)sender;
- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict;
- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string ;
- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname;
- (void) XMLRequestFinished:(LCXMLRequest *)sender;

#pragma mark "Accessors"
- (NSArray *) rules;
- (void) insertObject:(id)rule inRulesAtIndex:(unsigned int)index;
- (void) removeObjectFromRulesAtIndex:(unsigned int)index;
- (void) removeRule:(LCTriggersetValRule *)rule;
- (NSMutableDictionary *) ruleDictionary;
- (BOOL) refreshInProgress;
- (void) setRefreshInProgress:(BOOL)flag;
- (BOOL) initialRefreshCompleted;
- (void) setInitialRefreshCompleted:(BOOL)flag;
- (void) setDelegate:(id)newDelegate;


@property (nonatomic,retain) LCEntity *obj;
@property (nonatomic,retain) id tset;
@property (nonatomic,retain) id trg;
@property (nonatomic,retain,getter=ruleDictionary) NSMutableDictionary *ruleDictionary;
@property (nonatomic,retain) LCXMLRequest *refreshXMLRequest;
@property (nonatomic,retain) LCTriggersetValRule *curRule;
@property (nonatomic,retain) NSString *curXMLElement;
@property (nonatomic,retain) NSMutableString *curXMLString;
@property (nonatomic,getter=refreshInProgress,setter=setRefreshInProgress:) BOOL refreshInProgress;
@property (nonatomic,getter=initialRefreshCompleted,setter=setInitialRefreshCompleted:) BOOL initialRefreshCompleted;
@property (nonatomic,retain) NSMutableDictionary *xmlRuleDict;
@property (nonatomic,assign,setter=setDelegate:) id delegate;
@end
