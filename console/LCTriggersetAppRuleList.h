//
//  LCTriggersetAppRuleList.h
//  Lithium Console
//
//  Created by James Wilson on 6/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCTriggersetAppRule.h"
#import "LCXMLRequest.h"

@interface LCTriggersetAppRuleList : NSObject 
{
	/* Variables */
	LCEntity *obj;
	id tset;
	NSMutableArray *rules;
	NSMutableDictionary *ruleDictionary;
	
	/* Refresh */
	LCXMLRequest *refreshXMLRequest;
	LCTriggersetAppRule *curRule;
	NSString *curXMLElement;
	NSMutableString *curXMLString;
	BOOL refreshInProgress;	
	BOOL initialRefreshCompleted;
	NSMutableDictionary *xmlRuleDict;
	
	/* Delegate */
	id delegate;
}

#pragma mark "Constructors"
+ (LCTriggersetAppRuleList *) listWithObject:(LCEntity *)initObject triggerset:(id)initTriggerset;
- (LCTriggersetAppRuleList *) initWithObject:(LCEntity *)initObject triggerset:(id)initTriggerset;
- (LCTriggersetAppRuleList *) init;
- (void) dealloc;

#pragma mark "Refresh Methods"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;
- (void) XMLRequestPreParse:(id)sender;
- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict ;
- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string ;
- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname;
- (void) XMLRequestFinished:(LCXMLRequest *)sender;

#pragma mark "Accessors"
- (NSArray *) rules;
- (void) insertObject:(id)rule inRulesAtIndex:(unsigned int)index;
- (void) removeObjectFromRulesAtIndex:(unsigned int)index;
- (void) removeRule:(LCTriggersetAppRule *)rule;
- (NSMutableDictionary *) ruleDictionary;
- (BOOL) refreshInProgress;
- (void) setRefreshInProgress:(BOOL)flag;
- (BOOL) initialRefreshCompleted;
- (void) setInitialRefreshCompleted:(BOOL)flag;
- (void) setDelegate:(id)newDelegate;

@property (retain) LCEntity *obj;
@property (retain) id tset;
@property (retain,getter=ruleDictionary) NSMutableDictionary *ruleDictionary;
@property (retain) LCXMLRequest *refreshXMLRequest;
@property (retain) LCTriggersetAppRule *curRule;
@property (retain) NSString *curXMLElement;
@property (retain) NSMutableString *curXMLString;
@property (getter=refreshInProgress,setter=setRefreshInProgress:) BOOL refreshInProgress;
@property (getter=initialRefreshCompleted,setter=setInitialRefreshCompleted:) BOOL initialRefreshCompleted;
@property (retain) NSMutableDictionary *xmlRuleDict;
@property (assign,setter=setDelegate:) id delegate;
@end
