//
//  LCCoreLicenseKey.m
//  Lithium Console
//
//  Created by James Wilson on 6/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCoreLicenseKey.h"


@implementation LCCoreLicenseKey

- (id) init
{
	self = [super init];
	if (!self) return nil;

	if (!self.xmlTranslation)
	{
		self.xmlTranslation = [NSMutableDictionary dictionary];
		[self.xmlTranslation setObject:@"keyID" forKey:@"id"];
		[self.xmlTranslation setObject:@"status" forKey:@"status"];
		[self.xmlTranslation setObject:@"encryptedKey" forKey:@"enc"];
		[self.xmlTranslation setObject:@"serial" forKey:@"serial"];
		[self.xmlTranslation setObject:@"type" forKey:@"type"];
		[self.xmlTranslation setObject:@"product" forKey:@"product"];
		[self.xmlTranslation setObject:@"volume" forKey:@"volume"];
		[self.xmlTranslation setObject:@"flags" forKey:@"flags"];
		[self.xmlTranslation setObject:@"expiry" forKey:@"expiry"];
	}	
	
	return self;
}

- (void) dealloc
{
	[customer release];
	[encryptedKey release];
	[type release];
	[product release];
	[super dealloc];
}

#pragma mark "Accessors"
@synthesize customer;
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

@end
