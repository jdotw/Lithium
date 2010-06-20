//
//  LCBrowserTreeItem.m
//  Lithium Console
//
//  Created by James Wilson on 25/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserTreeItem.h"


@implementation LCBrowserTreeItem

- (id) init
{
	self = [super init];

	self.selectable = YES;
	self.displayString = @"--NONE--";
	self.uniqueIdentifier = [self className];
	self.rowHeight = 16.0;
	
	return self;
}
	

- (void) dealloc
{
	[displayString release];
	[treeIcon release];
	[children release];
	[uniqueIdentifier release];
	[super dealloc];
}

#pragma mark Properties

@synthesize displayString;

@synthesize rowHeight;

@synthesize children;
- (void) setChildren:(NSMutableArray *)value
{
	[children release];
	children = [value mutableCopy];
}
- (void) insertObject:(id)obj inChildrenAtIndex:(unsigned int)index
{
	[children insertObject:obj atIndex:index]; 
}
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{
	[children removeObjectAtIndex:index];
}

@synthesize isBrowserTreeLeaf;
@synthesize treeIcon;
@synthesize opState;
@synthesize refreshInProgress;
@synthesize selectable;
@synthesize uniqueIdentifier;

- (id) parent
{ return nil; }

@synthesize sortString;

@end

