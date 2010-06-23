//
//  MBScriptUploadRequest.h
//  ModuleBuilder
//
//  Created by James Wilson on 23/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MBCoreDeployment.h"

@interface MBModuleUploadRequest : NSObject 
{
	/* Request criteria */
	MBCoreDeployment *core;
	NSData *moduleData;
	NSString *name;
	
	/* Op variables */
	id delegate;
	BOOL success;
	BOOL inProgress;
	NSURLConnection *urlConn;
	NSMutableData *receivedData;
	NSString *curXMLElement;
	NSMutableString *curXMLString;
	
	/* Results */
	NSString *status;
	NSString *progressString;
	NSImage *progressIcon;
}

#pragma mark "Constructors"
+ (MBModuleUploadRequest *) uploadModule:(NSData *)initModuleData named:(NSString *)initName toCore:(MBCoreDeployment *)initCore;
- (MBModuleUploadRequest *) initWithData:(NSData *)initModuleData named:(NSString *)initName forCore:(MBCoreDeployment *)initCore;
- (MBModuleUploadRequest *) init;

#pragma mark "Request Manipulation"
- (void) performAsyncRequest;
- (void) cancel;

#pragma mark "Properties"
@property (retain) MBCoreDeployment *core;
@property (assign) id delegate;
@property (assign) BOOL success;
@property (assign) BOOL inProgress;
@property (copy) NSString *status;
@property (copy) NSString *progressString;
@property (copy) NSImage *progressIcon;


@end
