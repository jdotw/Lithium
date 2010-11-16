//
//  LTEntityDescriptor.h
//  Lithium
//
//  Created by James Wilson on 28/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTEntity.h"

@interface LTEntityDescriptor : NSObject 
{
	int type;
	NSString *name;
	NSString *desc;
	int opState;
	int adminState;

	NSString *custName;
	NSString *custDesc;
	int custOpState;
	int custAdminState;
	NSString *siteName;
	NSString *siteDesc;
	NSString *siteSuburb;
	int siteOpState;
	int siteAdminState;
	NSString *devName;
	NSString *devDesc;
	int devOpState;
	int devAdminState;
	NSString *cntName;
	NSString *cntDesc;
	int cntOpState;
	int cntAdminState;
	NSString *objName;
	NSString *objDesc;
	int objOpState;
	int objAdminState;
	NSString *metName;
	NSString *metDesc;
	int metOpState;
	int metAdminState;
	NSString *trgName;
	NSString *trgDesc;
	int trgOpState;
	int trgAdminState;
	
	NSString *resourceAddress;
}

#pragma mark "Constructors"
- (id) initWithEntity:(LTEntity *)entity;
- (id) initWithEntityAddress:(NSString *)entityAddress;
- (id) copy;

#pragma mark "XML Methods"
- (NSString *) xmlNodeString;
+ (LTEntityDescriptor *) entityDescriptorFromXml:(LCXMLNode *)xmlNode;

#pragma mark "Properties"
@property (readonly) NSString *entityAddress;

@property (assign) int type;
@property (copy) NSString *name;
@property (copy) NSString *desc;
@property (assign) int opState;
@property (assign) int adminState;

@property (copy) NSString *custName;
@property (copy) NSString *custDesc;
@property (assign) int custOpState;
@property (assign) int custAdminState;
@property (copy) NSString *siteName;
@property (copy) NSString *siteDesc;
@property (copy) NSString *siteSuburb;
@property (assign) int siteOpState;
@property (assign) int siteAdminState;
@property (copy) NSString *devName;
@property (copy) NSString *devDesc;
@property (assign) int devOpState;
@property (assign) int devAdminState;
@property (copy) NSString *cntName;
@property (copy) NSString *cntDesc;
@property (assign) int cntOpState;
@property (assign) int cntAdminState;
@property (copy) NSString *objName;
@property (copy) NSString *objDesc;
@property (assign) int objOpState;
@property (assign) int objAdminState;
@property (copy) NSString *metName;
@property (copy) NSString *metDesc;
@property (assign) int metOpState;
@property (assign) int metAdminState;
@property (copy) NSString *trgName;
@property (copy) NSString *trgDesc;
@property (assign) int trgOpState;
@property (assign) int trgAdminState;

@property (copy) NSString *resourceAddress;


@end
