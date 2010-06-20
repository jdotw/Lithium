//
//  LCPropertiesViewerController.m
//  Lithium Console
//
//  Created by James Wilson on 12/12/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCPropertiesViewerController.h"


@implementation LCPropertiesViewerController

#pragma mark "Initialisation"

- (LCPropertiesViewerController *) initWithDictionary:(NSDictionary *)dictionary
{
	/* Setup properties */
	for (NSString *key in [dictionary allKeys])
	{
		NSMutableDictionary *entry = [NSMutableDictionary dictionary];
		[entry setObject:key forKey:@"key"];
		[entry setObject:[dictionary objectForKey:key] forKey:@"value"];
		[properties addObject:entry];
	}
	
	/* Load NIB */
	[super initWithWindowNibName:@"PropertiesViewer"];
	[[self window] makeKeyAndOrderFront:self];
	
	return self;
}

- (void) dealloc
{
	[properties release];
	[super dealloc];
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Unbind controllerAlias */
	[controllerAlias setContent:nil];
	
	/* Autorelease */
	[self autorelease];
}

@synthesize properties;
@synthesize controllerAlias;
@end
