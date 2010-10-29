//
//  LCEntity.h
//  Lithium Console
//
//  Created by James Wilson on 14/06/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCResourceAddress.h"
#import "LCEntityAddress.h"
#import "LCXMLObject.h"
#import "LCXMLNode.h"

@interface LCEntity : LCXMLObject 
{
	/* Hierarchy */
	id parent;
	NSMutableArray *children;
	NSMutableDictionary *childrenDictionary;
	LCResourceAddress *resourceAddress;
	BOOL isOrphan;
	
	/* Entity Variables */
	int type;
	NSString *name;
	NSString *desc;
	NSString *resourceAddressString;
	int opState;
	int adminState;
	int listPriority;
	int entityIndex;
	unsigned long version;				// Actual version
	unsigned long syncVersion;			// Absolute latest version number
	NSString *uuidString;
	
	/* Dynamic Entity Variables */
	NSString *displayString;
	NSString *longDisplayString;
	NSString *displayValue;
	
	/* Refresh variables */
	BOOL childrenPopulated;
	BOOL refreshInProgress;
	BOOL restartInProgress;
	BOOL initialRefreshPerformed;
	id refreshXMLRequest;
	id restartXMLRequest;
	NSMutableString *curXMLString;
	LCEntity *curEntity;
	NSDate *localRefreshVersionTag;			/* Date of the refresh in progress */
	NSDate *refreshVersionTag;				/* Date the entity was refreshed */
	NSDate *childRefreshVersionTag;			/* Date the children were last refreshed */
	BOOL refreshSuccessful;
	NSDate *lastXmlRefresh;
	NSDate *parseStartDate;
	unsigned long receivedVersion;
	NSMutableDictionary *valueDictionary;
	
	/* Auto refresh */
	NSTimer *autoRefreshTimer;
	BOOL autoRefresh;
	
	/* Entity View */
	BOOL entityViewNIBLoaded;
	IBOutlet NSView *entityView;
	IBOutlet NSObjectController *entityViewObjectController;
	
	/* Properties Dictionary */
	NSMutableDictionary *properties;
	
	/* Incidents */
	NSMutableArray *incidents;
	
	/* State */
	BOOL versionWarningShown;
	BOOL presenceConfirmed;			/* Found in refresh vs locateEntity: */
	BOOL assistShown;
	BOOL isLicensed;
	BOOL resourceStarted;
	int browserViewType;
}

#pragma mark "Initialisation"
+ (LCEntity *) entityWithType:(int)initType name:(NSString *)initName desc:(NSString *)initDesc resourceAddressString:(NSString *)initResAddr;
- (LCEntity *) initWithType:(int)initType name:(NSString *)initName desc:(NSString *)initDesc resourceAddressString:(NSString *)initResAddr;
- (id) init;
- (void) dealloc;

#pragma mark "Refresh Methods"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;
- (void) updateEntityUsingXMLNode:(LCXMLNode *)node;

#pragma mark "Process Restart"
- (void) restartMonitoringProcess;

#pragma mark "Auto-refresh Methods"
- (BOOL) autoRefresh;
- (void) setAutoRefresh:(BOOL)flag;

#pragma mark "Browser-related Methods"
- (BOOL) isBrowserLeaf;

#pragma mark "Status Color"
- (NSColor *) opStateColor;
- (NSColor *) opStateColorWithAlpha:(float)alpha;

#pragma mark "Hierarchy Methods"
- (id) entityInHierarchyOfType:(int)type;
@property (readonly) id customer;
@property (readonly) id site;
@property (readonly) id device;
@property (readonly) id container;
@property (readonly) id object;
@property (readonly) id metric;
@property (readonly) id trigger;
- (BOOL) isDescendantOf:(LCEntity *)entity;

#pragma mark "Web URL"
- (NSString *) webURLString;

#pragma mark "Parent/Child Methods"
- (NSMutableArray *) children;
- (void) insertObject:(LCEntity *)entity inChildrenAtIndex:(unsigned int)index;
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index;
- (void) addChild:(LCEntity *)child;
- (void) removeChild:(LCEntity *)child;
- (NSMutableDictionary *) childrenDictionary;
- (LCEntity *) childNamed:(NSString *)childName;
- (Class) childClass;
- (void) addChildIfNew:(LCEntity *)newEntity;

#pragma mark "Incident Methods"
- (void) insertObject:(id)object inIncidentsAtIndex:(unsigned int)index;
- (void) removeObjectFromIncidentsAtIndex:(unsigned int)index;

#pragma mark "Connect-using Methods"
- (void) connectUsingSSH;
- (void) connectUsingTelnet;
- (void) connectUsingWeb;
- (void) connectUsingARD;

#pragma mark "Graphable Metrics"
- (NSMutableArray *) graphableMetrics;

#pragma mark "Sanctioned Properties"

@property (nonatomic,retain) id parent;
@property (nonatomic,retain) NSMutableDictionary *childrenDictionary;
@property (nonatomic, assign) BOOL isOrphan;
@property int type;
@property (nonatomic,copy) NSString *name;
@property (nonatomic,copy) NSString *desc;
@property (nonatomic,copy) NSString *resourceAddressString;
@property int opState;
@property int adminState;
@property int listPriority;
@property int entityIndex;
@property unsigned long version;
@property unsigned long syncVersion;
@property (nonatomic,copy) NSString *uuidString;
@property BOOL isLicensed;
@property BOOL resourceStarted;
@property BOOL presenceConfirmed;
@property BOOL childrenPopulated;
@property BOOL refreshInProgress;
@property BOOL restartInProgress;
@property BOOL initialRefreshPerformed;
@property BOOL refreshSuccessful;
@property (nonatomic,copy) NSDate *refreshVersionTag;
@property (nonatomic,copy) NSDate *childRefreshVersionTag;
@property (nonatomic,copy) NSDate *localRefreshVersionTag;
@property (nonatomic,copy) NSDate *lastXmlRefresh;
@property (nonatomic,retain) NSMutableArray *incidents;
@property (nonatomic,copy) NSString *displayString;
@property (nonatomic,copy) NSString *longDisplayString;
@property (nonatomic, assign) int browserViewType;
@property (nonatomic,copy) NSString *displayValue;
@property (readonly) NSString *uniqueIdentifier;
- (void) recursivelySetOpStateTo:(int)value;
@property (nonatomic,readonly) BOOL actionFilterLeaf;


#pragma mark "Unsure Properties"

@property (nonatomic,retain) NSMutableDictionary *properties;
@property BOOL entityViewNIBLoaded;
@property (nonatomic,retain) NSView *entityView;
@property (nonatomic,retain) NSObjectController *entityViewObjectController;
@property BOOL versionWarningShown;
@property BOOL assistShown;

#pragma mark "Accessors"

- (LCResourceAddress *) resourceAddress;
- (NSString *) typeString;
- (LCEntityAddress *) entityAddress;
- (id) entityDescriptor;
- (id) entityViewController;
- (id) viewController;
- (BOOL) isDeviceLeaf;
- (BOOL) isBrowserTreeLeaf;
- (BOOL) isContainerTreeLeaf;
- (BOOL) isObjectTreeLeaf;
- (NSString *) longDisplayString;

#pragma mark "Legacy Accessors FIX"

- (NSString *) typeInteger;
- (NSString *) adminstateInteger;
- (NSString *) opstateInteger;
- (NSNumber *) indexNumber;

#pragma mark "Ugly Accessors"
//- (BOOL) recentlyRestarted;

@end
