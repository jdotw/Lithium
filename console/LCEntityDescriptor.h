//
//  LCEntityDescriptor.h
//  Lithium Console
//
//  Created by James Wilson on 28/06/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCResourceAddress.h"
#import "LCEntity.h"
#import "LCXMLNode.h"

@interface LCEntityDescriptor : NSObject 
{
	NSMutableDictionary *properties;
}

#pragma mark "Initialisation"
+ (LCEntityDescriptor *) descriptorWithProperties:(id)initProperties;
+ (LCEntityDescriptor *) descriptorForEntity:(LCEntity *)entity;
+ (LCEntityDescriptor *) descriptorWithXmlNode:(LCXMLNode *)xmlNode;
- (id) initWithXmlNode:(LCXMLNode *)xmlNode;
- (id) initWithProperties:(id)initProperties;
- (id) initForEntity:(LCEntity *)entity;
- (id) init;
- (void) dealloc;

#pragma mark "XML Methods"
- (NSXMLNode *) xmlNode;

#pragma mark "Address string"
- (NSString *) addressString;

#pragma mark "Entity Location Methods"
- (id) locateEntity:(BOOL)createMissing;

#pragma mark "Entity Update Methods"
- (void) updateLocalFromDescriptor;

#pragma mark "Accessor Methods"
@property (readonly) NSMutableDictionary *properties;
- (void) updateProperties:(NSDictionary *)dictionary;

- (NSString *) type;
- (NSString *) type_num;
- (NSString *) name;
- (NSString *) desc;
- (NSString *) adminstate;
- (NSString *) adminstate_num;
- (NSString *) opstate;
- (NSString *) opstate_num;

- (NSString *) cust_name;
- (NSString *) cust_desc;
- (NSString *) cust_adminstate;
- (NSString *) cust_adminstate_num;
- (NSString *) cust_opstate;
- (NSString *) cust_opstate_num;

- (NSString *) site_name;
- (NSString *) site_desc;
- (NSString *) site_suburb;
- (NSString *) site_adminstate;
- (NSString *) site_adminstate_num;
- (NSString *) site_opstate;
- (NSString *) site_opstate_num;

- (NSString *) dev_name;
- (NSString *) dev_desc;
- (NSString *) dev_adminstate;
- (NSString *) dev_adminstate_num;
- (NSString *) dev_opstate;
- (NSString *) dev_opstate_num;

- (NSString *) cnt_name;
- (NSString *) cnt_desc;
- (NSString *) cnt_adminstate;
- (NSString *) cnt_adminstate_num;
- (NSString *) cnt_opstate;
- (NSString *) cnt_opstate_num;

- (NSString *) obj_name;
- (NSString *) obj_desc;
- (NSString *) obj_adminstate;
- (NSString *) obj_adminstate_num;
- (NSString *) obj_opstate;
- (NSString *) obj_opstate_num;

- (NSString *) met_name;
- (NSString *) met_desc;
- (NSString *) met_adminstate;
- (NSString *) met_adminstate_num;
- (NSString *) met_opstate;
- (NSString *) met_opstate_num;

- (NSString *) trg_name;
- (NSString *) trg_desc;
- (NSString *) trg_adminstate;
- (NSString *) trg_adminstate_num;
- (NSString *) trg_opstate;
- (NSString *) trg_opstate_num;

- (NSString *) longDisplayString;


@end
