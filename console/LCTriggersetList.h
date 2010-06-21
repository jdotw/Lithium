//
//  LCTriggersetList.h
//  Lithium Console
//
//  Created by James Wilson on 6/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCTriggerset.h"
#import "LCTriggersetTrigger.h"
#import "LCXMLRequest.h"

@interface LCTriggersetList : NSObject 
{
	/* Variables */
	LCEntity *obj;
	NSMutableArray *tsets;
	NSMutableDictionary *tsetDictionary;
	
	/* Refresh */
	LCXMLRequest *refreshXMLRequest;
	LCTriggerset *curTset;
	LCTriggersetTrigger *curTrg;
	NSString *curXMLElement;
	NSMutableString *curXMLString;
	NSMutableDictionary *xmlIncidentDictionary;
	BOOL refreshInProgress;
	
	/* Delegate */
	id delegate;
}

#pragma mark "Constructors"
+ (LCTriggersetList *) listWithObject:(LCEntity *)initObject;
- (LCTriggersetList *) initWithObject:(LCEntity *)initObject;
- (LCTriggersetList *) init;
- (void) dealloc;

#pragma mark "Refresh Methods"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;
- (void) XMLRequestPreParse:(id)sender;
- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict;
- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname;

#pragma mark "Accessors"
- (NSArray *) triggersets;
- (void) insertObject:(id)tset inTriggersetsAtIndex:(unsigned int)index;
- (void) removeObjectFromTriggersetsAtIndex:(unsigned int)index;
- (NSDictionary *) triggersetDictionary;
- (BOOL) refreshInProgress;
- (void) setRefreshInProgress:(BOOL)flag;
- (void) setDelegate:(id)newDelegate;

@property (nonatomic,retain) LCEntity *obj;
@property (nonatomic,retain) NSMutableArray *tsets;
@property (nonatomic,retain) NSMutableDictionary *tsetDictionary;
@property (nonatomic,retain) LCXMLRequest *refreshXMLRequest;
@property (nonatomic,retain) LCTriggerset *curTset;
@property (nonatomic,retain) LCTriggersetTrigger *curTrg;
@property (nonatomic,retain) NSString *curXMLElement;
@property (nonatomic,retain) NSMutableString *curXMLString;
@property (nonatomic,retain) NSMutableDictionary *xmlIncidentDictionary;
@property (nonatomic,getter=refreshInProgress,setter=setRefreshInProgress:) BOOL refreshInProgress;
@property (nonatomic,assign,setter=setDelegate:) id delegate;
@end
