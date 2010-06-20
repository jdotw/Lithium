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
@property (assign) LCCustomer *customer;
@property (assign) int keyID;
@property (assign) int status;
@property (copy) NSString *encryptedKey;
@property (assign) int serial;
@property (copy) NSString *type;
@property (copy) NSString *product;
@property (assign) int volume;
@property (assign) int flags;
@property (assign) int expiry;
@property (copy) NSString *expiryString;
@property (copy) NSString *statusString;
@property (copy) NSString *typeString;
@property (copy) NSString *productString;

@end
