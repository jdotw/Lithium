//
//  LCScriptedTask.m
//  Lithium Console
//
//  Created by James Wilson on 10/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCScriptedTask.h"


@implementation LCScriptedTask

#pragma mark "Constructors"

- (id) initWithHostEntity:(LCEntity *)initHostEntity
				 taskType:(NSString *)initType
{
	self = [self init];
	if (!self) return nil;
	
	self.hostEntity = initHostEntity;
	self.taskType = initType;
	
	return self;
}

- (id) init
{
	self = [super init];
	if (self != nil) 
	{
		/* Create config variables */
		self.configVariables = [[LCScriptConfigVariableList new] autorelease];
		configVariables.task = self;
		
		/* Setup XML Translation */
		if (!self.xmlTranslation) self.xmlTranslation = [NSMutableDictionary dictionary];
		[self.xmlTranslation setObject:@"taskID" forKey:@"id"];
		[self.xmlTranslation setObject:@"desc" forKey:@"desc"];
		[self.xmlTranslation setObject:@"enabled" forKey:@"enabled"];
		[self.xmlTranslation setObject:@"scriptName" forKey:@"script_file"];
	}
	return self;
}

- (id) mutableCopyWithZone:(NSZone *)zone
{
	LCScriptedTask *copy = [super mutableCopyWithZone:zone];
	
	copy.hostEntity = self.hostEntity;
	copy.taskType = self.taskType;
	
	return copy;
}

- (void) dealloc
{
	[desc release];
	[taskType release];
	[hostEntity release];
	[customer release];
	[scriptList release];
	[configVariables release];
	[super dealloc];
}

#pragma mark "XML Methods"

- (NSXMLElement *) xmlNode
{
	NSXMLElement *rootNode = [super xmlNode];
	
	for (LCScriptConfigVariable *var in [selectedScript configVariables])
	{ 
		[rootNode addChild:[var xmlNode]]; 
	}

	return rootNode;	
}

#pragma mark "Properties"

@synthesize taskID;
@synthesize desc;
@synthesize enabled;

@synthesize taskType;

@synthesize hostEntity;
- (void) setHostEntity:(LCEntity *)value
{
	[hostEntity release];
	hostEntity = [value retain];
	
	self.xmlEntity = hostEntity;
	
	if (!customer && customer != hostEntity.customer)
	{ self.customer = hostEntity.customer; }
}

@synthesize customer;
- (void) setCustomer:(LCCustomer *)value
{
	[customer release];
	customer = [value retain];
	if (!hostEntity && hostEntity != customer)
	{ self.hostEntity = customer; }
}	

@synthesize scriptName;
- (void) setScriptName:(NSString *)string
{
	[scriptName release];
	scriptName = [string copy];
	
	if (scriptName)
	{
		LCScript *script = [scriptList.scriptDict objectForKey:scriptName];
		if (!script)
		{
			script = [scriptList newScript];
			script.name = scriptName;
			script.desc = scriptName;
			[scriptList insertObject:script inScriptsAtIndex:scriptList.scripts.count];
			[script autorelease];
		}
		if (self.selectedScript != script) self.selectedScript = script;
	}
	else 
	{
		if (self.selectedScript) self.selectedScript = nil;
	}

}

@synthesize scriptList;
@synthesize selectedScript;
- (void) setSelectedScript:(LCScript *)value
{
	selectedScript = value;
	if (![self.scriptName isEqualToString:selectedScript.name])
	{ self.scriptName = selectedScript.name; }
	
	/* Ensure config variables are up to date */
	[self.configVariables updateScriptConfigVariables:selectedScript];
}

@synthesize configVariables;

@end
