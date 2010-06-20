//
//  LCSite.m
//  Lithium Console
//
//  Created by James Wilson on 20/07/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCSite.h"
#import "LCDevice.h"

@implementation LCSite

- (LCSite *) init
{
	[super init];
	
	[self.xmlTranslation setObject:@"addressLine1" forKey:@"addr1"];
	[self.xmlTranslation setObject:@"addressLine2" forKey:@"addr2"];
	[self.xmlTranslation setObject:@"addressLine3" forKey:@"addr3"];
	[self.xmlTranslation setObject:@"suburb" forKey:@"suburb"];
	[self.xmlTranslation setObject:@"state" forKey:@"state"];
	[self.xmlTranslation setObject:@"postcode" forKey:@"postcode"];
	[self.xmlTranslation setObject:@"country" forKey:@"country"];
	[self.xmlTranslation setObject:@"longitude" forKey:@"longitude"];
	[self.xmlTranslation setObject:@"latitude" forKey:@"latitude"];

	self.browserViewType = 1;

	return self;
}

- (void) dealloc
{
	[addressLine1 release];
	[addressLine2 release];
	[addressLine3 release];
	[suburb release];
	[state release];
	[postcode release];
	[country release];
	[super dealloc];
}

- (Class) childClass
{ return [LCDevice class]; }

@synthesize addressLine1;
@synthesize addressLine2;
@synthesize addressLine3;
@synthesize suburb;
@synthesize state;
@synthesize postcode;
@synthesize country;
@synthesize longitude;
@synthesize latitude;

- (void) setSuburb:(NSString *)string
{
	if (![string isEqualToString:suburb])
	{
		[suburb release];
		suburb = [string copy];
		
		if ([self.suburb length] > 0)
		{ self.displayString = [NSString stringWithFormat:@"%@ %@", self.desc, self.suburb]; }
	}
}

@end
