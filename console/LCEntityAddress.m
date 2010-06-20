//
//  LCEntityAddress.m
//  Lithium Console
//
//  Created by James Wilson on 15/08/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCEntityAddress.h"
#import "LCEntityDescriptor.h"

@implementation LCEntityAddress

+ (LCEntityAddress *) addressWithString:(NSString *)string
{
	return [[[LCEntityAddress alloc] initWithString:string] autorelease];
}

+ (LCEntityAddress *) addressForEntity:(id)entity
{
	return [[[LCEntityAddress alloc] initForEntity:entity] autorelease];
}

- (LCEntityAddress *) initWithString:(NSString *)string
{
	/* Create new LCEntityAddress from an address string */
	int i;
	
	[self init];	
	
	NSArray *comparray = [string componentsSeparatedByString:@":"];
	for (i=0; i < [comparray count]; i++)
	{
		switch (i)
		{
			case 0: type = [[NSNumber numberWithInt:[[comparray objectAtIndex:i] intValue]] retain];
				break;
			case 1: cust = [[comparray objectAtIndex:i] retain];
				break;
			case 2: site = [[comparray objectAtIndex:i] retain];
				break;
			case 3: dev = [[comparray objectAtIndex:i] retain];
				break;
			case 4: cnt = [[comparray objectAtIndex:i] retain];
				break;
			case 5: obj = [[comparray objectAtIndex:i] retain];
				break;
			case 6: met = [[comparray objectAtIndex:i] retain];
				break;
			case 7: trg = [[comparray objectAtIndex:i] retain];
				break;
		}
	}
	
	return self;
}

- (LCEntityAddress *) initForEntity:(id)entity
{
	[self init];

	LCEntityDescriptor *descriptor = [LCEntityDescriptor descriptorForEntity:entity];
	type = [[NSNumber numberWithInt:(int)[(LCEntity *)entity type]] retain];
	cust = [[descriptor cust_name] retain];
	site = [[descriptor site_name] retain];
	dev = [[descriptor dev_name] retain];
	cnt = [[descriptor cnt_name] retain];
	obj = [[descriptor obj_name] retain];
	met = [[descriptor met_name] retain];
	trg = [[descriptor trg_name] retain];

	return self;
}

- (LCEntityAddress *) init
{
	[super init];
	return self;
}

- (void) dealloc
{
	[type release];
	[cust release];
	[site release];
	[dev release];
	[cnt release];
	[obj release];
	[met release];
	[trg release];
	[super dealloc];
}

- (NSString *) addressString
{
	switch ([type intValue])
	{
		case 1: return [NSString stringWithFormat:@"%@:%@", type, cust];
		case 2: return [NSString stringWithFormat:@"%@:%@:%@", type, cust, site]; 
		case 3: return [NSString stringWithFormat:@"%@:%@:%@:%@", type, cust, site, dev];
		case 4: return [NSString stringWithFormat:@"%@:%@:%@:%@:%@", type, cust, site, dev, cnt];
		case 5: return [NSString stringWithFormat:@"%@:%@:%@:%@:%@:%@", type, cust, site, dev, cnt, obj];
		case 6: return [NSString stringWithFormat:@"%@:%@:%@:%@:%@:%@:%@", type, cust, site, dev, cnt, obj, met];
		case 7: return [NSString stringWithFormat:@"%@:%@:%@:%@:%@:%@:%@:%@", type, cust, site, dev, cnt, obj, met, trg];
	}
	
	return nil;
}

@synthesize type;
@synthesize cust;
@synthesize site;
@synthesize dev;
@synthesize cnt;
@synthesize obj;
@synthesize met;
@synthesize trg;

@end
