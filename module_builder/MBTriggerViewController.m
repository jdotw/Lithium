//
//  MBTriggerViewController.m
//  ModuleBuilder
//
//  Created by James Wilson on 6/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MBTriggerViewController.h"


@implementation MBTriggerViewController

#pragma mark "Constructors"

+ (MBTriggerViewController *) viewForTrigger:(MBTrigger *)initTrigger
{
	return [[[MBTriggerViewController alloc] initWithTrigger:initTrigger] autorelease];
}

- (MBTriggerViewController *) initWithTrigger:(MBTrigger *)initTrigger
{
	[super initWithNibName:@"TriggerView" bundle:nil];
	[self setRepresentedObject:initTrigger];
	[self loadView];
	
	return self;
}

- (void) dealloc
{
	[[self view] removeFromSuperview];
	[super dealloc];
}

#pragma mark Accessors

- (MBTrigger *) trigger;
{ return [self representedObject]; }


@end
