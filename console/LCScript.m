//
//  LCScript.m
//  Lithium Console
//
//  Created by James Wilson on 10/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCScript.h"


@implementation LCScript

#pragma mark "Constructors"

- (NSString *) xmlRootElement;
{
	return @"script";
}

- (id) initWithType:(NSString *)initType
{
	self = [super init];
	if (!self) return nil;
	
	self.type = initType;
	
	configVariables = [[NSMutableArray array] retain];
	configVariableDict = [[NSMutableDictionary dictionary] retain];
	
	/* Setup XML Translation */
	if (!self.xmlTranslation) self.xmlTranslation = [NSMutableDictionary dictionary];
	[self.xmlTranslation setObject:@"name" forKey:@"name"];
	[self.xmlTranslation setObject:@"desc" forKey:@"desc"];
	[self.xmlTranslation setObject:@"installedVersion" forKey:@"installed_version"];
	[self.xmlTranslation setObject:@"status" forKey:@"status"];
	[self.xmlTranslation setObject:@"type" forKey:@"type"];
	
	return self;	
}

- (void) dealloc
{
	[name release];
	[desc release];
	[info release];
	[installedVersion release];
	[statusIcon release];
	[statusString release];
	[type release];
	[configVariables release];
	[configVariableDict release];
	[super dealloc];
}

#pragma mark "Properties"

@synthesize name;
@synthesize desc;
@synthesize info;
@synthesize installedVersion;
@synthesize status;
- (void) setStatus:(int)value
{
	status = value;
	if (status == 1)
	{
		/* OK */
		self.statusIcon = [NSImage imageNamed:@"ok_16.tif"];
		self.statusString = @"OK";
	}
	else if (status == 2)
	{
		/* Error */
		self.statusIcon = [NSImage imageNamed:@"cancel_16.tif"];
		self.statusString = @"ERROR";
	}
	else
	{
		/* Other */
		self.statusIcon = [NSImage imageNamed:@"new_16.tif"];
		self.statusString = @"Unknown";
	}	
}
@synthesize statusIcon;
@synthesize statusString;
@synthesize type;
@synthesize configVariables;
- (void) insertObject:(LCScriptConfigVariable *)var inConfigVariablesAtIndex:(unsigned int)index
{
	[configVariables insertObject:var atIndex:index];
	[configVariableDict setObject:var forKey:[var name]];
}

- (void) removeObjectFromConfigVariablesAtIndex:(unsigned int)index
{
	if ([configVariables objectAtIndex:index])
	{ [configVariableDict removeObjectForKey:[[configVariables objectAtIndex:index] name]]; }
	[configVariables removeObjectAtIndex:index];
}
@synthesize configVariableDict;



@end
