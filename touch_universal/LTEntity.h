//
//  LTEntity.h
//  Lithium
//
//  Created by James Wilson on 26/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTMetricValue.h"
#import "LCXMLNode.h"
#import "LTAPIRequest.h"

#define ENT_CUSTOMER 1
#define ENT_SITE 2
#define ENT_DEVICE 3
#define ENT_CONTAINER 4
#define ENT_OBJECT 5
#define ENT_METRIC 6
#define ENT_TRIGGER 7

@class LTCustomer, LTCoreDeployment, LTEntityDescriptor;

@interface LTEntity : LTAPIRequest
{
	int type;
	NSString *name;
	NSString *desc;
	int adminState;
	int opState;
	NSString *currentValue;
	NSMutableArray *values;
	NSString *maxValue;
	NSTimeInterval refreshInterval;
	int recordMethod;
	BOOL recordEnabled;
	NSString *units;
	int kbase;
	unsigned long allocUnit;
	BOOL hasTriggers;
	BOOL showInSummary;
	unsigned long version;				// Actual version
	unsigned long syncVersion;			// Absolute latest version number
	NSString *uuidString;

	NSString *resourceAddress;
	NSString *entityAddress;
	LTEntityDescriptor *entityDescriptor;
	
	NSString *ipAddress;
	NSString *customerName;
	NSString *username;
	NSString *password;
	
	NSMutableArray *children;
	NSMutableDictionary *childDict;
	LTEntity *parent;
	LTCoreDeployment *coreDeployment;
	
	BOOL hasBeenRefreshed;
	NSDate *lastRefresh;
	NSString *xmlStatus;
	
	int indentLevel;
}

- (void) refresh;
- (void) setXmlValue:(id)value forKey:(NSString *)key;
- (void) updateEntityUsingXMLNode:(LCXMLNode *)node;
- (void) xmlParserDidFinish:(LCXMLNode *)rootNode;
- (NSString *) urlPrefix;
- (NSURL *) urlForXml:(NSString *)xmlName timestamp:(int)urlRefSec;
- (LTEntity *) parentOfType:(int)type;
- (LTEntity *) locateChildUsingEntityDescriptor:(LTEntityDescriptor *)entDesc;
- (LTEntity *) locateChildType:(int)childType usingEntityDescriptor:(LTEntityDescriptor *)entDesc;
	 
@property (assign) int type;
@property (copy) NSString *name;
@property (copy) NSString *desc;
@property (assign) int adminState;
@property (assign) int opState;
@property (copy) NSString *currentValue;
@property (retain) NSMutableArray *values;
@property (copy) NSString *maxValue;
@property (assign) NSTimeInterval refreshInterval;
@property (assign) int recordMethod;
@property (assign) BOOL recordEnabled;
@property (copy) NSString *units;
@property (assign) int kbase;
@property (assign) unsigned long allocUnit;
@property (assign) BOOL hasTriggers;
@property (assign) BOOL showInSummary;
@property unsigned long version;
@property unsigned long syncVersion;
@property (copy) NSString *uuidString;

@property (copy) NSString *resourceAddress;
@property (readonly) NSString *deviceEntityAddress;
@property (copy) NSString *entityAddress;
@property (retain) LTEntityDescriptor *entityDescriptor;
@property (retain) NSString *ipAddress;
@property (retain) NSString *customerName;
@property (retain) NSString *username;
@property (retain) NSString *password;
@property (readonly) NSMutableArray *children;
@property (readonly) NSMutableDictionary *childDict;
@property (assign) LTEntity *parent;
@property (readonly) LTEntity *device;
@property (assign) LTCoreDeployment *coreDeployment;
@property (assign) BOOL hasBeenRefreshed;
@property (copy) NSDate *lastRefresh;
@property (readonly) NSMutableDictionary *xmlTranslation;
@property (copy) NSString *xmlStatus;

@property (assign) int indentLevel;

@property (readonly) NSString *longDisplayString;
@property (readonly) NSString *longLocationString;

@property (readonly) UIImage *icon;

@property (readonly) NSArray *graphableMetrics;

@property (readonly) LTEntity *container;
@property (readonly) LTEntity *object;
@property (readonly) LTEntity *metric;


@end
