//
//  LCLicense.h
//  Lithium Console
//
//  Created by James Wilson on 29/03/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class LCLicense, LCCustomer;

@protocol LCLicenseDelegate

- (void) licenseActivated:(LCLicense *)license;
- (void) licenseNotActivated:(LCLicense *)license error:(NSError *)error;

@end


@interface LCLicense : NSObject 
{
	NSString *name;
	NSString *company;
	NSString *email;
	
	NSString *key; // User-Supplied
	NSString *signedKey;
	
	id <LCLicenseDelegate> delegate;
	
	NSURLConnection *urlConn;
	NSMutableData *receivedData;
	NSMutableString *xmlString;
}

- (void) activateLicenseForCustomer:(LCCustomer *)customer;

@property (nonatomic,retain) NSString *name;
@property (nonatomic,retain) NSString *company;
@property (nonatomic,retain) NSString *email;

@property (nonatomic,retain) NSString *key; // User-Supplied
@property (nonatomic,retain) NSString *signedKey;

@property (nonatomic,assign) id <LCLicenseDelegate> delegate;

@property (nonatomic,readonly) BOOL requiresActivation;
@property (nonatomic,readonly) BOOL isValid;


@end
