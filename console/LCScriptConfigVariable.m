//
//  LCScriptConfigVariable.m
//  Lithium Console
//
//  Created by James Wilson on 10/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCScriptConfigVariable.h"
#import "LCScriptedTask.h"

@implementation LCScriptConfigVariable

#pragma mark "Constructors"

- (id) init
{
	self = [super init];
	if (!self) return nil;
	
	/* Setup XML Translation */
	if (!self.xmlTranslation) self.xmlTranslation = [NSMutableDictionary dictionary];
	[self.xmlTranslation setObject:@"name" forKey:@"name"];
	[self.xmlTranslation setObject:@"desc" forKey:@"desc"];
	[self.xmlTranslation setObject:@"value" forKey:@"value"];
	[self.xmlTranslation setObject:@"required" forKey:@"required"];
	
	return self;
}

- (void) dealloc
{
	[name release];
	[desc release];
	[value release];
	[requiredIcon release];
	[super dealloc];
}

#pragma mark "XML Methods"

- (NSString *) xmlRootElement
{
	return @"config_variable";
}

#pragma mark "Properties"

@synthesize task;
@synthesize name;
@synthesize desc;
@synthesize value;
- (void) setValue:(NSString *)string
{
	[value release];
	value = [string copy];
	
	if (value)		
	{ [[[(LCScriptedTask *)task configVariables] variableDict] setObject:value forKey:self.name]; }
	else
	{ [[[(LCScriptedTask *)task configVariables] variableDict] removeObjectForKey:[self name]]; }
}
@synthesize required;
- (void) setRequired:(BOOL)flag
{
	required = flag;
	if (self.required)
	{ self.requiredIcon = [NSImage imageNamed:@"BlueDot.tiff"]; }
	else
	{ self.requiredIcon = nil; }
}
@synthesize requiredIcon;

@end
