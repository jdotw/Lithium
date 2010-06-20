//
//  LCXsanList.h
//  Lithium Console
//
//  Created by James Wilson on 11/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXMLRequest.h"
#import "LCXsanVolume.h"
#import "LCEntityDescriptor.h"

@interface LCXsanList : NSObject 
{
	/* Customer */
	id customer;
	
	/* Clients */
	NSMutableArray *clients;
	NSMutableDictionary *clientDictionary;

	/* Controler */
	NSMutableArray *controllers;
	NSMutableDictionary *controllerDictionary;	
	
	/* Volumes */
	NSMutableArray *volumes;
	NSMutableDictionary *volumeDictionary;	
	
	/* XML Variables */
	LCXMLRequest *refreshXMLRequest;
	NSMutableDictionary *curXMLDictionary;
	NSMutableDictionary *xmlControllerDictionary;
	NSMutableDictionary *xmlClientDictionary;
	NSMutableDictionary *xmlVolumeDictionary;
	NSString *curXMLElement;
	NSMutableString *curXMLString;
	BOOL refreshInProgress;		
	BOOL inVolume;
	int computerType;
}

#pragma mark "Constructors"
+ (LCXsanList *) xsanListForCustomer:(id)initCustomer;
- (LCXsanList *) initForCustomer:(id)initCustomer;
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
- (NSMutableArray *) volumes;
- (void) insertObject:(LCXsanVolume *)vol inVolumesAtIndex:(unsigned int)index;
- (void) removeObjectFromVolumesAtIndex:(unsigned int)index;
- (NSMutableArray *) controllers;
- (void) insertObject:(LCEntity *)entity inControllersAtIndex:(unsigned int)index;
- (void) removeObjectFromControllersAtIndex:(unsigned int)index;
- (NSMutableArray *) clients;
- (void) insertObject:(LCEntity *)entity inClientsAtIndex:(unsigned int)index;
- (void) removeObjectFromClientsAtIndex:(unsigned int)index;
- (BOOL) refreshInProgress;
- (void) setRefreshInProgress:(BOOL)flag;

@property (retain,getter=customer,setter=setCustomer:) id customer;
@property (retain,getter=clients) NSMutableArray *clients;
@property (retain) NSMutableDictionary *clientDictionary;
@property (retain,getter=controllers) NSMutableArray *controllers;
@property (retain) NSMutableDictionary *controllerDictionary;
@property (retain,getter=volumes) NSMutableArray *volumes;
@property (retain) NSMutableDictionary *volumeDictionary;
@property (retain) LCXMLRequest *refreshXMLRequest;
@property (retain) NSMutableDictionary *curXMLDictionary;
@property (retain) NSMutableDictionary *xmlControllerDictionary;
@property (retain) NSMutableDictionary *xmlClientDictionary;
@property (retain) NSMutableDictionary *xmlVolumeDictionary;
@property (retain) NSString *curXMLElement;
@property (retain) NSMutableString *curXMLString;
@property (getter=refreshInProgress,setter=setRefreshInProgress:) BOOL refreshInProgress;
@property BOOL inVolume;
@property int computerType;
@end
