//
//  LCCoreInfoController.h
//  Lithium Console
//
//  Created by James Wilson on 25/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCoreInfoResource.h"
#import "LCLithiumSetupWindowController.h"
#import "LCXMLRequest.h"

@interface LCCoreInfoController : NSObject 
{
	/* Related objects */
	IBOutlet LCLithiumSetupWindowController *setupController;
	
	/* UI Elements */
	IBOutlet NSWindow *restartSheet;
	
	/* Local Properties */
	NSMutableDictionary *properties;
	
	/* Resources */
	NSMutableArray *resources;
	NSMutableDictionary *resourceDictionary;
	
	/* Refresh timer */
	NSTimer *refreshTimer;
	
	/* XML */
	LCXMLRequest *refreshXMLRequest;
	NSMutableDictionary *curXMLDictionary;
	NSString *curXMLElement;
	NSMutableString *curXMLString;
	NSMutableDictionary *xmlResDictionary;
	BOOL refreshInProgress;		
	
	/* Current Version Check */
	NSURLConnection *urlConn;
	NSMutableData *receivedData;
	BOOL refreshLocal;
	
	/* Uptime */
	NSString *startTime;
	NSString *uptime;
	NSTimer *uptimeUpdateTimer;
}

#pragma mark "Constructors"
- (void) awakeFromNib;

#pragma mark "Refresh methods"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;
- (void) XMLRequestPreParse:(id)sender;
- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict ;
- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string;
- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname;
- (void) XMLRequestFinished:(LCXMLRequest *)sender;

#pragma mark "UI Actions"
- (IBAction) downloadUpdateClicked:(id)sender;
- (IBAction) restartProcessesClicked:(id)sender;
- (IBAction) restartSheetRestartClicked:(id)sender;
- (IBAction) restartSheetCancelClicked:(id)sender;

#pragma mark "Current Version Check"
- (void) checkCurrentVersion;

#pragma mark "Accessors"
- (NSMutableDictionary *) properties;
- (void) setProperties:(NSMutableDictionary *)dict;
- (NSMutableArray *) resources;
- (void) setResources:(NSMutableArray *)array;
- (void) insertObject:(LCCoreInfoResource *)resource inResourcesAtIndex:(unsigned int)index;
- (void) removeObjectFromResourcesAtIndex:(unsigned int)index;
- (NSMutableDictionary *) resourceDictionary;
- (void) setResourceDictionary:(NSMutableDictionary *)dict;
- (BOOL) refreshInProgress;
- (void) setRefreshInProgress:(BOOL)flag;


@property (retain) LCLithiumSetupWindowController *setupController;
@property (retain) NSWindow *restartSheet;
@property (retain,getter=properties,setter=setProperties:) NSMutableDictionary *properties;
@property (retain,getter=resources,setter=setResources:) NSMutableArray *resources;
@property (retain,getter=resourceDictionary,setter=setResourceDictionary:) NSMutableDictionary *resourceDictionary;
@property (retain) NSTimer *refreshTimer;
@property (retain) LCXMLRequest *refreshXMLRequest;
@property (retain) NSMutableDictionary *curXMLDictionary;
@property (retain) NSString *curXMLElement;
@property (retain) NSMutableString *curXMLString;
@property (retain) NSMutableDictionary *xmlResDictionary;
@property (getter=refreshInProgress,setter=setRefreshInProgress:) BOOL refreshInProgress;
@property (retain) NSURLConnection *urlConn;
@property (retain) NSMutableData *receivedData;
@property BOOL refreshLocal;
@property (retain,getter=startTime,setter=setStartTime:) NSString *startTime;
@property (retain,getter=uptime,setter=setUptime:) NSString *uptime;
@property (retain) NSTimer *uptimeUpdateTimer;
@end
