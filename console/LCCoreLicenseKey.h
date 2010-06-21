//
//  LCCoreLicenseKey.h
//  Lithium Console
//
//  Created by James Wilson on 6/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"

@interface LCCoreLicenseKey : LCXMLObject 
{
	/* Properties */
	int keyID;
	int status;
	NSString *encryptedKey;
	int serial;
	NSString *type;
	NSString *product;
	NSString *typeString;
	NSString *productString;
	int volume;
	int flags;
	int expiry;
	NSString *expiryString;
	
	/* Dynamic Properties */
	NSString *statusString;
	
	/* Related Objects */
	LCCustomer *customer;	
}

#pragma mark "Properties"
@property (nonatomic, assign) LCCustomer *customer;
@property (nonatomic, assign) int keyID;
@property (nonatomic, assign) int status;
@property (nonatomic,copy) NSString *encryptedKey;
@property (nonatomic, assign) int serial;
@property (nonatomic,copy) NSString *type;
@property (nonatomic,copy) NSString *product;
@property (nonatomic, assign) int volume;
@property (nonatomic, assign) int flags;
@property (nonatomic, assign) int expiry;
@property (nonatomic,copy) NSString *expiryString;
@property (nonatomic,copy) NSString *statusString;
@property (nonatomic,copy) NSString *typeString;
@property (nonatomic,copy) NSString *productString;

@end
