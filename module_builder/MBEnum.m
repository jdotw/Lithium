//
//  MBEnum.m
//  ModuleBuilder
//
//  Created by James Wilson on 12/05/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "MBEnum.h"


@implementation MBEnum

- (id) initWithLabel:(NSString *)label forValue:(NSString *)value
{
	self  = [super init];
	self.properties = [NSMutableDictionary dictionary];
	self.label = label;
	self.value = value;
	return self;
}

+ (MBEnum *) enumWithLabel:(NSString *)label forValue:(NSString *)value
{
	return [[[MBEnum alloc] initWithLabel:label forValue:value] autorelease];
}

- (MBEnum *) initWithCoder:(NSCoder *)decoder
{
	[self init];
	self.properties = [decoder decodeObjectForKey:@"properties"];
	return self;
}

- (void) encodeWithCoder:(NSCoder *)encoder
{
	[encoder encodeObject:properties forKey:@"properties"];
}

- (void) dealloc
{
	[properties release];
	[super dealloc];
}

#pragma mark "XML Output"

- (NSXMLNode *) xmlNode
{
	/* Create our root node */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"enumerator"];
	[rootnode addChild:[NSXMLNode elementWithName:@"value" 
									  stringValue:self.value]];
	[rootnode addChild:[NSXMLNode elementWithName:@"label" 
									  stringValue:self.label]];
	return rootnode;
}

@synthesize properties;

- (NSString *) value
{ return [self.properties objectForKey:@"value"]; }

- (void) setValue:(NSString *)string
{
	[[[document undoManager] prepareWithInvocationTarget:self] setLabel:[self value]];
	if (string) [self.properties setObject:string forKey:@"value"];
	else [self.properties removeObjectForKey:@"value"]; 
	[[document undoManager] setActionName:@"Set Enumerator Value"];
}

- (NSString *) label
{ return [self.properties objectForKey:@"label"]; }

- (void) setLabel:(NSString *)string
{
	[[[document undoManager] prepareWithInvocationTarget:self] setLabel:[self label]];
	if (string) [self.properties setObject:string forKey:@"label"];
	else [self.properties removeObjectForKey:@"label"]; 
	[[document undoManager] setActionName:@"Set Enumerator Label"];
}

@synthesize document;

@end
