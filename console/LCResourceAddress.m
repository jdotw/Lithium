//
//  LCResourceAddress.m
//  Lithium Console
//
//  Created by James Wilson on 21/06/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCResourceAddress.h"


@implementation LCResourceAddress

#pragma mark "Initialisation"

+ (LCResourceAddress *) addressWithString:(NSString *)string
{
	return [[[LCResourceAddress alloc] initWithString:string] autorelease];
}

+ (LCResourceAddress *) address
{
	return [[[LCResourceAddress alloc] init] autorelease];
}

- (LCResourceAddress *) initWithString:(NSString *)string
{
	/* Create new LCResourceAddress from an address string */
	int i;
	
	[self init];	
	
	NSArray *comparray = [string componentsSeparatedByString:@":"];
	for (i=0; i < [comparray count]; i++)
	{
		id object = nil;
		NSString *key = nil;

		switch (i)
		{
			case 0:	
				key = @"cluster";
				object = [comparray objectAtIndex:i];
				break;
			case 1: 
				key = @"node";
				object = [comparray objectAtIndex:i];
				break;
			case 2:
				key = @"type_int";
				object = [NSNumber numberWithInt:[[comparray objectAtIndex:i] intValue]];
				break;
			case 3: 
				key = @"ident_int";
				object = [NSNumber numberWithInt:[[comparray objectAtIndex:i] intValue]];
				break;
			case 4: 
				key = @"ident_str";
				object = [comparray objectAtIndex:i];
				break;
		}
		
		if (key && object)
		{ [properties setObject:object forKey:key]; }
	}
	
	return self;
}

- (LCResourceAddress *) init
{
	[super init];
	
	properties = [[NSMutableDictionary dictionary] retain];
	
	return self;
}

- (void) dealloc 
{
	[properties release];
	[super dealloc];
}

#pragma mark "Address String Methods"

- (NSString *) addressString
{
	return [NSString stringWithFormat:@"%@:%@:%@:%@:%@", [self cluster], [self node], [self typeInteger], [self idInteger], [self idString]];
}

#pragma mark "Accessor Methods"

@synthesize properties;

- (NSString *) cluster
{ return [properties objectForKey:@"cluster"]; }
- (NSString *) node
{ return [properties objectForKey:@"node"]; }
- (NSString *) typeInteger
{ return [properties objectForKey:@"type_int"]; }
- (NSString *) idString
{ return [properties objectForKey:@"ident_str"]; }
- (NSString *) idInteger 
{ return [properties objectForKey:@"ident_int"]; }

@end
