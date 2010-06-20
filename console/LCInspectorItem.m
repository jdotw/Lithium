//
//  LCInspectorItem.m
//  Lithium Console
//
//  Created by James Wilson on 2/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspectorItem.h"
#import "LCInspectorViewController.h"

@implementation LCInspectorItem

/*
 * The foundation of all Inspector items
 */

#pragma mark "Constructors"

+ (LCInspectorItem *) itemWithTarget:(id)initTarget forController:(id)initController;
{
	return [[[LCInspectorItem alloc] initWithTarget:initTarget forController:initController] autorelease];
}

+ (LCInspectorItem *) itemWithTarget:(id)initTarget
{
	return [[[LCInspectorItem alloc] initWithTarget:initTarget] autorelease];
}

- (LCInspectorItem *) initWithTarget:(id)initTarget forController:(id)initController
{
	[self initWithTarget:initTarget];
	[self setController:initController];
	return self;
}

- (LCInspectorItem *) initWithTarget:(id)initTarget
{
	[super initWithNibName:[self nibName] bundle:nil];
	[self loadView];
	
	viewControllers = [[NSMutableArray array] retain];
	[self setTarget:initTarget];
	[self setExpandByDefault:YES];
	
	self.displayString = @"None";
	
	return self;
}


- (void) dealloc
{
	[displayString release];
	[viewControllers release];
	[super dealloc];
}

#pragma mark "View Controllers"

@synthesize controller;

- (void) insertObject:(id)viewController inViewControllersAtIndex:(unsigned int)index
{ 
	[viewController setController:controller];
	[viewControllers insertObject:viewController atIndex:index]; 
}

- (void) removeObjectFromViewControllersAtIndex:(unsigned int)index
{ [viewControllers removeObjectAtIndex:index]; }

- (BOOL) leafNode
{ return NO; }

#pragma mark "Target Methods"

/* 
 * The target WILL NOT change throughout the life of the item 
 */

@synthesize target;

#pragma mark "Display String"

@synthesize displayString;

#pragma mark "View Height Methods"

- (void) adjustViewHeightsBy:(float)delta
{
	/* By default we evenly adjust the views */
	
	float perDelta = delta / (float) [viewControllers count];
	for (LCInspectorViewController *subView in viewControllers)
	{ [subView setRowHeight:([subView rowHeight] + perDelta)]; }
}

- (void) adjustViewHeightsToDefault
{
	for (LCInspectorViewController *subView in viewControllers)
	{ [subView setRowHeight:[subView defaultHeight]]; }	
}

#pragma mark "View Management"

- (void) removedFromInspector
{
	/* Prepare to be released and dealloced */
	for (LCInspectorViewController *viewController in viewControllers)
	{ [viewController removedFromInspector]; }
	[controllerAlias setContent:nil];
}

#pragma mark "Accessors"

- (NSString *) nibName
{ return @"InspectorItemView"; }

- (float) rowHeight
{ return 17.0; }

@synthesize expandByDefault;

- (BOOL) allowsResize
{ return YES; }

@synthesize viewControllers;

@end
