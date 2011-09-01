//
//  LCUpdateController.h
//  LCAdminTools
//
//  Created by James Wilson on 11/05/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <WebKit/WebKit.h>

@interface LCUpdateController : NSObject 
{
	/* UI Elements */
	IBOutlet NSTabView *contentTabView;			/* check / noupdate / update */
	IBOutlet NSWindow *progressSheet;
	IBOutlet NSWindow *window;
	IBOutlet NSTextField *noUpdateMessage;
	IBOutlet WebView *notesWebView;
	IBOutlet NSProgressIndicator *updateProgressIndicator;
	IBOutlet NSProgressIndicator *checkProgressIndicator;
	
	/* Update */
	NSString *updateVersion;					/* E.g 5.0.B1.1055 */
	int updateBuild;							/* E.g. 1055 */
	NSString *updateDate;						/* 20090508 */
	long updateSize;							/* Size in bytes */
	NSString *updateUrl;						/* Update download URL */
	NSString *updateNotesUrl;					/* Release notes URL */
	
	/* Check for Update */
	NSURLConnection *checkUrlConn;
	NSMutableData *checkData;
	NSMutableString *xmlString;
	
	/* Download and Install */
	NSURLDownload *updateDownload;
	NSURLResponse *updateDownloadResponse;
	long updatebytesReceived;
	float updateProgress;
	float updateMbReceived;
	NSString *updateStatus;
	BOOL userCanCancel;
	NSString *updateOperation;
	BOOL recentlyUpdated;
	
	/* CPU Type */
	BOOL x86_64;
}

#pragma mark "Check for Update Methods"
- (IBAction) checkForUpdateClicked:(id)sender;
- (IBAction) moreInfoClicked:(id)sender;
- (void) updateCheckFailed;
- (void) validateVersion;
@property (copy) NSString *updateVersion;
@property (assign) int updateBuild;
@property (copy) NSString *updateDate;
@property (assign) long updateSize;
@property (copy) NSString *updateUrl;
@property (copy) NSString *updateNotesUrl;
@property (readonly) NSString *localVersion;
@property (readonly) int localBuild;

#pragma mark "Install Update Methods"
- (IBAction) installUpdateClicked:(id)sender;
- (void) updateConfirmDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo;
- (IBAction) installUpdateCancelClicked:(id)sender;
- (void) downloadUpdateFailed;
- (void) installUpdateFailed;
@property (assign) float updateProgress;
@property (assign) float updateMbReceived;
@property (copy) NSString *updateStatus;
@property (assign) BOOL userCanCancel;
@property (copy) NSString *updateOperation;
@property (assign) BOOL recentlyUpdated;

#pragma mark "CPU Support"
@property (assign) BOOL x86_64;

@end

