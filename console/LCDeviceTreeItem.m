//
//  LCDeviceTreeItem.m
//  Lithium Console
//
//  Created by James Wilson on 4/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCDeviceTreeItem.h"

@implementation LCDeviceTreeItem

- (void) dealloc
{
	[children release];
	[super dealloc];
}

#pragma mark Accessors

- (NSString *) displayString
{ return @"--NONE--"; }

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

- (int) opState
{ return 0; }

- (BOOL) selectable
{ return YES; }

@synthesize isDeviceTreeLeaf;

@end
