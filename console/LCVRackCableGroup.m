//
//  LCVRackCableGroup.m
//  Lithium Console
//
//  Created by James Wilson on 20/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCVRackCableGroup.h"


@implementation LCVRackCableGroup

#pragma mark Constructors

- (id) init
{
	self = [super init];
	if (!self) return nil;
	
	/* Create initial UUID */
	CFUUIDRef uuidRef = CFUUIDCreate(nil);
	self.uuid = (NSString *)CFUUIDCreateString(nil, uuidRef);
    CFRelease(uuidRef);	
	
	/* Setup XML Translation */
	if (!self.xmlTranslation)
	{
		self.xmlTranslation = [NSMutableDictionary dictionary];
		[self.xmlTranslation setObject:@"uuid" forKey:@"uuid"];
		[self.xmlTranslation setObject:@"desc" forKey:@"desc"];
	}		
	
	return self;
}

- (void) dealloc
{
	[uuid release];
	[desc release];
	[super dealloc];
}

- (NSString *) xmlRootElement
{ return @"cablegroup"; }

#pragma mark "Document Import"

- (LCVRackCableGroup *) initWithCoder:(NSCoder *)decoder
{
	/*
	 * This is only used for document importing 
	 */
	[self init];
	
	NSDictionary *properties = [decoder decodeObjectForKey:@"properties"];
	self.desc = [properties objectForKey:@"desc"];
	
    return self;
}

@synthesize uuid;
@synthesize desc;

@end
