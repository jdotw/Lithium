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
#import "TBXML.h"

#define ENT_CUSTOMER 1
#define ENT_SITE 2
#define ENT_DEVICE 3
#define ENT_CONTAINER 4
#define ENT_OBJECT 5
#define ENT_METRIC 6
#define ENT_TRIGGER 7

#define kLTEntityStateChanged @"LTEntityStateChanged"           // Fires on adminSate or opState change
#define kLTEntityChildrenChanged @"LTEntityChildrenChanged"     // Fires on children array add/remove
#define kLTEntityValueChanged @"LTEntityValueChanged"           // Fires when the _existing_ currentValue changes (not on initial set)

@class LTCustomer, LTCoreDeployment, LTEntityDescriptor, LTGroup;

@interface LTEntity : LTAPIRequest <NSCopying>
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
	
	/* Device Properties */
	NSString *deviceIpAddress;
	NSString *lomIpAddress;
	int snmpVersion;
	NSString *snmpCommunity;
	int snmpAuthMethod;
	NSString *snmpAuthPassword;
	int snmpPrivacyMethod;
	NSString *snmpPrivacyPassword;
	NSString *deviceUsername;
	NSString *devicePassword;
	NSString *lomUsername;
	NSString *lomPassword;
	NSString *vendorModule;
	int deviceProtocol;
	BOOL useICMP;
	BOOL useProcessList;
	BOOL useLOM;

	NSString *resourceAddress;
	NSString *entityAddress;
	LTEntityDescriptor *entityDescriptor;
	
	NSString *ipAddress;
	NSString *customerName;
	NSString *username;
	NSString *password;
	
	NSMutableArray *children;
	NSMutableDictionary *childDict;
	LTEntity *parent;                       // Assigneg property
	LTCoreDeployment *coreDeployment;       // Assigned property
	
	BOOL hasBeenRefreshed;
	NSDate *lastRefresh;
	NSString *xmlStatus;
    BOOL isNew;             // Only true when entity is new and not yet added to parent 
    BOOL lastRefreshFailed;     // Set by local didFail method
	
	int indentLevel;
    LTGroup *groupParent;                   // Group parent
}

- (void) refresh;
- (void) forceRefresh;
- (void) postXmlToResource:(NSString *)resourceAddress 
			 entityAddress:(NSString *)entityAddress 
				   xmlName:(NSString *)xmlName 
					   xml:(NSString *)xml;
- (void) setXmlValue:(id)value forKey:(NSString *)key;
- (void) updateEntityUsingXML:(TBXML *)xml;
- (void) updateEntityUsingXMLNode:(TBXMLElement *)node;
- (NSString *) urlPrefix;
- (NSURL *) urlForXml:(NSString *)xmlName timestamp:(int)urlRefSec;
- (LTEntity *) parentOfType:(int)type;
- (LTEntity *) locateChildUsingEntityDescriptor:(LTEntityDescriptor *)entDesc;
- (LTEntity *) locateChildType:(int)childType usingEntityDescriptor:(LTEntityDescriptor *)entDesc;
	 
@property (nonatomic,assign) int type;
@property (nonatomic,copy) NSString *name;
@property (nonatomic,copy) NSString *desc;
@property (nonatomic,assign) int adminState;
@property (nonatomic,assign) int opState;
@property (nonatomic,copy) NSString *currentValue;
@property (nonatomic,retain) NSMutableArray *values;
@property (nonatomic,copy) NSString *maxValue;
@property (nonatomic,assign) NSTimeInterval refreshInterval;
@property (nonatomic,assign) int recordMethod;
@property (nonatomic,assign) BOOL recordEnabled;
@property (nonatomic,copy) NSString *units;
@property (nonatomic,assign) int kbase;
@property (nonatomic,assign) unsigned long allocUnit;
@property (nonatomic,assign) BOOL hasTriggers;
@property (nonatomic,assign) BOOL showInSummary;
@property (nonatomic,assign) unsigned long version;
@property (nonatomic,assign) unsigned long syncVersion;
@property (nonatomic,copy) NSString *uuidString;

/* Device Properties */
@property (nonatomic,copy) NSString *deviceIpAddress;
@property (nonatomic,copy) NSString *lomIpAddress;
@property (nonatomic,assign) int snmpVersion;
@property (nonatomic,copy) NSString *snmpCommunity;
@property (nonatomic,assign) int snmpAuthMethod;
@property (nonatomic,copy) NSString *snmpAuthPassword;
@property (nonatomic,assign) int snmpPrivacyMethod;
@property (nonatomic,copy) NSString *snmpPrivacyPassword;
@property (nonatomic,copy) NSString *deviceUsername;
@property (nonatomic,copy) NSString *devicePassword;
@property (nonatomic,copy) NSString *lomUsername;
@property (nonatomic,copy) NSString *lomPassword;
@property (nonatomic,copy) NSString *vendorModule;
@property (nonatomic,assign) int deviceProtocol;
@property (nonatomic,assign) BOOL useICMP;
@property (nonatomic,assign) BOOL useProcessList;
@property (nonatomic,assign) BOOL useLOM;

@property (nonatomic,readonly) NSString *deviceResourceAddress;
@property (nonatomic,copy) NSString *resourceAddress;
@property (nonatomic,readonly) NSString *deviceEntityAddress;
@property (nonatomic,copy) NSString *entityAddress;
@property (nonatomic,retain) LTEntityDescriptor *entityDescriptor;
@property (nonatomic,retain) NSString *ipAddress;
@property (nonatomic,retain) NSString *customerName;
@property (nonatomic,retain) NSString *username;
@property (nonatomic,retain) NSString *password;
@property (nonatomic,readonly) NSMutableArray *children;
@property (nonatomic,readonly) NSMutableDictionary *childDict;
@property (nonatomic,assign) LTEntity *parent;
@property (nonatomic,readonly) LTEntity *device;
@property (nonatomic,assign) LTCoreDeployment *coreDeployment;
@property (nonatomic,assign) BOOL hasBeenRefreshed;
@property (nonatomic,copy) NSDate *lastRefresh;
@property (nonatomic,readonly) NSMutableDictionary *xmlTranslation;
@property (nonatomic,copy) NSString *xmlStatus;
@property (nonatomic,assign) BOOL lastRefreshFailed;

@property (nonatomic,assign) int indentLevel;

@property (nonatomic,readonly) NSString *longDisplayString;
@property (nonatomic,readonly) NSString *longLocationString;

@property (nonatomic,readonly) UIImage *icon;

@property (nonatomic,readonly) NSArray *graphableMetrics;

@property (nonatomic,readonly) LTEntity *site;
@property (nonatomic,readonly) LTEntity *container;
@property (nonatomic,readonly) LTEntity *object;
@property (nonatomic,readonly) LTEntity *metric;

@property (readonly) BOOL isPercentage;
@property (nonatomic,assign) BOOL isNew;

@property (nonatomic,readonly) UIColor *opStateTintColor;
@property (nonatomic,readonly) LTEntity *valueMetric;

@property (nonatomic,retain) LTGroup *groupParent;

@end
