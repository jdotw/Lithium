//
//  LCRestoreController.h
//  LCAdminTools
//
//  Created by James Wilson on 31/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <SecurityFoundation/SFAuthorization.h>

@interface LCRestoreController : NSObject 
{
	BOOL restoreInProgress;
	
	IBOutlet NSWindow *window;
	NSOpenPanel *openPanel;
	
	AuthorizationRef authRef;
	
	NSString *stopStatus;
	NSString *dbStatus;
	NSString *dataStatus;
	NSString *webStatus;
	NSString *archiveStatus;
	NSString *restartStatus;
	
	NSString *status;
	NSImage *statusIcon;
	
	FILE *pipe;
	NSTimer *pollingTimer;
	
	NSString *restoreFile;	
}

#pragma mark "UI Actions"
- (IBAction) chooseFileClicked:(id)sender;
- (IBAction) restoreClicked:(NSSavePanel *)sheet returnCode:(int)returnCode  contextInfo:(void  *)contextInfo;
- (IBAction) cancelClicked:(id)sender;

#pragma mark "Properties"
- (NSString *) stopStatus;
- (void) setStopStatus:(NSString *)string;
- (NSString *) restartStatus;
- (void) setRestartStatus:(NSString *)string;
- (NSString *) dbStatus;
- (void) setDbStatus:(NSString *)string;
- (NSString *) webStatus;
- (void) setWebStatus:(NSString *)string;
- (NSString *) dataStatus;
- (void) setDataStatus:(NSString *)string;
- (NSString *) archiveStatus;
- (void) setArchiveStatus:(NSString *)string;
- (BOOL) restoreInProgress;
- (void) setRestoreInProgress:(BOOL)flag;
- (NSString *) status;
- (void) setStatus:(NSString *)string;
- (NSImage *) statusIcon;
- (void) setStatusIcon:(NSImage *)icon;
- (NSString *) restoreFile;
- (void) setRestoreFile:(NSString *)string;

@end
