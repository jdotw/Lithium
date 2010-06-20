//
//  LCEntityViewController.m
//  Lithium Console
//
//  Created by James Wilson on 8/12/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCEntityViewController.h"

#import "LCContainer.h"

@implementation LCEntityViewController

#pragma mark "Initialisation"

+ (LCEntityViewController *) controllerForEntity:(LCEntity *)initEntity
{
	return [[[LCEntityViewController alloc] initForEntity:initEntity] autorelease];
}

- (LCEntityViewController *) initForEntity:(LCEntity *)initEntity
{
	[self init];

	[self setEntity:initEntity];
	[self loadNIB];
	
	return self;
}

- (LCEntityViewController *) initGenericForEntity:(LCEntity *)initEntity
{
	[self init];
	[self setEntity:initEntity];
	[NSBundle loadNibNamed:@"GenericEntityView" owner:self]; 
	[self setUsingGeneric:YES];
	return self;
}

- (LCEntityViewController *) init
{
	[super init];
	
	return self;
}

- (void) dealloc
{
	[view release];
	[objectController release];
	[entity release];
	[super dealloc];
}

- (void) removeViewAndContent
{
	[view removeFromSuperview];
	[objectController setContent:nil];
}

#pragma mark NIB Loading

- (void) loadNIB
{
	NSString *entityPrefix = nil;
	
	/* Load entity-specfic NIB */
	switch ([[entity typeInteger] intValue])
	{
		case 1:
			entityPrefix = @"cust";
			break;
		case 2:
			entityPrefix = @"site";
			break;
		case 3:
			entityPrefix = @"dev";
			break;
		case 4:
			entityPrefix = @"cnt";
			break;
		case 5:
			entityPrefix = @"obj";
			break;
		case 6:
			entityPrefix = @"met";
			break;
		case 7:
			entityPrefix = @"trg";
			break;
	}
	
	if (entityPrefix)
	{ 
		if ([[[entity parent] name] hasPrefix:@"xrarray"])
		{
			[NSBundle loadNibNamed:@"obj_xrarrays" owner:self]; 
		}
		else if ([[[entity parent] name] hasPrefix:@"xrhostiface"])
		{
			[NSBundle loadNibNamed:@"obj_xrhostifaces" owner:self]; 
		}
		else if ([[[entity parent] name] hasPrefix:@"xsansplun_"])
		{
			[NSBundle loadNibNamed:@"obj_xsannode" owner:self]; 
		}
		else if ([[[entity parent] name] hasPrefix:@"xsanvolsp_"])
		{
			[NSBundle loadNibNamed:@"obj_xsansp" owner:self]; 
		}
		else if ([[[entity parent] name] hasPrefix:@"xsanvoldetail"])
		{
			[NSBundle loadNibNamed:@"obj_xsanvoldetail" owner:self]; 
		}
		else if ([[entity typeInteger] intValue] == 3)
		{
			/* Device */
			[NSBundle loadNibNamed:[NSString stringWithFormat:@"%@_%@", entityPrefix, [[entity properties] objectForKey:@"vendor"]] owner:self]; 
			if (!view)
			{ [NSBundle loadNibNamed:@"GenericDeviceView" owner:self]; }
		}
		if ([entity type] == 5 && [(LCContainer *) [entity parent] isModuleBuilder])
		{
			[NSBundle loadNibNamed:@"obj_modb" owner:self]; 
		}
		else
		{
			[NSBundle loadNibNamed:[NSString stringWithFormat:@"%@_%@", entityPrefix, [[entity parent] name]] owner:self]; 
		}
	}
	if (!view)
	{ 
		[NSBundle loadNibNamed:@"GenericEntityView" owner:self]; 
		[self setUsingGeneric:YES];
	}
}

#pragma mark Accessor Methods

- (LCEntity *) entity
{ return entity; }
- (void) setEntity:(LCEntity *)newEntity
{
	if (entity) [entity release];
	entity = [newEntity retain]; 
}

- (NSView *) view
{ return view; }

- (BOOL) usingGeneric
{ return usingGeneric; }
- (void) setUsingGeneric:(BOOL)flag
{ usingGeneric = flag; }

@synthesize view;
@synthesize objectController;
@end
