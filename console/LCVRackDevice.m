//
//  LCVRackDevice.m
//  Lithium Console
//
//  Created by James Wilson on 11/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCVRackDevice.h"
#import "LCEntityDescriptor.h"

@implementation LCVRackDevice

#pragma mark Constructors

+ (LCVRackDevice *) rackDeviceWithEntity:(LCEntity *)targetEntity
{
	return [[[LCVRackDevice alloc] initWithEntity:targetEntity] autorelease]; 
}

- (id) initWithEntity:(LCEntity *)targetEntity
{
	self = [self init];
	if (!self) return nil;
	
	self.entity = targetEntity;

	return self;
}

- (id) init
{
	self = [super init];
	if (!self) return nil;

	/* Set Defaults */
	CFUUIDRef uuidRef = CFUUIDCreate(nil);
	self.uuid = (NSString *)CFUUIDCreateString(nil, uuidRef);
    CFRelease(uuidRef);		
	self.colour = [NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:1.0];
	
	/* Setup XML Translation */
	if (!self.xmlTranslation)
	{
		self.xmlTranslation = [NSMutableDictionary dictionary];
		[self.xmlTranslation setObject:@"hostRUindex" forKey:@"ru_index"];
		[self.xmlTranslation setObject:@"size" forKey:@"size"];
	}		
	
	return self;
}

- (void) dealloc
{
	[colour release];
	[entity release];
	[super dealloc];
}

#pragma mark "XML Methods"

- (void) setXmlValuesUsingXmlNode:(LCXMLNode *)xmlNode
{
	[super setXmlValuesUsingXmlNode:xmlNode];
	
	self.colour = [NSColor colorWithCalibratedRed:[[xmlNode.properties objectForKey:@"color_red"] floatValue]
											green:[[xmlNode.properties objectForKey:@"color_green"] floatValue]
											 blue:[[xmlNode.properties objectForKey:@"color_blue"] floatValue]
											alpha:1.0];
	
	for (LCXMLNode *childNode in xmlNode.children)
	{
		if ([childNode.name isEqualToString:@"entity_descriptor"])
		{
			LCEntityDescriptor *entityDescriptor = [LCEntityDescriptor descriptorWithXmlNode:childNode];
			self.entity = [entityDescriptor locateEntity:YES];
		}
	}	
}

- (NSXMLElement *) xmlNode
{
	NSXMLElement *rootNode = (NSXMLElement *) [super xmlNode];
	
	[rootNode addChild:[NSXMLNode elementWithName:@"uuid" stringValue:self.uuid]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"color_red" stringValue:[NSString stringWithFormat:@"%.2f", [colour redComponent]]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"color_green" stringValue:[NSString stringWithFormat:@"%.2f", [colour greenComponent]]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"color_blue" stringValue:[NSString stringWithFormat:@"%.2f", [colour blueComponent]]]]; 
	if (self.entity) [rootNode addChild:[[self.entity entityDescriptor] xmlNode]];
	
	return rootNode;	
}

- (NSString *) xmlRootElement
{ return @"device"; }

#pragma mark "Document Import"

- (LCVRackDevice *) initWithCoder:(NSCoder *)decoder
{
	/*
	 * This is only used for document importing 
	 */
	[self init];
	
	NSDictionary *properties = [decoder decodeObjectForKey:@"properties"];
	if ([properties objectForKey:@"colour"]) self.colour = [properties objectForKey:@"colour"];
	self.entity = [properties objectForKey:@"entity"];
	self.size = [[properties objectForKey:@"size"] intValue];
	self.hostRUindex = [[properties objectForKey:@"hostRUindex"] intValue];
	
    return self;
}

#pragma mark Properties

@synthesize uuid;
@synthesize colour;
- (NSColor *) colourWithAlpha:(float)alpha
{
	return [NSColor colorWithDeviceRed:[[self colour] redComponent] green:[[self colour] greenComponent] blue:[[self colour] blueComponent] alpha:alpha];
}
@synthesize entity;
@synthesize hostRUindex;
@synthesize size;
@synthesize hostRU;
- (void) setHostRU:(id)value
{
	[hostRU release];
	hostRU = [value retain];
	
	self.hostRUindex = ((LCVRackUnit *) hostRU).ruIndex;
}

@end
