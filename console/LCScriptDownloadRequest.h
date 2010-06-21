//
//  LCScriptDownloadRequest.h
//  Lithium Console
//
//  Created by James Wilson on 31/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCActivity.h"
#import "LCCustomer.h"

@interface LCScriptDownloadRequest : NSObject 
{
	/* Request criteria */
	id customer;
	NSString *type;
	NSString *scriptName;
	NSString *localFilename;
	
	/* Op variables */
	id delegate;
	BOOL success;
	BOOL inProgress;
	NSURLConnection *urlConn;
	NSMutableData *receivedData;
	LCActivity *activity;	
	NSString *curXMLElement;
	NSMutableString *curXMLString;
	
	/* Results */
	NSString *scriptOutput;
	NSString *progressString;
	NSImage *progressIcon;
}

+ (LCScriptDownloadRequest *) downloadScript:(NSString *)initScriptName type:(NSString *)initType forCustomer:(LCCustomer *)initCustomer to:(NSString *)initLocalFilename;
- (LCScriptDownloadRequest *) initWithFilename:(NSString *)initScriptName type:(NSString *)initType forCustomer:(LCCustomer *)initCustomer  to:(NSString *)initLocalFilename;
- (LCScriptDownloadRequest *) init;
- (void) dealloc;
- (void) performAsyncRequest;
- (void) cancel;

#pragma mark HTTP Handling
- (void) connection:(NSURLConnection *)connection 
 didReceiveResponse:(NSURLResponse *)response;
-(void)	connection:(NSURLConnection *) connection
	didReceiveData:(NSData *) data;
-(NSURLRequest *)	connection:(NSURLConnection *)connection
			   willSendRequest:(NSURLRequest *)request
			  redirectResponse:(NSURLResponse *)redirectResponse;
-(void) connection:(NSURLConnection *)connection
		didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *) challenge;
-(void) connectionDidFinishLoading:(NSURLConnection *) connection;
- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error;
- (void) disabledRefreshTimerCallback;

#pragma mark "Accessor Methods"
- (id) customer;
- (void) setCustomer:(id)custent;
- (id) delegate;
- (void) setDelegate:(id)obj;
- (BOOL) success;
- (void) setSuccess:(BOOL)flag;
- (NSMutableData *) receivedData;
- (void) setReceivedData:(NSMutableData *)data;
- (BOOL) inProgress;
- (void) setInProgress:(BOOL)flag;
- (NSString *) progressString;
- (void) setProgressString:(NSString *)string;
- (NSString *) scriptOutput;
- (void) setScriptOutput:(NSString *)string;
- (NSImage *) progressIcon;
- (void) setProgressIcon:(NSImage *)icon;



@property (assign,getter=customer,setter=setCustomer:) id customer;
@property (nonatomic,retain) NSString *type;
@property (nonatomic,retain) NSString *scriptName;
@property (nonatomic,retain) NSString *localFilename;
@property (assign,getter=delegate,setter=setDelegate:) id delegate;
@property (getter=success,setter=setSuccess:) BOOL success;
@property (getter=inProgress,setter=setInProgress:) BOOL inProgress;
@property (nonatomic,retain) NSURLConnection *urlConn;
@property (nonatomic,retain) LCActivity *activity;
@property (nonatomic,retain) NSString *curXMLElement;
@property (nonatomic,retain) NSMutableString *curXMLString;
@property (retain,getter=scriptOutput,setter=setScriptOutput:) NSString *scriptOutput;
@property (retain,getter=progressString,setter=setProgressString:) NSString *progressString;
@property (retain,getter=progressIcon,setter=setProgressIcon:) NSImage *progressIcon;
@end
