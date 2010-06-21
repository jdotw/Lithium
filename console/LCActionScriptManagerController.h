//
//  LCActionScriptManagerController.h
//  Lithium Console
//
//  Created by James Wilson on 25/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"
#import "LCXMLRequest.h"
#import "LCActionScript.h"
#import "LCActionScriptList.h"
#import "LCScriptUploadRequest.h"
#import "LCScriptDownloadRequest.h"
#import "LCLithiumSetupWindowController.h"

@interface LCActionScriptManagerController : NSObject 
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
	
	/* Script uploading/downloading */
	LCScriptUploadRequest *uploadRequest;
	LCScriptDownloadRequest *downloadRequest;
	NSURLConnection *urlConn;
	NSMutableData *receivedData;
	LCActionScript *scriptBeingInstalled;	
	BOOL scriptInstallInProgress;
	NSString *scriptInstallProgressString;
	
	/* Script List */
	LCActionScriptList *scriptList;
	
	/* Delete */
	LCXMLRequest *deleteRequest;
	
	/* Repair */
	LCXMLRequest *repairRequest;
	BOOL repairInProgress;
	NSString *repairStatusString;
	NSString *repairScriptOutput;
	NSImage *repairStatusIcon;
	LCActionScript *scriptBeingRepaired;
	
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

#pragma mark "Community Script Downloads"
- (IBAction) installCommunityScriptClicked:(NSArray *)arg;
- (IBAction) cancelInstallCommunityScriptClicked:(id)sender;

#pragma mark "Script Selection"
- (LCActionScript *) selectedScript;
- (LCActionScript *) selectedCommunityScript;

#pragma mark "Accessors"
- (LCActionScriptList *) scriptList;
- (void) setScriptList:(LCActionScriptList *)list;
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
- (LCActionScript *) scriptBeingRepaired;
- (void) setScriptBeingRepaired:(LCActionScript *)script;
- (BOOL) scriptInstallInProgress;
- (void) setScriptInstallInProgress:(BOOL)flag;
- (NSString *) scriptInstallProgressString;
- (void) setScriptInstallProgressString:(NSString *)string;


@property (nonatomic,retain) LCLithiumSetupWindowController *setupController;
@property (nonatomic,retain) NSWindow *uploadSheet;
@property (nonatomic,retain) NSWindow *downloadSheet;
@property (nonatomic,retain) NSArrayController *scriptArrayController;
@property (nonatomic,retain) NSArrayController *communityScriptArrayController;
@property (nonatomic,retain) NSWindow *repairSheet;
@property (nonatomic,retain) NSWindow *communityDownloadSheet;
@property (retain,getter=uploadRequest,setter=setUploadRequest:) LCScriptUploadRequest *uploadRequest;
@property (retain,getter=downloadRequest,setter=setDownloadRequest:) LCScriptDownloadRequest *downloadRequest;
@property (nonatomic,retain) NSURLConnection *urlConn;
@property (nonatomic,retain) NSMutableData *receivedData;
@property (nonatomic,retain) LCActionScript *scriptBeingInstalled;
@property (getter=scriptInstallInProgress,setter=setScriptInstallInProgress:) BOOL scriptInstallInProgress;
@property (retain,getter=scriptInstallProgressString,setter=setScriptInstallProgressString:) NSString *scriptInstallProgressString;
@property (nonatomic,retain) LCActionScriptList *scriptList;
@property (nonatomic,retain) LCXMLRequest *deleteRequest;
@property (nonatomic,retain) LCXMLRequest *repairRequest;
@property (getter=repairInProgress,setter=setRepairInProgress:) BOOL repairInProgress;
@property (retain,getter=repairStatusString,setter=setRepairStatusString:) NSString *repairStatusString;
@property (retain,getter=repairScriptOutput,setter=setRepairScriptOutput:) NSString *repairScriptOutput;
@property (retain,getter=repairStatusIcon,setter=setRepairStatusIcon:) NSImage *repairStatusIcon;
@property (assign,getter=scriptBeingRepaired,setter=setScriptBeingRepaired:) LCActionScript *scriptBeingRepaired;
@property (nonatomic,retain) NSMutableString *xmlString;
@property (nonatomic,retain) NSString *xmlElement;
@end
