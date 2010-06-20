//
//  LCLicenseKey.m
//  LCAdminTools
//
//  Created by James Wilson on 25/09/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCLicenseKey.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <openssl/sha.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <time.h>

time_t time(time_t *tloc);

int b64decode(unsigned char* str)
{
    unsigned char *cur, *start;
    int d, dlast, phase;
    unsigned char c;
    static int table[256] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
        41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
    };
	
    d = dlast = phase = 0;
    start = str;
    for (cur = str; *cur != '\0'; ++cur )
    {
        if(*cur == '\n' || *cur == '\r'){phase = dlast = 0; continue;}
        d = table[(int)*cur];
        if(d != -1)
        {
            switch(phase)
            {
				case 0:
					++phase;
					break;
				case 1:
					c = ((dlast << 2) | ((d & 0x30) >> 4));
					*str++ = c;
					++phase;
					break;
				case 2:
					c = (((dlast & 0xf) << 4) | ((d & 0x3c) >> 2));
					*str++ = c;
					++phase;
					break;
				case 3:
					c = (((dlast & 0x03 ) << 6) | d);
					*str++ = c;
					phase = 0;
					break;
            }
            dlast = d;
        }
    }
    *str = '\0';
    return str - start;
}

@implementation LCLicenseKey

+ (LCLicenseKey *) keyWithID:(int)initKeyID customerName:(NSString *)initName string:(NSString *)initString
{
	return [[[LCLicenseKey alloc] initWithKeyID:initKeyID customerName:initName string:initString] autorelease];
}

- (id) initWithKeyID:(int)initKeyID customerName:(NSString *)initName string:(NSString *)initString
{
	self = [super init];
	if (!self) return nil;
	
	/* Set Properties */
	self.keyID = initKeyID;
	self.encryptedKey = initString;
	self.customerName = initName;
	
	/*
	 * Decode the key
	 */
	
    char *pub_key = "-----BEGIN PUBLIC KEY-----\r\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDVRyTSjUU8r3+bQHIrx5p3AZBj\r\nUpiYEmRyOToD/C0U2ML9Fw/oFAYA4M5X+XmVJ92DitEk3KXetH9zkEMqCvVmWKwP\r\nqFQnakMU4f7n8/ynDwNLexMUhPvZnhP1ubSFeHFF6TEd5Hlnh3+3e5Of092SEYSf\r\n5b4puVQqmUuisHjchwIDAQAB\r\n-----END PUBLIC KEY-----";
    BIO *bio;
	
	const char *enc_str = [self.encryptedKey UTF8String];
	char *raw_str;
	int num;
	
    /* Decode key */
    if((bio = BIO_new_mem_buf(pub_key, strlen(pub_key)+1)))
    {
		RSA* rsa_key = 0;
		if(PEM_read_bio_RSA_PUBKEY(bio, &rsa_key, NULL, NULL))
		{
			unsigned char dst[strlen(enc_str)+1];
			unsigned char *src = (unsigned char *) strdup (enc_str);
			int src_size = b64decode(src);
			num = RSA_public_decrypt(src_size, src, dst, rsa_key, RSA_PKCS1_PADDING);
			if (num != -1)
			{
				dst[num] = '\0';
				raw_str = strdup ((char *)dst);
				status = KEY_DECODED;    /* Key decoded */
			}
			else
			{
				char buf[1024];
				ERR_error_string(ERR_get_error(), buf);
				status = KEY_INVALID;    /* Invalid key */
				return self;
			}
			RSA_free(rsa_key);
		}
		else
		{
			status = KEY_INVALID;    /* Invalid key */
			return self;
		}
		BIO_free(bio);
    }
    else
    {
		status = KEY_INVALID;
		return self;
    }
	
	/*
	 * Interpret Decoded Key String 
	 */

	
	/* 
     * Iterpret the key
     */
    char *temp = strdup (raw_str);
    char *tempptr = temp;
	
    /* License serial num */
    char *endptr = strchr(tempptr, ':');
    if (!endptr)
    { self.status = KEY_INVALID; return self; }
    *endptr = '\0';
    self.serial = atol (tempptr);
    tempptr = endptr+1;
	
    /* License type */
    endptr = strchr(tempptr, ':');
    if (!endptr)
    { self.status = KEY_INVALID; return self; }
    *endptr = '\0';
    self.type = [NSString stringWithUTF8String:tempptr];
    tempptr = endptr+1;
	
    /* Customer hash */
    endptr = strchr(tempptr, ':');
    if (!endptr)
    { self.status = KEY_INVALID; return self; }
    *endptr = '\0';
	self.customerHash = [NSString stringWithUTF8String:tempptr];
    tempptr = endptr+1;
	
    /* Product ID */
    endptr = strchr(tempptr, ':');
    if (!endptr)
    { self.status = KEY_INVALID; return self; }
    *endptr = '\0';
	self.product = [NSString stringWithUTF8String:tempptr];
    tempptr = endptr+1;
	
    /* Version */
    endptr = strchr(tempptr, ':');
    if (!endptr)
    { self.status = KEY_INVALID; return self; }
    *endptr = '\0';
	self.version = (int) strtol (tempptr, NULL, 16);
    tempptr = endptr+1;
	
    /* Volume */
    endptr = strchr(tempptr, ':');
    if (!endptr)
    { self.status = KEY_INVALID; return self; }
    *endptr = '\0';
	self.volume = (int) strtoul (tempptr, NULL, 16);
    tempptr = endptr+1;
	
    /* Flags */
    endptr = strchr(tempptr, ':');
    if (!endptr)
    { self.status = KEY_INVALID; return self; }
    *endptr = '\0';
	self.flags = (int) strtoul (tempptr, NULL, 16);
    tempptr = endptr+1;
	
    /* Expiry */
	self.expiry = (int) strtoul (tempptr, NULL, 16);
	
    free (temp);
	
    /* Check for expiry */
    if (self.expiry > 0 && self.expiry < time(NULL))
    {
		/* Key has expired */
		self.status = KEY_INVALID_EXPIRED;
		return self;
    }
	
    /* If control reaches here, the key is valid */
    self.status = KEY_VALID;
	
	
	return self;
}

@synthesize keyID;
@synthesize status;
- (void) setStatus:(int)value
{
	status = value;
	switch (value)
	{
		case 0:
			self.statusString = @"Not Processed";
			break;
		case 1:
			self.statusString = @"Key Decoded";
			break;
		case 2:
			self.statusString = @"Invalid";
			break;
		case 3:
			self.statusString = @"Invalid (Wrong Customer)";
			break;
		case 4:
			self.statusString = @"Expired";
			break;
		case 5:
		case 6:
			self.statusString = @"Duplicate";
			break;
		case 7:
			self.statusString = @"Invalid (Wrong Type)";
			break;
		case 128:
			self.statusString = @"Valid";
			break;
		default:
			self.statusString = @"Unknown";
	}
}

@synthesize encryptedKey;
@synthesize serial;
@synthesize customerHash;
@synthesize type;
- (void) setType:(NSString *)value
{
	
	[type release];
	type = [value copy];
	if ([type isEqualToString:@"CSTD"]) self.typeString = @"Standard Customer License";
	else if ([type isEqualToString:@"CMSP"]) self.typeString = @"MSP Customer License";
	else if ([type isEqualToString:@"CACN"]) self.typeString = @"ACN Customer License";
	else if ([type isEqualToString:@"RACN"]) self.typeString = @"Lithium-ACN Master License";
	else if ([type isEqualToString:@"RMSP"]) self.typeString = @"Lithium-MSP Master License";
	else self.typeString = @"Unknown";
}

@synthesize product;
- (void) setProduct:(NSString *)value
{
	[product release];
	product = [value copy];
	
	if ([product isEqualToString:@"lithium"]) self.productString = @"Lithium";
	else if ([product isEqualToString:@"free"]) self.productString = @"Demo";
	else if ([product isEqualToString:@"demo"]) self.productString = @"Demo";
	else if ([product isEqualToString:@"regdemo"]) self.productString = @"Registered Demo";
	else if ([product isEqualToString:@"limited"]) self.productString = @"Lithium LE";
	else self.productString = @"Unknown";	
}

@synthesize volume;
@synthesize flags;
@synthesize expiry;
- (void) setExpiry:(int)value
{
	expiry = value;
	if (expiry == 0)
	{ self.expiryString = @"Never"; }
	else 
	{ self.expiryString = [[NSDate dateWithTimeIntervalSince1970:(NSTimeInterval)expiry] description]; }
}

- (NSImage *) smallIcon
{ return [NSImage imageNamed:@"mortgage_16.tif"]; }

@synthesize statusString;

@synthesize typeString;
@synthesize productString;

@synthesize expiryString;

@synthesize version;

@synthesize customerName;

- (BOOL) licensedForCustomerName:(NSString *)name
{
	if ([self.type isEqualToString:@"RACN"] || [self.type isEqualToString:@"RMSP"]) return YES;
	
	unsigned char custhash_raw[SHA_DIGEST_LENGTH];
    char *custhash;
    SHA1 ((const unsigned char *)[name UTF8String], strlen([name UTF8String]), custhash_raw);
    asprintf (&custhash, "%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x", custhash_raw[0], custhash_raw[1], custhash_raw[2], custhash_raw[3],
			  custhash_raw[4], custhash_raw[5], custhash_raw[6], custhash_raw[7], custhash_raw[8], custhash_raw[9], custhash_raw[10],
			  custhash_raw[11], custhash_raw[12], custhash_raw[13], custhash_raw[14], custhash_raw[15], custhash_raw[16], custhash_raw[17],
			  custhash_raw[18], custhash_raw[19]);
    if ([[NSString stringWithUTF8String:custhash] isEqualToString:customerHash])
    {
		/* Customer matches */
		return YES;
    }
	else
	{
		return NO;
	}	
}

@end

