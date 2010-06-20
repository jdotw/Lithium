//
//  LCCustomerList.h
//  Lithium Console
//
//  Created by James Wilson on 12/09/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"
#import "LCCoreDeployment.h"

@interface LCCustomerList : NSObject 
{
	/* Customer data */
	NSMutableArray *array;
	NSMutableDictionary *dict;
	
	/* Deployments */
	NSMutableArray *staticDeployments;
	NSMutableArray *dynamicDeployments;
	NSMutableDictionary *dynamicDeploymentDict;
	
	/* Op variables */
	BOOL refreshInProgress;
	NSMutableDictionary *element_dict;
	NSString *curXMLElement;
	NSMutableString *curXMLString;
	BOOL testMode;
	unsigned int customersFound;
	BOOL encounteredError;
	
	/* Interface variables */
	NSModalSession msession;
	IBOutlet NSWindow *window;
	IBOutlet NSProgressIndicator *pbar;
	
	/* LCEntity Compatibility */
	BOOL childrenPopulated;
}

+ (LCCustomerList *) list;
- (LCCustomerList *) init;
+ (BOOL) urlconfEnabled;
+ (NSString *) urlconfURL;
+ (BOOL) discoveryEnabled;
+ (BOOL) alertOnErrorEnabled;
+ (LCCustomerList *) masterInit;
+ (void) masterRefresh;
+ (LCCustomerList *) masterList;
+ (NSMutableArray *) masterArray;
+ (NSMutableDictionary *) masterDict;
+ (LCCustomer *) masterCustByName:(NSString *)custname;
- (void) refresh;
- (void) refreshFromURL:(NSString *)urlString;
- (BOOL) testUrl:(NSString *)urlString;
- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict;
- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string;
- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname;
- (void) refreshFinished;

/* Bonjour Browser */
- (void)netServiceBrowserWillSearch:(NSNetServiceBrowser *)browser;
- (void)netServiceBrowserDidStopSearch:(NSNetServiceBrowser *)browser;
- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
			 didNotSearch:(NSDictionary *)errorDict;
- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
		   didFindService:(NSNetService *)aNetService
			   moreComing:(BOOL)moreComing;
- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
		 didRemoveService:(NSNetService *)aNetService
			   moreComing:(BOOL)moreComing;


- (NSMutableArray *) array;
- (void) setArray:(NSArray *)newArray;
- (void) insertObject:(LCCustomer *)cust inArrayAtIndex:(unsigned int)index;
- (void) removeObjectFromArrayAtIndex:(unsigned int)index;
- (NSMutableDictionary *) dict;
- (LCCustomer *) custByName:(NSString *)custname;
- (NSMutableArray *) staticDeployments;
- (void) insertObject:(LCCoreDeployment *)deployment inStaticDeploymentsAtIndex:(unsigned int)index;
- (void) removeObjectFromStaticDeploymentsAtIndex:(unsigned int)index;
- (NSMutableArray *) dynamicDeployments;
- (void) insertObject:(LCCoreDeployment *)deployment inDynamicDeploymentsAtIndex:(unsigned int)index;
- (void) removeObjectFromDynamicDeploymentsAtIndex:(unsigned int)index;
- (BOOL) refreshInProgress;
- (void) setRefreshInProgress:(BOOL)value;

#pragma mark Browser Outlineview Compatibility
- (NSArray *) children;
- (NSString *) name;
- (NSString *) desc;
- (BOOL) childrenPopulated;
- (void) setChildrenPopulated:(BOOL)flag;

@property (retain,getter=staticDeployments) NSMutableArray *staticDeployments;
@property (retain,getter=dynamicDeployments) NSMutableArray *dynamicDeployments;
@property (retain) NSMutableDictionary *dynamicDeploymentDict;
@property (getter=refreshInProgress,setter=setRefreshInProgress:) BOOL refreshInProgress;
@property (retain) NSMutableDictionary *element_dict;
@property (retain) NSString *curXMLElement;
@property (retain) NSMutableString *curXMLString;
@property BOOL testMode;
@property unsigned int customersFound;
@property BOOL encounteredError;
@property NSModalSession msession;
@property (retain) NSWindow *window;
@property (retain) NSProgressIndicator *pbar;
@property (getter=childrenPopulated,setter=setChildrenPopulated:) BOOL childrenPopulated;
@end

