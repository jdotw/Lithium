//
//  MBOutlineViewController.m
//  ModuleBuilder
//
//  Created by James Wilson on 8/03/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MBOutlineViewController.h"

#import "MBEntity.h"

@implementation MBOutlineViewController

#pragma mark "Row height"

- (float) rowHeight
{ 
	float height = [[self view] frame].size.height;

	MBEntity *entity = [self representedObject];
	if ([entity infoViewVisible])
	{
		if (prefsView)
		{ 
			height = height + [prefsView frame].size.height; 
		}
		if (customView)
		{ height = height + [customView frame].size.height; }
	}
	
	
	return height;
}

#pragma mark "UI Actions"

- (IBAction) showInfoClicked:(id)sender
{
	MBEntity *entity = [self representedObject];
	if ([entity infoViewVisible])
	{ [entity setInfoViewVisible:NO]; }
	else
	{ [entity setInfoViewVisible:YES]; }	
	
	[[NSNotificationCenter defaultCenter] postNotificationName:@"ResetItYo" object:self];
}

#pragma mark "Properties"
@synthesize prefsView;
@synthesize customView;

@end
