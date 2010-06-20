//
//  LCBackupController.h
//  LCAdminTools
//
//  Created by James Wilson on 31/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <SecurityFoundation/SFAuthorization.h>

@interface LCBackupController : NSObject 
{
	BOOL backupInProgress;
	
	IBOutlet NSWindow *window;
	NSSavePanel *savePanel;

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
	
	NSString *backupFile;
}

#pragma mark "UI Actions"
- (IBAction) chooseFileClicked:(id)sender;
- (IBAction) backupClicked:(NSSavePanel *)sheet returnCode:(int)returnCode  contextInfo:(void  *)contextInfo;
- (IBAction) cancelClicked:(id)sender;

#pragma mark "Accessors"
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
- (BOOL) backupInProgress;
- (void) setBackupInProgress:(BOOL)flag;
- (NSString *) status;
- (void) setStatus:(NSString *)string;
- (NSImage *) statusIcon;
- (void) setStatusIcon:(NSImage *)icon;
- (NSString *) backupFile;
- (void) setBackupFile:(NSString *)string;


@end
