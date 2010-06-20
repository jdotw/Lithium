//
//  LCScriptConfigVariableList.m
//  Lithium Console
//
//  Created by James Wilson on 10/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCScriptConfigVariableList.h"

#import "LCScript.h"
#import "LCXMLNode.h"
#import "LCScriptedTask.h"

@implementation LCScriptConfigVariableList

#pragma mark "Constructors"

- (id) init
{
	self = [super init];
	if (self != nil) {
		variableDict = [[NSMutableDictionary dictionary] retain];
	}
	return self;
}

- (void) dealloc
{
	[variableDict release];
	if (refreshXMLRequest)
	{
		[refreshXMLRequest cancel];
		[refreshXMLRequest release];
	}
	[super dealloc];
}

#pragma mark "Refresh methods"

- (void) refreshWithPriority:(int)priority
{
	/* Check state */
	if (!task) return;
	if (refreshXMLRequest) return;
	
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"task"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];	
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:[NSString stringWithFormat:@"%@id", ((LCScriptedTask *)task).taskType]
									  stringValue:[NSString stringWithFormat:@"%i", ((LCScriptedTask *)task).taskID]]];	
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:[((LCScriptedTask *)task).hostEntity customer]
												  resource:[((LCScriptedTask *)task).hostEntity resourceAddress] 
													entity:[((LCScriptedTask *)task).hostEntity entityAddress] 
												   xmlname:[NSString stringWithFormat:@"%@_configvar_list", ((LCScriptedTask *)task).taskType]
													refsec:0 
													xmlout:xmldoc] retain];
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
	for (NSString *key in rootNode.properties)
	{
		/* Set value in local dictionary */
		[variableDict setObject:[rootNode.properties objectForKey:key] forKey:key];		
	}
	
	/* Update configured value for script */
	for (LCScript *script in ((LCScriptedTask *)task).scriptList.scripts)
	{
		[self updateScriptConfigVariables:script];
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
	
	/* Set Flag */
	self.refreshInProgress = NO;
}

#pragma mark "Script Update"

- (void) updateScriptConfigVariables:(id)script
{
	for (NSString *key in [variableDict allKeys])
	{ 
		LCScriptConfigVariable *variable = [((LCScript *)script).configVariableDict objectForKey:key];
		if (!variable && !((LCScript *)script).installedVersion)
		{
			/* The variable isn't known *AND* the script does not 
			 * appear to have been refreshed (installedVersion is
			 * now known, create the variable knowing that a refrsh
			 * is probably inprogress to find the actual config variables
			 */
			variable = [LCScriptConfigVariable new];
			variable.name = key;
			[((LCScript *)script) insertObject:variable inConfigVariablesAtIndex:((LCScript *)script).configVariables.count];
			[variable autorelease];
		}
		variable.value = [variableDict objectForKey:key];
	}
	
}

#pragma mark "Properties"

@synthesize task;
@synthesize variableDict;
@synthesize refreshInProgress;

@end
