//
//  LCScriptList.m
//  Lithium Console
//
//  Created by James Wilson on 10/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCScriptList.h"


@implementation LCScriptList

#pragma mark "Constructor"

- (id) initWithCustomer:(LCCustomer *)initCustomer task:(id)initTask
{
	self = [super init];
	if (!self) return nil;
	
	self.customer = initCustomer;
	self.task = initTask;
	scripts = [[NSMutableArray array] retain];
	scriptDict = [[NSMutableDictionary dictionary] retain];
	
	return self;
}

- (void) dealloc
{
	if (refreshXMLRequest)
	{ 
		[refreshXMLRequest cancel]; 
		[refreshXMLRequest release]; 
	}
	[scripts release];
	[scriptDict release];
	[customer release];
	[super dealloc];
}

#pragma mark "Refresh methods"

- (void) refreshWithPriority:(int)priority
{
	/* Check state */
	if (!customer) return;
	if (refreshInProgress) return;
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:[customer customer]
												  resource:[customer resourceAddress] 
													entity:[customer entityAddress] 
												   xmlname:[NSString stringWithFormat:@"%@_script_list", self.scriptType]
													refsec:0 
													xmlout:nil] retain];
	[refreshXMLRequest setDelegate:self];
	[refreshXMLRequest setThreadedXmlDelegate:self];
	[refreshXMLRequest setPriority:priority];
	[refreshXMLRequest performAsyncRequest];
	self.refreshInProgress = YES;
}

- (void) highPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_HIGH]; }

- (void) normalPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_NORMAL]; }

- (void) lowPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_LOW]; }

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	NSMutableArray *seenScripts = [NSMutableArray array];
	for (LCXMLNode *scriptNode in rootNode.children)
	{
		if ([scriptNode.name isEqualToString:@"script"])
		{
			/* Process Script */
			LCScript *script = [scriptDict objectForKey:[scriptNode.properties objectForKey:@"name"]];
			if (!script)
			{
				script = [self newScript];
				script.name = [scriptNode.properties objectForKey:@"name"];
				[self insertObject:script inScriptsAtIndex:scripts.count];
				[script autorelease];
			}
			[script setXmlValuesUsingXmlNode:scriptNode];
			[seenScripts addObject:script];
			
			/* Process Scripts Config Variables */
			NSMutableArray *seenVars = [NSMutableArray array];
			for (LCXMLNode *varNode in scriptNode.children)
			{
				if ([varNode.name isEqualToString:@"config_variable"])
				{
					LCScriptConfigVariable *configVar = [script.configVariableDict objectForKey:[varNode.properties objectForKey:@"name"]];
					if (!configVar)
					{
						configVar = [LCScriptConfigVariable new];
						configVar.name = [varNode.properties objectForKey:@"name"];
						[script insertObject:configVar inConfigVariablesAtIndex:script.configVariables.count];
						[configVar autorelease];
					}
					[configVar setXmlValuesUsingXmlNode:varNode];
					[seenVars addObject:configVar];
				}
			}
			NSMutableArray *removeVars = [NSMutableArray array];
			for (LCScriptConfigVariable *configVar in script.configVariables)
			{
				if (![seenVars containsObject:configVar])
				{ [removeVars addObject:configVar]; }
			}
			for (LCScriptConfigVariable *configVar in removeVars)
			{
				[script removeObjectFromConfigVariablesAtIndex:[script.configVariables indexOfObject:configVar]];
			}
		}		
	}
	
	NSMutableArray *removeArray = [NSMutableArray array];
	for (LCScript *script in scripts)
	{
		if (![seenScripts containsObject:script])
		{
			[removeArray addObject:script];
		}
	}
	for (LCScript *script in removeArray)
	{
		[self removeObjectFromScriptsAtIndex:[scripts indexOfObject:script]];		
	}		
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Free XML request */
	if (sender == refreshXMLRequest)
	{
		[refreshXMLRequest release];
		refreshXMLRequest = nil;
	}
	
	/* Delegate */
	if ([delegate respondsToSelector:@selector(scriptListRefreshFinished:)])
	{
		[delegate performSelector:@selector(scriptListRefreshFinished:) 
					   withObject:self];
	}		
	
	/* Set Flag */
	self.refreshInProgress = NO;
}

#pragma mark "Sub-Class Methods (to be overidden)"

- (LCScript *) newScript
{
	return nil;
}

- (NSString *) scriptType
{ 
	return nil; 
}

#pragma mark "Properties"

@synthesize customer;
@synthesize task;
@synthesize delegate;
@synthesize scriptType;
@synthesize scripts;
- (void) insertObject:(LCScript *)script inScriptsAtIndex:(unsigned int)index
{ 
	[scripts insertObject:script atIndex:index];
	[scriptDict setObject:script forKey:script.name];
}
- (void) removeObjectFromScriptsAtIndex:(unsigned int)index
{
	LCScript *script = [scripts objectAtIndex:index];
	[scriptDict removeObjectForKey:script.name];
	[scripts removeObjectAtIndex:index];
}
@synthesize scriptDict;
@synthesize refreshInProgress;


@end
