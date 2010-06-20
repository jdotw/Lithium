//
//  LCTriggersetValRule.h
//  Lithium Console
//
//  Created by James Wilson on 6/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXMLRequest.h"

@interface LCTriggersetValRule : NSObject 
{
	/* Variables */
	LCEntity *obj;
	id tset;
	id trg;
	NSMutableDictionary *properties;
	BOOL exclusive;

	/* Refresh */
	BOOL blindUpdate;
	NSString *curXMLElement;
	NSMutableString *curXMLString;
	BOOL operationInProgress;
	id delegate;
}

#pragma mark "Constructors"
+ (LCTriggersetValRule *) ruleWithObject:(LCEntity *)initObject triggerset:(id)initTriggerset trigger:(id)initTrigger;
+ (LCTriggersetValRule *) ruleWithObject:(LCEntity *)initObject 
							  triggerset:(id)initTriggerset
								 trigger:(id)initTrigger
							criteriaSite:(LCEntity *)cSite
						  criteriaDevice:(LCEntity *)cDevice
						  criteriaObject:(LCEntity *)cObject
						 criteriaTrgType:(int)cTrgType
						  criteriaXValue:(NSString *)cXValue
						  criteriaYValue:(NSString *)cYValue
						criteriaDuration:(NSString *)cDuration
					  criteriaAdminstate:(int)cAdminstate;
- (LCTriggersetValRule *) initWithObject:(LCEntity *)initObject triggerset:(id)initTriggerset trigger:(id)initTrigger;
- (LCTriggersetValRule *) initWithObject:(LCEntity *)initObject 
							  triggerset:(id)initTriggerset
								 trigger:(id)initTrigger
							criteriaSite:(LCEntity *)cSite
						  criteriaDevice:(LCEntity *)cDevice
						  criteriaObject:(LCEntity *)cObject
						 criteriaTrgType:(int)cTrgType
						  criteriaXValue:(NSString *)cXValue
						  criteriaYValue:(NSString *)cYValue
						criteriaDuration:(NSString *)cDuration
					  criteriaAdminstate:(int)cAdminstate;
- (void) dealloc;

#pragma mark "Update"
- (void) update;

#pragma mark "Delete"
- (void) delete;

#pragma mark "XML Processing"
- (void) XMLRequestPreParse:(id)sender;
- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict ;
- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string ;
- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname;
- (void) XMLRequestFinished:(LCXMLRequest *)sender;

#pragma mark "Accessors"
- (NSMutableDictionary *) properties;
- (void) setProperties:(NSMutableDictionary *)newProperties;
- (LCEntity *) object;
- (id) triggerset;
- (id) trigger;
- (NSString *) ID;
- (NSString *) siteName;
- (void) setSiteName:(NSString *)value;
- (NSString *) siteDesc;
- (void) setSiteDesc:(NSString *)value;
- (NSString *) deviceName;
- (void) setDeviceName:(NSString *)value;
- (NSString *) deviceDesc;
- (void) setDeviceDesc:(NSString *)value;
- (NSString *) objectName;
- (void) setObjectName:(NSString *)value;
- (NSString *) objectDesc;
- (void) setObjectDesc:(NSString *)value;
- (NSString *) triggerName;
- (void) setTriggerName:(NSString *)value;
- (NSString *) triggerDesc;
- (void) setTriggerDesc:(NSString *)value;
- (NSString *) units;
- (NSString *) unitsDisplayString;
- (NSString *) xValue;
- (NSString *) xValueDisplayString;
- (void) setXValue:(NSString *)value;
- (NSString *) yValue;
- (NSString *) yValueDisplayString;
- (void) setYValue:(NSString *)value;
- (NSString *) triggerType;
- (NSString *) triggerTypeInteger;
- (void) setTriggerType:(int)type;
- (NSString *) effect;
- (NSString *) duration;
- (void) setDuration:(NSString *)value;
- (NSString *) adminState;
- (void) setAdminState:(int)value;
- (NSString *) adminStateInteger;
- (BOOL) operationInProgress;
- (void) setOperationInProgress:(BOOL)flag;
- (void) setDelegate:(id)newDelegate;
- (BOOL) exclusive;
- (void) setExclusive:(BOOL)flag;

@property (retain,getter=object) LCEntity *obj;
@property (retain,getter=triggerset) id tset;
@property (retain,getter=trigger) id trg;
@property (retain,getter=properties,setter=setProperties:) NSMutableDictionary *properties;
@property (getter=exclusive,setter=setExclusive:) BOOL exclusive;
@property BOOL blindUpdate;
@property (retain) NSString *curXMLElement;
@property (retain) NSMutableString *curXMLString;
@property (getter=operationInProgress,setter=setOperationInProgress:) BOOL operationInProgress;
@property (assign,setter=setDelegate:) id delegate;
@end
