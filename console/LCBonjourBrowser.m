//
//  LCBonjourBrowser.m
//  Lithium Console
//
//  Created by James Wilson on 30/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBonjourBrowser.h"
#import "LCBonjourBrowserList.h"

@implementation LCBonjourBrowser

#pragma mark "Constructors"

- (id) init
{
	self = [super init];
	if (!self) return nil;

	/* Create properties */
	children = [[NSMutableArray array] retain];
	childDictionary = [[NSMutableDictionary dictionary] retain];
	
	/* Setup XML Translation */
	if (!self.xmlTranslation)
	{ self.xmlTranslation = [NSMutableDictionary dictionary]; }
	[self.xmlTranslation setObject:@"service" forKey:@"servicename"];
	[self.xmlTranslation setObject:@"desc" forKey:@"desc"];
	
	return self;
}

- (void) dealloc
{
	[children release];
	[childDictionary release];
	[service release];
	[desc release];
	[super dealloc];
}

#pragma mark "Services Accessors"

@synthesize children;
- (void) insertObject:(LCBonjourService *)newService inChildrenAtIndex:(unsigned int)index
{
	[children insertObject:newService atIndex:index];
	[childDictionary setObject:newService forKey:[newService name]];
	[self updateDisplayString];
}
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{
	[childDictionary removeObjectForKey:[[children objectAtIndex:index] name]];
	[children removeObjectAtIndex:index];
	[self updateDisplayString];
}
@synthesize childDictionary;

#pragma mark "Properties"

@synthesize service;
@synthesize desc;
@synthesize displayString;

- (void) updateDisplayString
{ self.displayString = [NSString stringWithFormat:@"%@ (%i)", self.desc, children.count]; }

- (NSString *) ip
{ return nil; }

@synthesize parent;

- (NSString *) uniqueIdentifier
{ return [NSString stringWithFormat:@"%@-%@", [(LCBonjourBrowserList *)parent uniqueIdentifier], self.service]; }


@end
