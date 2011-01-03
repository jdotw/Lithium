//
//  LCConfigController.h
//  LCAdminTools
//
//  Created by James Wilson on 31/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface LCConfigController : NSObject 
{
	IBOutlet NSWindow *window;
	
	NSString *adminUsername;
	NSString *adminPassword;
	NSString *adminPasswordConfirm;
	
	NSString *dbUsername;
	NSString *dbPassword;
	NSString *dbPasswordConfirm;
	NSString *dbHostname;
	NSString *dbPort;
	
	NSString *httpRoot;
	
	BOOL authExternal;
	
	NSAlert *alert;
	
	BOOL sqlMetricRecording;
	
	BOOL deepSearch;
}

+ (LCConfigController *) masterController;

#pragma mark "UI Actions"
- (IBAction) saveConfigClicked:(id)sender;

#pragma mark "Config Read/Write"
- (void) readConfig;
- (NSString *) writeConfig;

#pragma mark "Accessors"
- (NSString *) adminUsername;
- (void) setAdminUsername:(NSString *)string;
- (NSString *) adminPassword;
- (void) setAdminPassword:(NSString *)string;
- (NSString *) adminPasswordConfirm;
- (void) setAdminPasswordConfirm:(NSString *)string;
- (NSString *) dbUsername;
- (void) setDbUsername:(NSString *)string;
- (NSString *) dbPassword;
- (void) setDbPassword:(NSString *)string;
- (NSString *) dbPasswordConfirm;
- (void) setDbPasswordConfirm:(NSString *)string;
- (NSString *) dbHostname;
- (void) setDbHostname:(NSString *)string;
- (NSString *) dbPort;
- (void) setDbPort:(NSString *)string;
- (NSString *) httpRoot;
- (void) setHttpRoot:(NSString *)string;
- (BOOL) authExternal;
- (void) setAuthExternal:(BOOL)flag;
- (BOOL) sqlMetricRecording;
- (void) setSqlMetricRecording:(BOOL)flag;
- (BOOL) deepSearch;
- (void) setDeepSearch:(BOOL)flag;

@end
