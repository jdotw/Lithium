//
//  LCIPRegistryNetwork.m
//  Lithium Console
//
//  Created by James Wilson on 2/10/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCIPRegistryNetwork.h"
#import "LCIPRegistryNetworkList.h"

@implementation LCIPRegistryNetwork

#pragma mark "Constructors"

- (id) init
{
	self = [super init];
	if (!self) return nil;
	
	children = [[NSMutableArray array] retain];
	childDictionary = [[NSMutableDictionary dictionary] retain];
	
	if (!self.xmlTranslation)
	{ self.xmlTranslation = [NSMutableDictionary dictionary]; }
	[self.xmlTranslation setObject:@"address" forKey:@"address"];
	
	return self;
}

- (void) dealloc
{
	[displayString release];
	[address release];
	[children release];
	[childDictionary release];
	[super dealloc];
}

#pragma mark "Entry Accessors"

@synthesize children;
- (void) insertObject:(LCIPRegistryEntry *)entry inChildrenAtIndex:(unsigned int)index
{
	[children insertObject:entry atIndex:index];
	[childDictionary setObject:entry forKey:[entry ip]];
	[self updateDisplayString];
}
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{
	[childDictionary removeObjectForKey:[[children objectAtIndex:index] ip]];
	[children removeObjectAtIndex:index];
	[self updateDisplayString];
}
@synthesize childDictionary;

#pragma mark "Accessors"

@synthesize displayString;
@synthesize address;
- (void) setAddress:(NSString *)value
{
	[address release];
	address = [value copy];
	[self updateDisplayString];
}

- (void) updateDisplayString
{
	self.displayString = [NSString stringWithFormat:@"%@ (%i)", self.address, [children count]];
}

- (NSString *) ip
{ return nil; }

@synthesize parent;

- (NSString *) uniqueIdentifier
{ return [NSString stringWithFormat:@"%@-%@", [(LCIPRegistryNetworkList *)parent uniqueIdentifier], self.address]; }

@end
