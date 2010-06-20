//
//  LCSSceneOverlay.m
//  Lithium Console
//
//  Created by James Wilson on 27/08/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCSSceneOverlay.h"
#import "LCEntityDescriptor.h"

@implementation LCSSceneOverlay

#pragma mark Constructors

+ (LCSSceneOverlay *) overlayWithFrame:(NSRect)initFrame
{
	return [[[LCSSceneOverlay alloc] initWithFrame:initFrame] autorelease];
}

- (LCSSceneOverlay *) initWithFrame:(NSRect)initFrame
{
	[self init];
	
	self.frame = initFrame;
	
	return self;
}

- (id) init
{
	self = [super init];
	if (!self) return nil;

	/* Create initial UUID */
	CFUUIDRef uuidRef = CFUUIDCreate(nil);
	self.uuid = (NSString *)CFUUIDCreateString(nil, uuidRef);
    CFRelease(uuidRef);	
	
	/* Setup XML Translation */
	if (!self.xmlTranslation)
	{
		self.xmlTranslation = [NSMutableDictionary dictionary];
		[self.xmlTranslation setObject:@"autoSize" forKey:@"autosize"];
		[self.xmlTranslation setObject:@"uuid" forKey:@"uuid"];
	}		

	return self;
}

- (void) dealloc
{
	[uuid release];
	[entity release];
	[super dealloc];
}

//#pragma mark "Encoding"
//- (void)encodeWithCoder:(NSCoder *)encoder
//{
//	[encoder encodeObject:properties forKey:@"properties"];
//}

#pragma mark "XML Methods"

- (void) setXmlValuesUsingXmlNode:(LCXMLNode *)xmlNode
{
	[super setXmlValuesUsingXmlNode:xmlNode];
	
	self.frame = NSMakeRect([[xmlNode.properties objectForKey:@"x"] floatValue], [[xmlNode.properties objectForKey:@"y"] floatValue],
							[[xmlNode.properties objectForKey:@"width"] floatValue], [[xmlNode.properties objectForKey:@"height"] floatValue]);
	
	for (LCXMLNode *childNode in xmlNode.children)
	{
		if ([childNode.name isEqualToString:@"entity_descriptor"])
		{
			LCEntityDescriptor *entityDescriptor = [LCEntityDescriptor descriptorWithXmlNode:childNode];
			self.entity = [entityDescriptor locateEntity:YES];
		}
	}	
}

- (NSXMLNode *) xmlNode
{
	NSXMLElement *rootNode = (NSXMLElement *) [NSXMLNode elementWithName:@"overlay"];
	
	[rootNode addChild:[NSXMLNode elementWithName:@"uuid" stringValue:self.uuid]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"x" stringValue:[NSString stringWithFormat:@"%.2f", NSMinX(self.frame)]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"y" stringValue:[NSString stringWithFormat:@"%.2f", NSMinY(self.frame)]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"width" stringValue:[NSString stringWithFormat:@"%.2f", NSWidth(self.frame)]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"height" stringValue:[NSString stringWithFormat:@"%.2f", NSHeight(self.frame)]]]; 
	[rootNode addChild:[NSXMLNode elementWithName:@"autosize" stringValue:[NSString stringWithFormat:@"%i", self.autoSize]]]; 
	if (self.entity) [rootNode addChild:[[self.entity entityDescriptor] xmlNode]];
	
	return rootNode;	
}

- (NSString *) xmlRootElement
{ return @"overlay"; }

#pragma mark "Document Import"

- (LCSSceneOverlay *) initWithCoder:(NSCoder *)decoder
{
	/*
	 * This is only used for document importing 
	 */
	[self init];
	
	NSDictionary *properties = [decoder decodeObjectForKey:@"properties"];
	self.frame = NSRectFromString([properties objectForKey:@"frame"]);
	self.entity = [properties objectForKey:@"entity"];
	self.autoSize = [[properties objectForKey:@"autosize"] intValue];
	
    return self;
}

#pragma mark Accessors

@synthesize frame;
@synthesize entity;
@synthesize autoSize;
@synthesize uuid;

+ (NSSize) minimumSize
{ return NSMakeSize(16, 16); }

@end
