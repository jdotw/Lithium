//
//  LCServiceScriptManagerController.h
//  Lithium Console
//
//  Created by James Wilson on 7/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCXMLRequest.h"
#import "LCServiceScriptList.h"
#import "LCServiceScript.h"
#import "LCScriptDownloadRequest.h"
#import "LCScriptUploadRequest.h"
#import "LCLithiumSetupWindowController.h"

@interface LCServiceScriptManagerController : NSObject 
{
	/* Related objects */
	IBOutlet LCLithiumSetupWindowController *setupController;
	
	/* UI Elements */
	IBOutlet NSWindow *uploadSheet;
	IBOutlet NSWindow *downloadSheet;
	IBOutlet NSArrayController *scriptArrayController;
	IBOutlet NSArrayController *communityScriptArrayController;
	IBOutlet NSWindow *repairSheet;
	IBOutlet NSWindow *communityDownloadSheet;
	IBOutlet NSTableView *localScriptsTableView;
	IBOutlet NSTableView *communityScriptsTableView;
	
	/* Script uploading/downloading */
	LCScriptUploadRequest *uploadRequest;
	LCScriptDownloadRequest *downloadRequest;
	NSURLConnection *urlConn;
	NSMutableData *receivedData;
	LCServiceScript *scriptBeingInstalled;
	BOOL scriptInstallInProgress;
	NSString *scriptInstallProgressString;
	
	/* Script List */
	LCServiceScriptList *scriptList;
	NSPredicate *localScriptsFilter;
	
	/* Delete */
	LCXMLRequest *deleteRequest;
	
	/* Repair */
	LCXMLRequest *repairRequest;
	BOOL repairInProgress;
	NSString *repairStatusString;
	NSString *repairScriptOutput;
	NSImage *repairStatusIcon;
	LCServiceScript *scriptBeingRepaired;
	
	/* XML Parsing */
	NSMutableString *xmlString;
	NSString *xmlElement;
}

#pragma mark "Constructors"
- (void) dealloc;

#pragma mark "UI Actions"
- (IBAction) uploadScriptClicked:(id)sender;
- (IBAction) closeUploadSheetClicked:(id)sender;
- (IBAction) cancelUploadClicked:(id)sender;
- (IBAction) deleteScriptClicked:(id)sender;
- (IBAction) repairScriptClicked:(id)sender;
- (IBAction) repairSheetCloseClicked:(id)sender;
- (IBAction) refreshListClicked:(id)sender;
- (IBAction) downloadScriptClicked:(id)sender;
- (IBAction) installCommunityScriptClicked:(NSArray *)arg;
- (IBAction) cancelInstallCommunityScriptClicked:(id)sender;

#pragma mark "Script Selection"
- (LCServiceScript *) selectedScript;

#pragma mark "Script Selection"
- (LCServiceScript *) selectedCommunityScript;

#pragma mark "Accessors"
- (LCServiceScriptList *) scriptList;
- (void) setScriptList:(LCServiceScriptList *)list;
- (LCScriptUploadRequest *) uploadRequest;
- (void) setUploadRequest:(LCScriptUploadRequest *)request;
- (LCScriptDownloadRequest *) downloadRequest;
- (void) setDownloadRequest:(LCScriptDownloadRequest *)request;
- (BOOL) repairInProgress;
- (void) setRepairInProgress:(BOOL)flag;
- (NSString *) repairStatusString;
- (void) setRepairStatusString:(NSString *)string;
- (NSString *) repairScriptOutput;
- (void) setRepairScriptOutput:(NSString *)string;
- (NSImage *) repairStatusIcon;
- (void) setRepairStatusIcon:(NSImage *)icon;
- (LCServiceScript *) scriptBeingRepaired;
- (void) setScriptBeingRepaired:(LCServiceScript *)script;
- (BOOL) scriptInstallInProgress;
- (void) setScriptInstallInProgress:(BOOL)flag;
- (NSString *) scriptInstallProgressString;
- (void) setScriptInstallProgressString:(NSString *)string;

@property (retain) LCLithiumSetupWindowController *setupController;
@property (retain) NSWindow *uploadSheet;
@property (retain) NSWindow *downloadSheet;
@property (retain) NSArrayController *scriptArrayController;
@property (retain) NSArrayController *communityScriptArrayController;
@property (retain) NSWindow *repairSheet;
@property (retain) NSWindow *communityDownloadSheet;
@property (retain) NSTableView *localScriptsTableView;
@property (retain) NSTableView *communityScriptsTableView;
@property (retain,getter=uploadRequest,setter=setUploadRequest:) LCScriptUploadRequest *uploadRequest;
@property (retain,getter=downloadRequest,setter=setDownloadRequest:) LCScriptDownloadRequest *downloadRequest;
@property (retain) NSURLConnection *urlConn;
@property (retain) NSMutableData *receivedData;
@property (retain) LCServiceScript *scriptBeingInstalled;
@property (getter=scriptInstallInProgress,setter=setScriptInstallInProgress:) BOOL scriptInstallInProgress;
@property (retain,getter=scriptInstallProgressString,setter=setScriptInstallProgressString:) NSString *scriptInstallProgressString;
@property (retain,getter=scriptList,setter=setScriptList:) LCServiceScriptList *scriptList;
@property (retain) NSPredicate *localScriptsFilter;
@property (retain) LCXMLRequest *deleteRequest;
@property (retain) LCXMLRequest *repairRequest;
@property (getter=repairInProgress,setter=setRepairInProgress:) BOOL repairInProgress;
@property (retain,getter=repairStatusString,setter=setRepairStatusString:) NSString *repairStatusString;
@property (retain,getter=repairScriptOutput,setter=setRepairScriptOutput:) NSString *repairScriptOutput;
@property (retain,getter=repairStatusIcon,setter=setRepairStatusIcon:) NSImage *repairStatusIcon;
@property (assign,getter=scriptBeingRepaired,setter=setScriptBeingRepaired:) LCServiceScript *scriptBeingRepaired;
@property (retain) NSMutableString *xmlString;
@property (retain) NSString *xmlElement;
@end
