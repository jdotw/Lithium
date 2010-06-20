//
//  LCLicenseKey.h
//  LCAdminTools
//
//  Created by James Wilson on 25/09/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#define KEY_NOTPROCESSED 0
#define KEY_DECODED 1
#define KEY_INVALID 2
#define KEY_INVALID_CUSTOMER 3
#define KEY_INVALID_EXPIRED 4
#define KEY_INVALID_DUPLICATE 5
#define KEY_INVALID_DUPLICATE_EXCLUSIVE 6
#define KEY_INVALID_WRONGTYPE 7
#define KEY_VALID 128

#define KEY_FLAG_NFR 8
#define KEY_FLAG_DEMO 16
#define KEY_FLAG_FREE_UNREG 32
#define KEY_FLAG_FREE_EXPIRED 64

@interface LCLicenseKey : NSObject 
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
	NSString *customerHash;
	int volume;
	int flags;
	int expiry;
	int version;
	NSString *expiryString;
	NSString *customerName;
	
	/* Dynamic Properties */
	NSString *statusString;
}

+ (LCLicenseKey *) keyWithID:(int)initKeyID customerName:(NSString *)initName string:(NSString *)initString;
- (id) initWithKeyID:(int)initKeyID customerName:(NSString *)initName string:(NSString *)initString;
- (BOOL) licensedForCustomerName:(NSString *)name;

#pragma mark "Properties"
@property (assign) int keyID;
@property (assign) int status;
@property (copy) NSString *encryptedKey;
@property (assign) int serial;
@property (copy) NSString *type;
@property (copy) NSString *product;
@property (copy) NSString *customerHash;
@property (assign) int volume;
@property (assign) int flags;
@property (assign) int expiry;
@property (assign) int version;
@property (copy) NSString *expiryString;
@property (copy) NSString *statusString;
@property (copy) NSString *typeString;
@property (copy) NSString *productString;
@property (copy) NSString *customerName;

@end
