//
//  LCBrowser2ContentViewController.m
//  Lithium Console
//
//  Created by James Wilson on 26/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowser2ContentViewController.h"


@implementation LCBrowser2ContentViewController

- (id) init
{
	self = [super init];
	if (self)
	{
		self.sortStringDescriptors = [NSArray arrayWithObject:[[NSSortDescriptor alloc] initWithKey:@"sortString" ascending:YES]];
	}
	return self;
}

- (BOOL) treeSelectionCanChangeToRepresent:(id)obj
{
	return YES;
}

- (void) removedFromBrowserWindow
{
	/* Shutdown and prepare to be autoreleased */
	[controllerAlias setContent:nil];
	[self autorelease];
}

- (void) selectEntity:(LCEntity *)entity
{
	
}

- (void) selectIncident:(LCIncident *)incident
{
	
}

@synthesize statusBarView;
@synthesize bottomView;
@synthesize inspectorView;

- (CGFloat) preferredFixedComponentHeight
{ return 280.0; }

- (CGFloat) preferredInspectorWidth
{ return 251.0; }

@synthesize resizeMode;

@synthesize sortStringDescriptors;

@end
