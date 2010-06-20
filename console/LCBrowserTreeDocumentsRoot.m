//
//  LCBrowserTreeDocumentsRoot.m
//  Lithium Console
//
//  Created by James Wilson on 16/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserTreeDocumentsRoot.h"

#import "LCBrowserTreeScenes.h"
#import "LCBrowserTreeVRacks.h"

@implementation LCBrowserTreeDocumentsRoot

#pragma mark "Constructors"

- (LCBrowserTreeDocumentsRoot *) init
{
	[super init];
	
	LCBrowserTreeScenes *scenes = [LCBrowserTreeScenes new];
	LCBrowserTreeVRacks *vracks = [LCBrowserTreeVRacks new];
	
	self.children = [NSMutableArray arrayWithObjects:scenes, vracks, nil];
	[scenes autorelease];
	[vracks autorelease];
	
	self.isBrowserTreeLeaf = NO;
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"DOCUMENTS";
}

@end
