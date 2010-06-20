//
//  LCBrowserTreeItemRoot.m
//  Lithium Console
//
//  Created by James Wilson on 21/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserTreeItemRoot.h"


@implementation LCBrowserTreeItemRoot

- (id) init
{
	self = [super init];
	if (self != nil) 
	{
		if ([[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithFormat:@"LCBrowserTree-%@-Index", self.displayString]])
		{ preferredIndex = [[NSUserDefaults standardUserDefaults] integerForKey:[NSString stringWithFormat:@"LCBrowserTree-%@-Index", self.displayString]];	}
		else
		{ preferredIndex = 1000; }
		
		if ([[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithFormat:@"LCBrowserTree-%@-Enabled", self.displayString]])
		{ enabled = [[NSUserDefaults standardUserDefaults] integerForKey:[NSString stringWithFormat:@"LCBrowserTree-%@-Enabled", self.displayString]]; }
		else
		{ enabled = YES; }
	}
	return self;
}

@synthesize preferredIndex;
- (void) setPreferredIndex:(int)value
{
	preferredIndex = value;
	[[NSUserDefaults standardUserDefaults] setInteger:preferredIndex forKey:[NSString stringWithFormat:@"LCBrowserTree-%@-Index", self.displayString]];
}

@synthesize enabled;
- (void) setEnabled:(BOOL)value
{
	enabled = value;
	[[NSUserDefaults standardUserDefaults] setInteger:enabled forKey:[NSString stringWithFormat:@"LCBrowserTree-%@-Enabled", self.displayString]];
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LCBrowserTreeItemRootEnabledChanged" object:self];
}
	

@end
