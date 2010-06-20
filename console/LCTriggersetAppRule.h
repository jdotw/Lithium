//
//  LCTriggersetAppRule.h
//  Lithium Console
//
//  Created by James Wilson on 6/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCEntity.h"
#import "LCXMLRequest.h"

@interface LCTriggersetAppRule : NSObject 
{
	/* Variables */
	LCEntity *object;
	id triggerset;
	NSMutableDictionary *properties;
	
	/* Refresh */
	NSString *curXMLElement;
	NSMutableString *curXMLString;
	BOOL operationInProgress;
	id delegate;
	BOOL blindUpdate;
	
	/* Exclusive */
	BOOL exclusive;
	
	/* Delete */
	BOOL deleted;
}

#pragma mark Constructors
+ (LCTriggersetAppRule *) ruleWithObject:(LCEntity *)initObject triggerset:(id)initTriggerset;
- (LCTriggersetAppRule *) initWithObject:(LCEntity *)initObject 
							  triggerset:(id)initTriggerset
							criteriaSite:(LCEntity *)criteriaSite
						  criteriaDevice:(LCEntity *)criteriaDevice
						  criteriaObject:(LCEntity *)criteriaObject
					   criteriaApplyFlag:(BOOL)criteriaApplyFlag;
- (LCTriggersetAppRule *) initWithObject:(LCEntity *)initObject triggerset:(id)initTriggerset;
- (LCTriggersetAppRule *) init;
- (void) dealloc;

#pragma mark "Update"
- (void) update;

#pragma mark "Delete"
- (void) delete;

#pragma mark "XML Processing"
- (void) XMLRequestPreParse:(id)sender;
- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict;
- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string;
- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname;
- (void) XMLRequestFinished:(LCXMLRequest *)sender;

#pragma mark "Direct-Edit Methods"
- (int) siteIndex;
- (void) setSiteIndex:(int)index;
- (int) deviceIndex;
- (void) setDeviceIndex:(int)index;
- (int) objectIndex;
- (void) setObjectIndex:(int)index;
- (int) effectIndex;
- (void) setEffectIndex:(int)index;

#pragma mark Accessors
- (NSMutableDictionary *) properties;
- (void) setProperties:(NSMutableDictionary *)newProperties;
- (id) triggerset;
- (LCEntity *) object;
- (NSString *) ID;
- (NSString *) siteName;
- (void) setSiteName:(NSString *)name;
- (NSString *) siteDesc;
- (void) setSiteDesc:(NSString *)desc;
- (NSString *) deviceName;
- (void) setDeviceName:(NSString *)name;
- (NSString *) deviceDesc;
- (void) setDeviceDesc:(NSString *)desc;
- (NSString *) objectName;
- (void) setObjectName:(NSString *)name;
- (NSString *) objectDesc;
- (void) setObjectDesc:(NSString *)desc;
- (BOOL) apply;
- (void) setApply:(BOOL)flag;
- (NSString *) applyInteger;
- (NSString *) effect;
- (BOOL) operationInProgress;
- (void) setOperationInProgress:(BOOL)flag;
- (void) setDelegate:(id)newDelegate;
- (BOOL) exclusive;
- (void) setExclusive:(BOOL)flag;

@property (retain,getter=object) LCEntity *object;
@property (retain,getter=triggerset) id triggerset;
@property (retain,getter=properties,setter=setProperties:) NSMutableDictionary *properties;
@property (retain) NSString *curXMLElement;
@property (retain) NSMutableString *curXMLString;
@property (getter=operationInProgress,setter=setOperationInProgress:) BOOL operationInProgress;
@property (assign,setter=setDelegate:) id delegate;
@property BOOL blindUpdate;
@property (getter=exclusive,setter=setExclusive:) BOOL exclusive;
@property BOOL deleted;
@end
