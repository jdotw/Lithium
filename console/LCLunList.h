//
//  LCLunList.h
//  Lithium Console
//
//  Created by James Wilson on 12/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXMLRequest.h"
#import "LCLunList.h"
#import "LCEntityDescriptor.h"
#import "LCLun.h"

@interface LCLunList : NSObject 
{
	/* Customer */
	id customer;
	
	/* Volumes */
	NSMutableArray *luns;
	NSMutableDictionary *lunDictionary;	
	NSMutableDictionary *wwnDictionary;
	
	/* XML Variables */
	LCXMLRequest *refreshXMLRequest;
	NSMutableDictionary *curXMLDictionary;
	NSMutableDictionary *xmlLunDictionary;
	NSString *curXMLElement;
	NSMutableString *curXMLString;
	BOOL refreshInProgress;		
	NSString *curWwn;
	int curType;
}

#pragma mark "Constructors"
+ (LCLunList *) lunListForCustomer:(id)initCustomer;
- (LCLunList *) initForCustomer:(id)initCustomer;
- (void) dealloc;

#pragma mark "Refresh methods"
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
- (id) customer;
- (void) setCustomer:(id)newCustomer;
- (NSMutableArray *) luns;
- (void) insertObject:(LCLun *)lun inLunsAtIndex:(unsigned int)index;
- (void) removeObjectFromLunsAtIndex:(unsigned int)index;
- (NSMutableDictionary *) lunDictionary;
- (NSMutableDictionary *) wwnDictionary;
- (BOOL) refreshInProgress;
- (void) setRefreshInProgress:(BOOL)flag;

@property (retain,getter=customer,setter=setCustomer:) id customer;
@property (retain,getter=luns) NSMutableArray *luns;
@property (retain,getter=lunDictionary) NSMutableDictionary *lunDictionary;
@property (retain,getter=wwnDictionary) NSMutableDictionary *wwnDictionary;
@property (retain) LCXMLRequest *refreshXMLRequest;
@property (retain) NSMutableDictionary *curXMLDictionary;
@property (retain) NSMutableDictionary *xmlLunDictionary;
@property (retain) NSString *curXMLElement;
@property (retain) NSMutableString *curXMLString;
@property (getter=refreshInProgress,setter=setRefreshInProgress:) BOOL refreshInProgress;
@property (retain) NSString *curWwn;
@property int curType;
@end
