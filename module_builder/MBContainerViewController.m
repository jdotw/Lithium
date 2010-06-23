//
//  MBContainerViewController.m
//  ModuleBuilder
//
//  Created by James Wilson on 7/09/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MBContainerViewController.h"
#import "MBDocumentWindowController.h"

@implementation MBContainerViewController

#pragma mark "Constructors"

+ (MBContainerViewController *) viewForContainer:(MBContainer *)initContainer
{
	return [[[MBContainerViewController alloc] initWithContainer:initContainer] autorelease];
}

- (MBContainerViewController *) initWithContainer:(MBContainer *)initContainer
{
	[super initWithNibName:@"ContainerView" bundle:nil];
	[self setRepresentedObject:initContainer];
	[self loadView];
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark "UI Actions"

- (IBAction) deleteContainerClicked:(id)sender
{
	[[self document] removeObjectFromContainersAtIndex:[[[self document] containers] indexOfObject:[self container]]];
}

#pragma mark "General Accessors"

- (MBContainer *) container
{ return [self representedObject]; }

- (ModuleDocument *) document
{ return [[self container] document]; }

- (MBDocumentWindowController *) mainWindowController
{ return (MBDocumentWindowController *) [[self document] mainWindowController]; }
 
@end
