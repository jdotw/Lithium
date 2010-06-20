//
//  LCActionEntityList.m
//  Lithium Console
//
//  Created by James Wilson on 25/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCActionEntityList.h"
#import "LCEntityDescriptor.h"
#import "LCAction.h"

@implementation LCActionEntityList

#pragma mark "Constructors"

- (id) initWithAction:(id)initAction
{
	self = [super initWithSource:[(LCAction *)initAction hostEntity]
						 xmlName:@"action_entity_list"
						forClass:[LCEntity class]
				   usingIndexKey:nil];
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

#pragma mark "Accessor Methods"
@synthesize action;

@end
