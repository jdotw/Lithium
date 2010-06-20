//
//  LCInspectorViewController.m
//  Lithium Console
//
//  Created by James Wilson on 2/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspectorViewController.h"


@implementation LCInspectorViewController

#pragma mark "Constructor"

- (LCInspectorViewController *) initWithTarget:(id)initTarget
{
	[super initWithTarget:initTarget];		/* Item Init */
	
	NSNumber *preferredHeight = [[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithFormat:@"%@_PreferredHeight", [self className]]];
	if (preferredHeight)
	{ rowHeight = [preferredHeight floatValue]; }
	else
	{ rowHeight = [self defaultHeight]; }
	
	return self;
}

#pragma mark "Tree Methods"

- (BOOL) leafNode
{ return YES; }

#pragma mark "Accessors"

- (float) defaultHeight
{ return 40.0; }

- (float) rowHeight
{ 
	if (rowHeight < 1) return 2;
	return rowHeight; 
}

- (void) setRowHeight:(float)height
{ 
	if (height < 5.0) return;
	rowHeight = height; 
	[[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithFloat:height] 
											  forKey:[NSString stringWithFormat:@"%@_PreferredHeight", [self className]]];
}

@end
