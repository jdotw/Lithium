//
//  LCXMLNode.m
//  Lithium Console
//
//  Created by James Wilson on 13/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCXMLNode.h"


@implementation LCXMLNode

- (id) init
{
	[super init];
	
	properties = [[NSMutableDictionary dictionary] retain];
	children = [[NSMutableArray array] retain];
	value = [[NSMutableString string] retain];
	
	return self;
}

- (void) dealloc
{
	[properties release];
	[children release];
	[name release];
	[value release];
	[super dealloc];
}

@synthesize name;
@synthesize value;
@synthesize children;
@synthesize properties;

@end
