//
//  LCXMLRequest.h
//  Lithium Console
//
//  Created by James Wilson on 15/08/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCResourceAddress.h"
#import "LCEntityAddress.h"
#import "LCAuthenticator.h"
#import "LCActivity.h"
#import "LCXMLParseOperation.h"

@interface LCXMLRequest : NSObject 
{
	/* Request criteria */
	id cust;
	LCResourceAddress *resource;
	LCEntityAddress *entity;
	NSString *xmlname;
	NSTimeInterval refsec;
	NSXMLDocument *xmlout;
	
	/* Op variables */
	int priority;
	id delegate;
	id xmldelegate;
	id threadedXmlDelegate;
	LCXMLParseOperation *xmlParser;
	NSTimeInterval timeout;
	BOOL success;
	BOOL debug;
	BOOL inProgress;
	NSURLConnection *urlConn;
	NSMutableData *receivedData;
	LCActivity *activity;
	NSString *lastUsername;
	NSString *lastPassword;
}

#define XMLREQ_PRIO_HIGH 1
#define XMLREQ_PRIO_NORMAL 0
#define XMLREQ_PRIO_LOW -1

+ (LCXMLRequest *) requestWithCriteria:(id)initcust 
							  resource:(LCResourceAddress *)initres 
								entity:(LCEntityAddress *)initent 
							   xmlname:(NSString *)initxmlname 
								refsec:(time_t)initrefsec
								xmlout:(NSXMLDocument *)initxmlout;
- (LCXMLRequest *) initWithCriteria:(id)initcust 
						   resource:(LCResourceAddress *)initres 
							 entity:(LCEntityAddress *)initent 
							xmlname:(NSString *)initxmlname 
							 refsec:(time_t)initrefsec 
							 xmlout:(NSXMLDocument *)initxmlout;
- (LCXMLRequest *) init;

- (void) performAsyncRequest;
- (void) performQueuedRequest;
- (void) cancel;

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
- (void) parserDidFinish;

#pragma mark "Accessor Methods"
- (id) customer;
- (void) setCustomer:(id)entity;
- (LCResourceAddress *) resource;
- (void) setResource:(LCResourceAddress *)resaddr;
- (LCEntityAddress *) entity;
- (void) setEntity:(LCEntityAddress *)entaddr;
- (NSString *) xmlname;
- (void) setXMLname:(NSString *) name;
- (NSTimeInterval) refsec;
- (void) setRefsec:(NSTimeInterval)time;
- (NSXMLDocument *) xmlout;
- (void) setXMLout:(NSXMLDocument *)doc;
- (id) delegate;
- (void) setDelegate:(id)obj;
- (id) xmlDelegate;
- (NSString *) requestDescription;
- (void) setXMLDelegate:(id)obj;
- (BOOL) success;
- (void) setSuccess:(BOOL)flag;
- (BOOL) debug;
- (void) setDebug:(BOOL)flag;
- (NSMutableData *) receivedData;
- (void) setReceivedData:(NSMutableData *)data;
- (int) priority;
- (void) setPriority:(int)newPriority;
- (BOOL) inProgress;
- (void) setInProgress:(BOOL)flag;
- (NSString *) lastUsername;
- (void) setLastUsername:(NSString *)string;
- (NSString *) lastPassword;
- (void) setLastPassword:(NSString *)string;

@property (assign,getter=customer,setter=setCustomer:) id cust;
@property (retain,getter=xmlname) NSString *xmlname;
@property (getter=refsec,setter=setRefsec:) NSTimeInterval refsec;
@property (retain,getter=xmlout) NSXMLDocument *xmlout;
@property (assign,setter=setXMLDelegate:) id xmldelegate;
@property (assign) id threadedXmlDelegate;
@property NSTimeInterval timeout;
@property (getter=success,setter=setSuccess:) BOOL success;
@property (getter=debug,setter=setDebug:) BOOL debug;
@property (getter=inProgress,setter=setInProgress:) BOOL inProgress;
@property (retain) NSURLConnection *urlConn;
@property (retain) LCActivity *activity;
@property (retain,getter=lastUsername,setter=setLastUsername:) NSString *lastUsername;
@property (retain,getter=lastPassword,setter=setLastPassword:) NSString *lastPassword;
@end
