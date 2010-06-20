//
//  LCActionHistoryList.m
//  Lithium Console
//
//  Created by James Wilson on 3/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCActionHistoryList.h"
#import "LCAction.h"
#import "LCCustomer.h"

@implementation LCActionHistoryList

#pragma mark "Constructors"

+ (LCActionHistoryList *) actionHistoryListForAction:(id)initAction
{
	return [[[LCActionHistoryList alloc] initWithAction:initAction] autorelease];
}

- (id) initWithAction:(id)initAction
{
	self = [super initWithSource:((LCAction *)initAction).hostEntity
						 xmlName:@"action_history_list"
						forClass:[LCActionLogEntry class]
				   usingIndexKey:@"logID"];
	if (!self) return nil;
	
	self.action = initAction;
	
	return self;
}

- (void) dealloc 
{
	[super dealloc];
}

#pragma mark "Refresh methods"

- (NSXMLElement *) criteriaXmlNode
{
	NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"action"];
	[rootNode addChild:[NSXMLNode elementWithName:@"actionid" 
									  stringValue:[NSString stringWithFormat:@"%i", [(LCAction *)action taskID]]]];		
	return rootNode;
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	if ([sender success]) self.hasBeenRefreshed = YES;
	
	[super XMLRequestFinished:sender];
}

#pragma mark "Accessor Methods"
@synthesize action;
@synthesize hasBeenRefreshed;

@end
