//
//  LCDemoLicense.h
//  Lithium Console
//
//  Created by James Wilson on 29/03/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class LCDemoLicense, LCCustomer;

@protocol LCDemoLicenseDelegate
- (void) demoLicenseGranted:(LCDemoLicense *)license;
- (void) demoLicenseNotGranted:(LCDemoLicense *)license error:(NSError *)error;
@end

@interface LCDemoLicense : NSObject 
{
	NSString *name;
	NSString *email;
	NSString *company;
	
	NSString *signedKey;

	id <LCDemoLicenseDelegate> delegate;
	
	NSURLConnection *urlConn;
	NSMutableData *receivedData;
	
	NSString *curXMLElement;
	NSMutableString *curXMLString;
}

@property (nonatomic,retain) NSString *name;
@property (nonatomic,retain) NSString *email;
@property (nonatomic,retain) NSString *company;

@property (nonatomic,retain) NSString *signedKey;

@property (nonatomic,assign) id <LCDemoLicenseDelegate> delegate;

- (void) requestLicenseForCustomer:(LCCustomer *)customer;

@end
